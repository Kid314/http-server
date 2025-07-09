#include "router.h"
#include "httpconnection/httpconnection.h" // For HttpResponse

void Router::add_route(std::string_view path, HttpHandler handler) {
    routes_[path] = std::move(handler);
}

void Router::route(const HttpRequest& req, HttpResponse& res) {
    auto it = routes_.find(req.path);
    if (it != routes_.end()) {
        it->second(req, res);
    } else {
        // Simple 404 Not Found response
        res.set_status_code(404, "Not Found");
        res.set_body("<html><body><h1>404 Not Found</h1></body></html>");
        res.add_header("Content-Type", "text/html");
    }
}