#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <functional>
#include <map>
#include <string_view>

class HttpResponse; // Forward declaration
class HttpRequest;  // Forward declaration

using HttpHandler = std::function<void(const HttpRequest&, HttpResponse&)>;

class Router
{
public:
    void add_route(std::string_view path, HttpHandler handler);
    void route(const HttpRequest& req, HttpResponse& res);

private:
    std::map<std::string_view, HttpHandler> routes_;
};

#endif //ROUTER_H