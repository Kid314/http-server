#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include "socketRAII/socketRAII.h"
#include "sendbuffer/sendbuffer.h"
#include "router/router.h"

// Represents an HTTP Request
struct HttpRequest
{
    std::string_view method;
    std::string_view path;
    std::string_view version;
    std::map<std::string_view, std::string_view> headers;
    std::string_view body;
    std::string raw_data; // To hold the actual data for string_views
};

// Represents an HTTP Response
class HttpResponse
{
public:
    void set_status_code(int code, std::string_view message);
    void add_header(std::string_view key, std::string_view value);
    void set_body(std::string_view body);
    std::string to_string();

private:
    int status_code_ = 200;
    std::string status_message_ = "OK";
    std::map<std::string, std::string> headers_;
    std::string body_;
};

class HttpConnection
{
public:
    HttpConnection() = default;
    void init(socketRAII&& sock, class EpollReactor* reactor, Router* router);
    void handle_events(uint32_t events);
    int get_fd() const;
    bool is_to_be_closed() const;
    void set_to_be_closed();

private:
    void handle_read();
    void handle_write();
    void close_conn();

    socketRAII sock_;
    std::vector<char> read_buffer_;
    SendBuffer write_buffer_;
    HttpRequest request_;
    class EpollReactor* reactor_ = nullptr;
    Router* router_ = nullptr;
    bool to_be_closed_ = false;
};

#endif //HTTPCONNECTION_H
