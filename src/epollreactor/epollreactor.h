#ifndef EPOLLREACTOR_H
#define EPOLLREACTOR_H

#include <vector>
#include <sys/epoll.h>
#include "socketRAII/socketRAII.h"

class EpollReactor
{
public:
    explicit EpollReactor(int max_events = 1024);

    void add_fd(int fd, uint32_t events);

    void mod_fd(int fd, uint32_t events);

    void del_fd(int fd);

    int wait(int timeout_ms);

    const epoll_event &get_event(int i) const;

private:
    socketRAII epoll_fd_;
    std::vector<epoll_event> events_;
};

#endif //EPOLLREACTOR_H
