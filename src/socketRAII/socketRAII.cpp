//
// Created by kid314 on 25-7-7.
//

#include "socketRAII.h"
#include <utility> // For std::swap

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
    // Steal the resource from the other object
    fd_ = other.fd_;
    // Leave the other object in a safe state (no longer owning the fd)
    other.fd_ = -1;
}

socketRAII& socketRAII::operator=(socketRAII&& other) noexcept
{
    // Self-assignment check
    if (this != &other)
    {
        // Release our own resource first
        if (is_alive())
        {
            ::close(fd_);
        }
        // Steal the resource from the other object
        fd_ = other.fd_;
        // Leave the other object in a safe state
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