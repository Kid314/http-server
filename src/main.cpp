#include <iostream>
#include "HttpServer/HttpServer.h"
#include <csignal>
#include "spdlog/spdlog.h"
static HttpServer* server_ptr=nullptr;
void signal_handler(int signal)
{
    if (signal==SIGINT||signal==SIGTERM)
    {
        if (server_ptr!=nullptr)
        {
            server_ptr->is_running=false;
        }
    }
}
int main()
{
    signal(SIGINT,signal_handler);
    signal(SIGTERM,signal_handler);
    try
    {
        spdlog::info("hello");
        HttpServer server1(8080,16,30);
        server_ptr=&server1;
        server1.run();
        std::cout<<"server stop";
    }
    catch (const std::exception& error)
    {
        std::cerr<<"error:"<<error.what()<<"\n";
        return 1;
    }
    server_ptr=nullptr;
    return 0;
}

