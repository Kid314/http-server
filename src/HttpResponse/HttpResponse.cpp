//
// Created by kid314 on 25-8-13.
//

#include "HttpResponse.h"
#include <fstream>
#include <sstream>
HttpResponse::HttpResponse():status(200),Content_length(0)
{

}
std::string HttpResponse::generate_response() const
{
    std::ostringstream header_stream;
    header_stream<<http_version<<" "<<status;
    if (status==200)
    {
        header_stream<<" OK";
    }
    else
    {
        header_stream<<" Not Found";
    }
    header_stream<<"\r\n";
    header_stream<<Content_type;
    header_stream<<"Content-Length: "<<Content_length<<"\r\n";
    if (Content_Disposition.has_value())
    {
        header_stream<<Content_Disposition.value();
        header_stream<<"\r\n";
    }
    header_stream<<body;
    return header_stream.str();
}

