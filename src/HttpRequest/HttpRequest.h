//
// Created by kid314 on 25-8-13.
//

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <optional>
#include <string>
#include <string_view>
#include "http_method.h"
class HttpRequest
{
public:
    explicit HttpRequest(std::string_view);
    [[nodiscard]] inline Method get_method() const
    {
        return method;
    }
    [[nodiscard]] inline std::optional<std::string_view> get_path() const
    {
        if (path.has_value())
        {
            return path.value();
        }
        return std::nullopt;
    }
    [[nodiscard]] std::string_view get_http_version() const
    {
        return http_version;
    }
private:
    void prase(std::string_view);
    Method method;
    std::optional<std::string> path;
    std::string http_version;
    //std::string body;
};



#endif //HTTPREQUEST_H
