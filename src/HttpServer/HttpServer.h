//
// Created by kid314 on 25-7-11.
//

#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "../Epoller/Epoll.h"
#include "../SocketRAII/SocketRAII.h"
#include "../threadpool/threadpool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
class HttpServer
{
private:
    int port;
    SocketRAII listen_socket;
    Epoll epoller;
    threadpool tpool;
    std::unordered_map<int,SocketRAII> client_sockets;
    std::mutex cs_lock;
    [[nodiscard]] inline SocketRAII init_lisSock();
    void new_connect();
    void client_event(int);
    void close_connect(int);
public:
    explicit HttpServer(int,int,int);
    ~HttpServer();
    void run();
};



#endif //HTTPSERVER_H
