#include <http/http_server.h>
#include "log.h"

using namespace TinyServer;
static Ref<Logger> logger = TINY_LOG_ROOT;

void run()
{
    logger->setLevel(LogLevel::INFO);
    Ref<Address> addr = Address::LookupIPAddress("0.0.0.0:8030");
    if (!addr)
    {
        TINY_LOG_ERROR(logger) << "create addr fail";
        return;
    }
    Ref<http::HttpServer> server(new http::HttpServer);
    while (!server->bind(addr))
    {
        TINY_LOG_ERROR(logger) << "http server bind addr = " << *addr << "fail";
        sleep(1);
    }
    server->start();
}


int main()
{
    IOManager iom;
    iom.schedule(run);
    return 0;
}

