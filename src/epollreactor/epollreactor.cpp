#include "epollreactor.h"
#include <unistd.h> // for close
#include <stdexcept> // for std::runtime_error

EpollReactor::EpollReactor(int max_events)
    : epoll_fd_(epoll_create1(0)), events_(max_events) {
    if (!epoll_fd_.is_alive()) {
        throw std::runtime_error("epoll_create1 failed");
    }
}

void EpollReactor::add_fd(int fd, uint32_t events) {
    epoll_event event{};
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epoll_fd_.get_fd(), EPOLL_CTL_ADD, fd, &event) == -1) {
        throw std::runtime_error("epoll_ctl: add failed");
    }
}

void EpollReactor::mod_fd(int fd, uint32_t events) {
    epoll_event event{};
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epoll_fd_.get_fd(), EPOLL_CTL_MOD, fd, &event) == -1) {
        throw std::runtime_error("epoll_ctl: mod failed");
    }
}

void EpollReactor::del_fd(int fd) {
    // We don't check for errors here because the fd might have been closed already
    epoll_ctl(epoll_fd_.get_fd(), EPOLL_CTL_DEL, fd, nullptr);
}

int EpollReactor::wait(int timeout_ms) {
    return epoll_wait(epoll_fd_.get_fd(), events_.data(), events_.size(), timeout_ms);
}

const epoll_event& EpollReactor::get_event(int i) const {
    return events_[i];
}
