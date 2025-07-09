#include "httpconnection.h"
#include "httpparser/httpparser.h"
#include "epollreactor/epollreactor.h"
#include <unistd.h> // for read/write
#include <iostream>
#include <sys/epoll.h>

// --- HttpResponse Implementation ---
void HttpResponse::set_status_code(int code, std::string_view message) {
    status_code_ = code;
    status_message_ = std::string(message);
}

void HttpResponse::add_header(std::string_view key, std::string_view value) {
    headers_[std::string(key)] = std::string(value);
}

void HttpResponse::set_body(std::string_view body) {
    body_ = std::string(body);
}

std::string HttpResponse::to_string() {
    std::string s = "HTTP/1.1 " + std::to_string(status_code_) + " " + status_message_ + "\r\n";
    headers_["Content-Length"] = std::to_string(body_.size());
    for (const auto& h : headers_) {
        s += h.first + ": " + h.second + "\r\n";
    }
    s += "\r\n";
    s += body_;
    return s;
}

// --- HttpConnection Implementation ---
void HttpConnection::init(socketRAII&& sock, EpollReactor* reactor, Router* router) {
    sock_ = std::move(sock);
    reactor_ = reactor;
    router_ = router;
    read_buffer_.resize(2048);
    reactor_->add_fd(sock_.get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void HttpConnection::handle_events(uint32_t events) {
    if (to_be_closed_) return;

    if (events & (EPOLLHUP | EPOLLERR)) {
        close_conn();
        return;
    }
    if (events & EPOLLIN) {
        handle_read();
    }
    if (events & EPOLLOUT) {
        handle_write();
    }
}

void HttpConnection::handle_read() {
    ssize_t bytes_read = 0;
    while (true) {
        bytes_read = read(sock_.get_fd(), read_buffer_.data(), read_buffer_.size());
        if (bytes_read > 0) {
            request_.raw_data.append(read_buffer_.data(), bytes_read);
        } else if (bytes_read == 0) { // Client closed connection
            close_conn();
            return;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Finished reading for now
                break;
            } else { // Error
                close_conn();
                return;
            }
        }
    }

    HttpParser::ParseResult result = HttpParser::parse_request(request_.raw_data, request_);
    if (result == HttpParser::Good) {
        HttpResponse response;
        router_->route(request_, response);
        std::string response_str = response.to_string();
        write_buffer_.append(response_str);
        reactor_->mod_fd(sock_.get_fd(), EPOLLIN | EPOLLOUT | EPOLLET | EPOLLONESHOT);
    } else if (result == HttpParser::Bad) {
        close_conn();
    } // If Incomplete, do nothing and wait for more data
}

void HttpConnection::handle_write() {
    if (write_buffer_.empty()) {
        reactor_->mod_fd(sock_.get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
        return;
    }

    ssize_t bytes_written = 0;
    while (!write_buffer_.empty()) {
        bytes_written = write(sock_.get_fd(), write_buffer_.data(), write_buffer_.size());
        if (bytes_written > 0) {
            write_buffer_.consume(bytes_written);
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                close_conn();
            }
            return; // Wait for next EPOLLOUT
        }
    }

    // Finished writing, re-register for reading only
    reactor_->mod_fd(sock_.get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void HttpConnection::close_conn() {
    if (!to_be_closed_) {
        reactor_->del_fd(sock_.get_fd());
        sock_.reset(); // Closes the fd via RAII
        to_be_closed_ = true;
    }
}

int HttpConnection::get_fd() const {
    return sock_.get_fd();
}

bool HttpConnection::is_to_be_closed() const {
    return to_be_closed_;
}

void HttpConnection::set_to_be_closed() {
    to_be_closed_ = true;
}
