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
    struct epoll_event temp_event{};
    temp_event.data.fd=fd;
    temp_event.events=events;
    if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&temp_event)<0)
    {
        throw std::runtime_error("epoll add failed");
    }
}
void Epoll::mod_fd(int fd,uint32_t events)
{
    struct epoll_event temp_event{};
    temp_event.data.fd=fd;
    temp_event.events=events;
    if (epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&temp_event)<0)
    {
        throw std::runtime_error("epoll mod failed");
    }
}
void Epoll::del_fd(int fd)
{
    if (epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,nullptr)<0)
    {
        throw std::runtime_error("epoll del failed");
    }
}
int Epoll::wait(int time_op)
{
    int events_num=epoll_wait(epoll_fd,epoll_events.data(),static_cast<int>(epoll_events.size()),time_op);
    if (events_num<0)
    {
        throw std::runtime_error("epoll wait failed");
    }
    return events_num;
}
[[nodiscard]] int Epoll::get_fd(int index) const
{
    if (index<0||index>=epoll_events.size())
    {
        throw std::runtime_error("get_fd::out of epoll_events range");
    }
    return epoll_events[index].data.fd;
}
[[nodiscard]] uint32_t Epoll::get_events(int index) const
{
    if (index<0||index>=epoll_events.size())
    {
        throw std::runtime_error("get_events::out of epoll_events range");
    }
    return epoll_events[index].events;
}

