#include "HttpServer/HttpServer.h"

int main()
{
    try
    {
        HttpServer server1(8080,16,30);
        server1.run();
    }
    catch (...)
    {
        exit(1);
    }

    return 0;
}

