//
// Created by kid314 on 25-8-13.
//

#ifndef HTTP_METHOD_H
#define HTTP_METHOD_H
#include <array>
#include <string_view>
#include <utility>
enum class Method
{
    GET,POST,PUT,DELETE,HEAD,OPTIONS,TRACE,CONNECT,PATCH,UNKNOWN
};
constexpr std::array<std::pair<std::string_view,Method>,9> METHOD_MAP=
    {
        {
            {"GET",Method::GET},
                {"POST",Method::POST},
            {"PUT",Method::POST},
            {"DELETE",Method::DELETE},
            {"HEAD",Method::HEAD},
            {"OPTIONS",Method::OPTIONS},
            {"TRACE",Method::TRACE},
            {"CONNECT",Method::CONNECT},
            {"PATCH",Method::PATCH}
        }
    };
constexpr Method to_method(const std::string_view method_str)
{
    for (const auto& [str,method_enum]:METHOD_MAP)
    {
        if (method_str==str)
        {
            return method_enum;
        }
    }
    return Method::UNKNOWN;
}
#endif //HTTP_METHOD_H
