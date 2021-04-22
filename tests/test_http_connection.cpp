#include "http/http_connection.h"
#include "log.h"
#include "iomanager.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

void run()
{
    Ref<Address> addr = Address::LookupIPAddress("www.sylar.top:80");
    if (!addr)
    {
        TINY_LOG_ERROR(logger) << "get addr error";
        return;
    }

    Ref<Socket> sock = Socket::CreateTCP(addr);
    bool res = sock->connect(addr);
    if (!res)
    {
        TINY_LOG_ERROR(logger) << "connect " << *addr << "failed";
        return;
    }
    Ref<http::HttpConnection> conn(new http::HttpConnection(sock));
    Ref<http::HttpRequest> req(new http::HttpRequest);
    req->setPath("/blog/");
    req->setHeader("host", "www.sylar.top");
    conn->sendRequest(req);
    auto rsp = conn->recvResponse();
    TINY_LOG_INFO(logger) << "req:" << std::endl << *req;
    if (!rsp)
    {
        TINY_LOG_INFO(logger) << "recv response error";
        return;
    }
    TINY_LOG_INFO(logger) << "rsp:" << std::endl <<*rsp;
}


int main()
{
    IOManager iom(2);
    iom.schedule(&run);
    return 0;
}