#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include <string_view>
#include "httpconnection/httpconnection.h" // For HttpRequest

class HttpParser
{
public:
    enum ParseResult { Good, Bad, Incomplete };

    static ParseResult parse_request(std::string_view data, HttpRequest &req);
};

#endif //HTTPPARSER_H
