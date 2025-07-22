//
// Created by kid314 on 25-7-10.
//

#ifndef EPOLL_H
#define EPOLL_H
#include <cstdint>
#include <vector>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>
class Epoll
{
private:
    int epoll_fd;
    std::vector<struct epoll_event> epoll_events;
public:
    explicit Epoll(int);
    ~Epoll();
    Epoll(const Epoll&) = delete;
    Epoll& operator=(const Epoll&) = delete;
    Epoll(Epoll&&) = delete;
    Epoll& operator=(Epoll&&) = delete;
    void add_fd(int,uint32_t);
    void mod_fd(int,uint32_t);
    void del_fd(int);
    [[nodiscard]] int wait(int);
    [[nodiscard]] int get_fd(int) const;
    [[nodiscard]] uint32_t get_events(int) const;
};



#endif //EPOLL_H
