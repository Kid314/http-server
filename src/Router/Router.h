//
// Created by kid314 on 25-8-12.
//

#ifndef ROUTER_H
#define ROUTER_H
#include <memory>
#include "../HttpRequest/HttpRequest.h"
class Router
{
public:
    static std::string route(std::shared_ptr<HttpRequest>);
private:
    static std::string sendHello();
    static std::string sendNoFound();
    static std::string findFilleError();
    static std::string sendFile(const std::string&);
};

#endif //ROUTER_H
