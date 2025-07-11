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
    inline void add_fd(int,uint32_t);
    inline void mod_fd(int,uint32_t);
    inline void del_fd(int);
    [[nodiscard]] inline int wait(int);
    [[nodiscard]] inline int get_fd(int) const;
    [[nodiscard]] inline uint32_t get_events(int) const;
};



#endif //EPOLL_H
