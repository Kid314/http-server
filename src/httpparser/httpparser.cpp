#include "httpparser.h"
#include <iostream>

// Basic and not fully robust HTTP parser. 
// It only handles simple GET requests for this example.
HttpParser::ParseResult HttpParser::parse_request(std::string_view data, HttpRequest& req) {
    // Find the end of the request line
    size_t req_line_end = data.find("\r\n");
    if (req_line_end == std::string_view::npos) return Incomplete;

    // Parse request line
    std::string_view request_line = data.substr(0, req_line_end);
    size_t method_end = request_line.find(' ');
    if (method_end == std::string_view::npos) return Bad;
    req.method = request_line.substr(0, method_end);

    size_t path_end = request_line.find(' ', method_end + 1);
    if (path_end == std::string_view::npos) return Bad;
    req.path = request_line.substr(method_end + 1, path_end - (method_end + 1));

    req.version = request_line.substr(path_end + 1);

    // For simplicity, we'll skip header and body parsing in this example.
    // A real implementation would parse headers and body here.

    return Good;
}