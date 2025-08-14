//
// Created by kid314 on 25-7-11.
//
#include "HttpServer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include "../Router/Router.h"

void setNonBlocking(int fd)
{
    int flags=fcntl(fd,F_GETFL,0);
    if (flags==-1)
    {
        close(fd);
        throw std::runtime_error("F_GETFL error : "+std::to_string(fd));
    }
    if (fcntl(fd,F_SETFL,flags|O_NONBLOCK)==-1)
    {
        close(fd);
        throw std::runtime_error("F_SETFL error : "+std::to_string(fd));
    }
}

HttpConnection::HttpConnection(int fd):socket(fd),write_offset(0)
{

}
HttpServer::HttpServer(int set_port, int max_events, int max_threads):port(set_port),epoller(max_events),tpool(max_threads),listen_fd(init_listen_fd()),is_running(true)
{

}
SocketRAII HttpServer::init_listen_fd() const
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
HttpServer::~HttpServer()
{
    is_running=false;
    tpool.shutdown();
    {
        std::lock_guard<std::mutex> lock(conn_lock);
        connections.clear();
    }
}
void HttpServer::run()
{
    epoller.add_fd(listen_fd.get_fd(),EPOLLIN|EPOLLET);
    while (is_running)
    {
        int event_count=epoller.wait(-1);
        for (int i=0;i<event_count;++i)
        {
            int event_fd=epoller.get_fd(i);
            uint32_t events=epoller.get_events(i);
            if (event_fd==listen_fd.get_fd())
            {
                handleAccept();
            }
            else if (events&(EPOLLRDHUP|EPOLLHUP|EPOLLERR))
            {
                closeConnection(event_fd);
            }
            else if (events&EPOLLIN)
            {
                tpool.enqueue(&HttpServer::handleRead,this,event_fd);
            }
            else if (events&EPOLLOUT)
            {
                tpool.enqueue(&HttpServer::handleWrite,this,event_fd);
            }
        }
    }
}
void HttpServer::handleWrite(int fd)
{
    std::shared_ptr<HttpConnection> conn;
    {
        std::lock_guard<std::mutex> lock(conn_lock);
        auto it=connections.find(fd);
        if (it==connections.end())
        {
            return;
        }
        conn=it->second;
    }
    std::lock_guard<std::mutex> lock(conn->connection_lock);
    while (conn->write_offset<conn->write_buffer.length())
    {
        const char* send_data=conn->write_buffer.c_str()+conn->write_offset;
        size_t send_bytes=conn->write_buffer.length()-conn->write_offset;
        ssize_t sent_bytes=write(fd,send_data,send_bytes);
        if (sent_bytes>0)
        {
            conn->write_offset+=sent_bytes;
        }
        else
        {
            if (sent_bytes<0&&(errno==EAGAIN||errno==EWOULDBLOCK))
            {
                return;
            }
            perror("write error");
            closeConnection(fd);
            return;
        }
    }
    if (conn->write_offset>=conn->write_buffer.length())
    {
        conn->write_buffer.clear();
        conn->write_offset=0;
        epoller.mod_fd(fd,EPOLLIN|EPOLLET);
        //closeConnection(fd);
    }
}
void HttpServer::handleRead(int fd)
{
    std::shared_ptr<HttpConnection> conn;
    {
        std::lock_guard<std::mutex> lock(conn_lock);
        auto it=connections.find(fd);
        if (it==connections.end())
        {
            return;
        }
        conn=it->second;
    }
    char buffer[4096];
    while (true)
    {
        ssize_t read_bytes=read(fd,buffer,sizeof(buffer));
        if (read_bytes>0)
        {
            conn->read_buffer.append(buffer,read_bytes);
        }
        else if (read_bytes==0)
        {
            closeConnection(fd);
            return;
        }
        else
        {
            if (errno==EAGAIN||errno==EWOULDBLOCK)
            {
                break;
            }
            perror("read error");
            closeConnection(fd);
            return;
        }
    }
    while (true)
    {
        size_t handle_pos=conn->read_buffer.find("\r\n\r\n");
        if (handle_pos==std::string::npos)
        {
            break;
        }
        std::string_view each_request(conn->read_buffer.data(),handle_pos+4);
        auto temp=std::make_shared<HttpRequest>(each_request);
        std::string http_response=Router::route(temp).generate_response();
        {
            std::lock_guard<std::mutex> lock(conn->connection_lock);
            conn->write_buffer+=http_response;
        }
        conn->read_buffer.erase(0,handle_pos+4);
    }
    if (!conn->write_buffer.empty())
    {
        epoller.mod_fd(fd,EPOLLIN|EPOLLET|EPOLLOUT);
    }
}
void HttpServer::handleAccept()
{
    struct sockaddr_in client_addr={};
    socklen_t len =sizeof(client_addr);
    while (true)
    {
        int client_fd=accept(listen_fd.get_fd(),reinterpret_cast<struct sockaddr*>(&client_addr),&len);
        if (client_fd<0)
        {
            if (errno==EAGAIN||errno==EWOULDBLOCK)
            {
                break;
            }
            throw std::runtime_error("accept error");
        }
        setNonBlocking(client_fd);
        epoller.add_fd(client_fd,EPOLLIN|EPOLLET);
        {
            std::lock_guard<std::mutex> lock(conn_lock);
            connections.emplace(client_fd,std::make_shared<HttpConnection>(client_fd));
        }
    }
}
void HttpServer::closeConnection(int fd)
{
    std::lock_guard<std::mutex> lock(conn_lock);
    auto it=connections.find(fd);
    if (it!=connections.end())
    {
        epoller.del_fd(fd);
        connections.erase(it);
    }
}





