//
// Created by kid314 on 25-7-11.
//

#include "SocketRAII.h"

SocketRAII::SocketRAII(int fd_=-1):fd(fd_){}
SocketRAII::~SocketRAII()
{
    if (fd!=-1)
    {
        ::close(fd);
    }
}
int SocketRAII::get_fd() const
{
    return fd;
}
int SocketRAII::release()
{
    int temp_fd=fd;
    fd=-1;
    return temp_fd;
}
void SocketRAII::reset(int new_fd=-1)
{
    if (fd!=-1)
    {
        ::close(fd);
    }
    fd=new_fd;
}
SocketRAII::SocketRAII(SocketRAII&& new_socket) noexcept : fd(new_socket.fd)
{
    new_socket.fd=-1;
}
SocketRAII &SocketRAII::operator=(SocketRAII&& new_socket) noexcept
{
    if (this!=&new_socket)
    {
        if (fd!=-1)
        {
            ::close(fd);
        }
        fd=new_socket.fd;
        new_socket.fd=-1;
    }
    return *this;
}

