//
// Created by kid314 on 25-8-13.
//

#include "HttpRequest.h"

HttpRequest::HttpRequest(std::string_view new_request):method(Method::GET)
{
    prase(new_request);
}

void HttpRequest::prase(std::string_view new_request)
{
    method=Method::UNKNOWN;
    if (new_request.empty())
    {
        return ;
    }
    size_t line_pos=new_request.find("\r\n");
    if (line_pos==std::string_view::npos)
    {
        return ;
    }
    std::string_view request_line=new_request.substr(0,line_pos);
    size_t method_pos=request_line.find(' ');
    if (method_pos==std::string_view::npos)
    {
        return ;
    }
    method=to_method(request_line.substr(0,method_pos));
    request_line.remove_prefix(method_pos+1);
    size_t uri_pos=request_line.find(' ');
    if (uri_pos==std::string_view::npos)
    {
        path=std::nullopt;
    }
    path=std::string(request_line.substr(0,uri_pos));
    request_line.remove_prefix(uri_pos+1);
    http_version=request_line;
}
