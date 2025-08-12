//
// Created by kid314 on 25-7-11.
//

#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "../Epoller/Epoll.h"
#include "../SocketRAII/SocketRAII.h"
#include "../threadpool/threadpool.h"
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
class HttpConnection
{
public:
    explicit HttpConnection(int);
    [[nodiscard]] inline int get_fd() const
    {
        return socket.get_fd();
    }
    //const std::string response="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello World";
    std::string read_buffer;
    std::string write_buffer;
    size_t write_offset;
private:
    SocketRAII socket;
};
class HttpServer
{
public:
    explicit HttpServer(int,int,int);
    ~HttpServer();
    void run();
    std::atomic<bool> is_running;
private:
    void handleAccept();
    void handleRead(int);
    void handleWrite(int);
    void closeConnection(int);
    [[nodiscard]] SocketRAII init_listen_fd() const;
    int port;
    SocketRAII listen_fd;
    Epoll epoller;
    threadpool tpool;
    std::mutex conn_lock;
    std::unordered_map<int,std::shared_ptr<HttpConnection>> connections;
};



#endif //HTTPSERVER_H
