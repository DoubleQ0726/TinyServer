#include "tcp_server.h"
#include "iomanager.h"
#include "log.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

void run()
{
    auto addr = Address::LookupAny("0.0.0.0:8033");
    //auto addr2 = Ref<UnixAddress>(new UnixAddress("/tmp/unix_addr"));
    std::vector<Ref<Address>> addrs;
    std::vector<Ref<Address>> fails;
    addrs.push_back(addr);
    //addrs.push_back(addr2);

    Ref<TCPServer> tcp_server(new TCPServer);

    while (!tcp_server->bind(addrs, fails))    //绑定socket并监听
    {
        TINY_LOG_ERROR(logger) << "error";
        sleep(2);
    }
    tcp_server->start();
}

int main()
{
    IOManager iom(2);
    iom.schedule(&run);
    return 0;
}