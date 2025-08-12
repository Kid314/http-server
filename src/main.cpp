#include <iostream>
#include "HttpServer/HttpServer.h"
#include <csignal>
#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
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
void set_logger()
{
    auto max_size=1048576*5;
    auto max_files=3;
    auto rotating_sink=std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/server.log",max_size,max_files);
    auto logger=std::make_shared<spdlog::logger>("http_server_logger",rotating_sink);
    logger->set_level(spdlog::level::info);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}
int main()
{
    set_logger();
    signal(SIGINT,signal_handler);
    signal(SIGTERM,signal_handler);
    try
    {
        spdlog::info("server start");
        auto server1=std::make_unique<HttpServer>(8080,16,30);
        server_ptr=server1.get();
        server1->run();
        server_ptr=nullptr;
        spdlog::info("server stop");
    }
    catch (const std::exception& error)
    {
        spdlog::error("find error:{}",error.what());
        std::cerr<<"error:"<<error.what()<<"\n";
        return 1;
    }
    return 0;
}

