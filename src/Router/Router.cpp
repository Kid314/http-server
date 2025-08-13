//
// Created by kid314 on 25-8-12.
//

#include "Router.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include "spdlog/spdlog.h"

std::string Router::route(std::shared_ptr<HttpRequest> request)
{

}

std::string Router::sendHello()
{
    return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello World\n";
}
std::string Router::sendNoFound()
{
    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 10\r\n\r\nNot Found\n";
}
std::string Router::findFilleError()
{
    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nFind file error\n";
}
std::string Router::sendFile(const std::string & path)
{
    std::ifstream file(path,std::ios::binary|std::ios::ate);
    if (!file.is_open())
    {
        spdlog::error("can't open files");
        return findFilleError();
    }
    std::streamsize file_size=file.tellg();
    file.seekg(0,std::ios::beg);
    std::string_view filename=path;
    size_t last_slash=path.find_last_of('/');
    if (last_slash!=std::string::npos)
    {
        filename=std::string_view(path).substr(last_slash+1);
    }
    std::ostringstream header_stream;
    header_stream<<"HTTP/1.1 200 OK\r\n";
    header_stream<<"Content-Type: application/octet-stream\r\n";
    header_stream<<"Content-Length: "<<file_size<<"\r\n";
    header_stream<<"Content-Disposition: attachment; filename=\""<<filename<<"\"\r\n";
    header_stream<<"\r\n";
    std::string response_header=header_stream.str();
    response_header.resize(response_header.length()+file_size);
    if (!file.read(&response_header[header_stream.str().length()],file_size))
    {
        spdlog::error("can't read file");
        return findFilleError();
    }
    return response_header;
}


