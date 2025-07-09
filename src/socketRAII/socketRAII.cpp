//
// Created by kid314 on 25-7-7.
//

#include "socketRAII.h"

socketRAII::socketRAII(int new_fd) : fd_(new_fd) {}

socketRAII::~socketRAII()
{
    if (is_alive())
    {
        ::close(fd_);
    }
}

socketRAII::socketRAII(socketRAII&& other) noexcept : fd_(-1)
{
    fd_ = other.fd_;
    other.fd_ = -1;
}

socketRAII& socketRAII::operator=(socketRAII&& other) noexcept
{
    if (this != &other)
    {
        if (is_alive())
        {
            ::close(fd_);
        }
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

int socketRAII::get_fd() const
{
    return fd_;
}

bool socketRAII::is_alive() const
{
    return fd_ != -1;
}

void socketRAII::reset(int new_fd)
{
    if (is_alive())
    {
        ::close(fd_);
    }
    fd_ = new_fd;
}