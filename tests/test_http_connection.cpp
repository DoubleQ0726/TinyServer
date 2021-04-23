#include "http/http_connection.h"
#include "log.h"
#include "iomanager.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

void test_pool()
{
    Ref<http::HttpConnectionPool> pool(new http::HttpConnectionPool("www.sylar.top", "", 80, 10, 1000 * 3, 5));
    IOManager::GetThis()->addTimer(1000, [pool](){
        auto r = pool->doGet("/", 300);
        TINY_LOG_INFO(logger) << r->toString();
    }, true);
}

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
    TINY_LOG_INFO(logger) << "=========================";

    auto r = http::HttpConnection::DoGet("http://www.sylar.top/blog/", 300);
    TINY_LOG_INFO(logger) << "result = " << r->result
                          << " error = " << r->error
                          << " rsp = " << (r->response ? r->response->toString() : "");
    TINY_LOG_INFO(logger) << "=========================";
    test_pool();
}


int main()
{
    IOManager iom(2);
    iom.schedule(&run);
    return 0;
}