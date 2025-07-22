#include "HttpServer/HttpServer.h"

int main()
{
    HttpServer server1(8080,16,30);
    server1.run();
    return 0;
}

