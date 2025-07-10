//
// Created by kid314 on 25-7-10.
//

#include "Epoll.h"

#include <stdexcept>

Epoll::Epoll(int maxEvents_num):epoll_fd(epoll_create1(0)),epoll_events(maxEvents_num)
{
    if(epoll_fd<0)
    {
        throw std::runtime_error("epoll_create1 failed");
    }
}
Epoll::~Epoll()
{
    close(epoll_fd);
}
void Epoll::add_fd(int fd,uint32_t events)
{
    if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,))
}
void Epoll::mod_fd(int fd,uint32_t events)
{

}
void Epoll::del_fd(int fd)
{

}
int Epoll::wait(int)
{

}