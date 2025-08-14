//
// Created by kid314 on 25-8-12.
//

#include "Router.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include "spdlog/spdlog.h"

HttpResponse Router::route(const std::shared_ptr<HttpRequest>& request)
{
    if (request->get_method()==Method::UNKNOWN||!request->get_path().has_value())
    {
        return sendNoFound();
    }
    std::string_view request_view=request->get_path().value();
    if (request_view=="/")
    {
        return sendHello();
    }
    else if (request_view.starts_with("/public/"))
    {
        if (request_view.find("..")!=std::string_view::npos)
        {
            return sendNoFound();
        }
        return sendFile(request_view,request->get_http_version());
    }
    return sendNoFound();
}

HttpResponse Router::sendHello()
{
    HttpResponse response;
    response.http_version="HTTP/1.1";
    response.status=200;
    response.Content_type="Content-Type: text/plain\r\n";
    response.Content_length=12;
    response.body="Hello World\n";
    response.Content_Disposition=std::nullopt;
    return response;
}
HttpResponse Router::sendNoFound()
{
    HttpResponse response;
    response.http_version="HTTP/1.1";
    response.status=404;
    response.Content_type="Content-Type: text/plain\r\n";
    response.Content_length=10;
    response.body="Not Found\n";
    response.Content_Disposition=std::nullopt;
    return response;
}
HttpResponse Router::findFileError()
{
    HttpResponse response;
    response.http_version="HTTP/1.1";
    response.status=404;
    response.Content_type="Content-Type: text/plain\r\n";
    response.Content_length=16;
    response.body="Find file error\n";
    response.Content_Disposition=std::nullopt;
    return response;
}
HttpResponse Router::sendFile(std::string_view path,std::string_view http_ver)
{
    std::string file_path(path.substr(1));
    std::ifstream file(file_path,std::ios::binary|std::ios::ate);
    if (!file.is_open())
    {
        spdlog::error("can't open files，current working directory:{}，{}",std::filesystem::current_path().c_str(),file_path);
        return findFileError();
    }
    std::streamsize file_size=file.tellg();
    file.seekg(0,std::ios::beg);
    std::string_view filename=path;
    size_t last_slash=path.find_last_of('/');
    if (last_slash!=std::string::npos)
    {
        filename=std::string_view(path).substr(last_slash+1);
    }
    HttpResponse response;
    std::string http_version(http_ver);
    response.status=200;
    response.http_version=std::move(http_version);
    response.Content_Disposition="Content-Disposition: attachment; filename=\"";
    response.Content_Disposition->append(filename);
    response.Content_Disposition->append("\"\r\n");
    response.Content_length=file_size;
    response.Content_type="Content-Type: application/octet-stream\r\n";
    response.body.resize(file_size);
    if (!file.read(&response.body[0],file_size))
    {
        spdlog::error("can't read file");
        return findFileError();
    }
    return response;
}


