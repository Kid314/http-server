//
// Created by kid314 on 25-8-12.
//

#include "Router.h"
#include <cstring>
#include <fstream>
#include <sstream>

#include "spdlog/spdlog.h"

std::string Router::route(std::string_view request_view)
{
    if (request_view.empty())
    {
        return sendNoFound();
    }
    size_t first_line=request_view.find("\r\n");
    if (first_line==std::string_view::npos)
    {
        return sendNoFound();
    }
    std::string_view request_line=request_view.substr(0,first_line);
    size_t method_pos=request_line.find(' ');
    if (method_pos==std::string_view::npos)
    {
        return sendNoFound();
    }
    std::string_view method=request_line.substr(0,method_pos);
    request_line.remove_prefix(method_pos+1);
    size_t uri_pos=request_line.find(' ');
    if (uri_pos==std::string_view::npos)
    {
        return sendNoFound();
    }
    std::string_view uri=request_line.substr(0,uri_pos);

    if (method!="GET")
    {
        spdlog::error("is not GET method");
        return sendNoFound();
    }

    if (uri=="/")
    {
        return sendHello();
    }
    else if (uri.starts_with("/public/"))
    {
        std::string_view filename_view(uri);
        if (filename_view.empty()||filename_view.find("..")!=std::string_view::npos)
        {
            spdlog::error("has .. in uri");
            return findFilleError();
        }
        std::string file_path(filename_view.substr(1));
        return sendFile(file_path);
    }
    else
    {
        spdlog::error("uri error");
        return findFilleError();
    }
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


