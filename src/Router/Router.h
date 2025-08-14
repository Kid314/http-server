//
// Created by kid314 on 25-8-12.
//

#ifndef ROUTER_H
#define ROUTER_H
#include <memory>
#include "../HttpRequest/HttpRequest.h"
#include "../HttpResponse/HttpResponse.h"
class Router
{
public:
    static HttpResponse route(const std::shared_ptr<HttpRequest>&);
private:
    static HttpResponse sendHello();
    static HttpResponse sendNoFound();
    static HttpResponse findFileError();
    static HttpResponse sendFile(std::string_view,std::string_view);
};
static const std::string HELLO_WORLD_RESPONSE = []{
    HttpResponse response;
    response.http_version = "HTTP/1.1";
    response.status = 200;
    response.Content_type = "Content-Type: text/plain\r\n";
    response.Content_length = 12;
    response.body = "Hello World\n";
    return response.generate_response();
}();
#endif //ROUTER_H
