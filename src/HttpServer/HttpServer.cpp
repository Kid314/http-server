//
// Created by kid314 on 25-7-11.
//

#include "HttpServer.h"

#include <cstring>

SocketRAII HttpServer::init_lisSock()
{
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if (fd<0)
    {
        throw std::runtime_error("create socket error");
    }
    SocketRAII temp(fd);
    int opt=1;
    if (setsockopt(temp.get_fd(),SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
    {
        throw std::runtime_error("setsockopt error");
    }
    struct sockaddr_in server_addr={};
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(port);

    if (bind(temp.get_fd(),reinterpret_cast<struct sockaddr*>(&server_addr),sizeof(server_addr))<0)
    {
        throw std::runtime_error("bind error");
    }
    if (listen(temp.get_fd(),4096)<0)
    {
        throw std::runtime_error("listen error");
    }
    return temp;
}
void HttpServer::client_event(int)
{

}
void HttpServer::close_connect(int fd)
{
    epoller.del_fd(fd);
    {
        std::lock_guard<std::mutex> lock(cs_lock);
        client_sockets.erase(fd);
    }
}
HttpServer::HttpServer(int new_port, int max_threadNum,int max_events):port(new_port),tpool(max_threadNum),epoller(max_events),listen_socket(init_lisSock())
{

}
void HttpServer::new_connect()
{
    struct sockaddr_in client_addr={};
    socklen_t client_addr_len=sizeof(client_addr);
    while (true)
    {
        int client_fd=accept(listen_socket.get_fd(),reinterpret_cast<struct sockaddr *>(&client_addr),&client_addr_len);
        if (client_fd<0)
        {
            break;
        }
        printf("new connection:fd=%d\n",client_fd);
        epoller.add_fd(client_fd,EPOLLIN|EPOLLET);
        {
            std::lock_guard<std::mutex> lock(cs_lock);
            client_sockets.emplace(client_fd,SocketRAII(client_fd));
        }
    }

}
void HttpServer::run()
{
    while (true)
    {
        int event_count=epoller.wait(-1);
        for (int i=0;i<event_count;++i)
        {
            int event_fd=epoller.get_fd(i);
            uint32_t events=epoller.get_events(i);
            if (event_fd==listen_socket.get_fd())
            {
                new_connect();
            }
            else if (events&(EPOLLRDHUP|EPOLLHUP|EPOLLERR))
            {
                close_connect(event_fd);
            }
            else if (events&EPOLLIN)
            {
                tpool.enqueue(&HttpServer::client_event,this,event_fd);
            }
        }
    }
}
HttpServer::~HttpServer()
{

}





