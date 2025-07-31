//
// Created by kid314 on 25-7-11.
//

#include "HttpServer.h"
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <string>
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
    fcntl(temp.get_fd(),F_SETFL,fcntl(temp.get_fd(),F_GETFL,0)|O_NONBLOCK);
    printf("kid start\n");
    return temp;
}
void HttpServer::client_event(int fd)
{
    // std::vector<char> buffer;
    // char temp_buffer[4096];
    // ssize_t byte_read;
    //
    // while (true)
    // {
    //     byte_read=read(fd,temp_buffer,sizeof(temp_buffer));
    //     if (byte_read>0)
    //     {
    //         buffer.insert(buffer.end(),temp_buffer,temp_buffer+byte_read);
    //     }
    //     else if (byte_read==0)
    //     {
    //         close_connect(fd);
    //         return ;
    //     }
    //     else
    //     {
    //         if (errno==EAGAIN||errno==EWOULDBLOCK)
    //         {
    //             break;
    //         }
    //         close_connect(fd);
    //         throw std::runtime_error("read failed");
    //     }
    // }
    std::string html_content="<html><body><h1>hello</h1></body></html>";
    std::string http_response=
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: " + std::to_string(html_content.length()) + "\r\n"
        "Connection: close\r\n" // 我们将主动关闭连接
        "\r\n"
        + html_content;
    int flags=fcntl(fd,F_GETFL,0);
    if (flags==-1)
    {
        close_connect(fd);
        throw std::runtime_error("fcntl F_GETFL failed");
    }
    if (fcntl(fd,F_SETFL,flags&~O_NONBLOCK)==-1)
    {
        close_connect(fd);
        throw std::runtime_error("fcntl F_SETFL to BLOCK failed");
    }
    ssize_t total_sent=0;
    ssize_t byte_sent;
    while (total_sent<http_response.length())
    {
        byte_sent=write(fd,http_response.c_str()+total_sent,http_response.length()-total_sent);
        if (byte_sent<0)
        {
            close_connect(fd);
            throw std::runtime_error("write failed");
        }
        total_sent+=byte_sent;
    }
    close_connect(fd);
}
void HttpServer::close_connect(int fd)
{
    epoller.del_fd(fd);
    {
        std::lock_guard<std::mutex> lock(cs_lock);
        client_sockets.erase(fd);
    }
}
HttpServer::HttpServer(int new_port, int max_threadNum,int max_events):port(new_port),tpool(max_threadNum),epoller(max_events),listen_socket(init_lisSock()),is_running(true)
{
    epoller.add_fd(listen_socket.get_fd(),EPOLLIN|EPOLLET);
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
        int flags=fcntl(client_fd,F_GETFL,0);
        if (flags==-1)
        {
            close_connect(client_fd);
            throw std::runtime_error("fcntl F_GETFL failed");
        }
        if (fcntl(client_fd,F_SETFL,flags|O_NONBLOCK)==-1)
        {
            close_connect(client_fd);
            throw std::runtime_error("fcntl F_SETFL O_NONBLOCK failed");
        }
        //printf("new connection:fd=%d\n",client_fd);
        epoller.add_fd(client_fd,EPOLLIN|EPOLLET);
        {
            std::lock_guard<std::mutex> lock(cs_lock);
            client_sockets.emplace(client_fd,SocketRAII(client_fd));
        }
    }

}
void HttpServer::run()
{
    while (is_running)
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
                //client_event(event_fd);
            }
        }
    }
}
HttpServer::~HttpServer()
{
    tpool.shutdown();
    std::lock_guard<std::mutex> lock(cs_lock);
    client_sockets.clear();
}





