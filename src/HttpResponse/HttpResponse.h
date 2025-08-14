//
// Created by kid314 on 25-8-13.
//

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <optional>
#include <string>

class HttpResponse
{
public:
    HttpResponse();
    [[nodiscard]] std::string generate_response() const;
    int status;
    std::streamsize Content_length;
    std::optional<std::string> Content_Disposition;
    std::string Content_type;
    std::string http_version;
    std::string body;
};



#endif //HTTPRESPONSE_H
