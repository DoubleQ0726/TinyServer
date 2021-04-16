#include "TinyServer.h"
#include "socket.h"
#include "iomanager.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

void test_socket()
{
    // std::vector<Ref<Address>> addrs;
    // Ref<IPAddress> addr;
    // Address::Lookup(addrs, "www.baidu.com", AF_INET);
    // for (auto& item : addrs)
    // {
    //     TINY_LOG_INFO(logger) << item->toString();
    //     addr = std::dynamic_pointer_cast<IPAddress>(item);
    //     if (addr)
    //         break;
    // }
    Ref<IPAddress> addr = Address::LookupIPAddress("www.baidu.com");
    if (addr)
    {
        TINY_LOG_INFO(logger) << "get address: " << addr->toString();
    }
    else
    {
        TINY_LOG_ERROR(logger) << "get address fail";
        return;
    }
    Ref<Socket> sock = Socket::CreateTCP(addr);
    addr->setPort(80);
    if (!sock->connect(addr))
    {
        TINY_LOG_ERROR(logger) << "connect " << addr->toString() << " fail";
        return;
    }
    else
    {
        TINY_LOG_INFO(logger) << "connect " << addr->toString() << " connected";
    }
    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int res = sock->send(buff, sizeof(buff));
    if (res <= 0)
    {
        TINY_LOG_ERROR(logger) << "send fail res = " << res;
        return;
    }
    std::string buffers;
    buffers.resize(4096);
    res = sock->recv(&buffers[0], buffers.size());
    if (res <= 0)
    {
        TINY_LOG_ERROR(logger) << "recv fail res = " << res;
        return;
    }
    buffers.resize(res);
    TINY_LOG_INFO(logger) << buffers;
}

int main()
{
    IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}