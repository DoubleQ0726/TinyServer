#include "tcp_server.h"
#include "iomanager.h"
#include "log.h"
#include "bytearray.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

class EchoServer : public TCPServer
{
public:
    EchoServer(int type);
    void handleClient(Ref<Socket> client);

private:
    int m_type = 0;
};

EchoServer::EchoServer(int type)
    : m_type(type)
{

}

void EchoServer::handleClient(Ref<Socket> client)
{
    TINY_LOG_INFO(logger) << "handleClient" << *client;
    Ref<ByteArray> ba(new ByteArray);
    while (true)
    {
        ba->clear();
        std::vector<iovec> iovec;
        ba->getWriteBuffers(iovec, 1024);
        int res = client->recv(&iovec[0], iovec.size());
        if (res == 0)
        {
            TINY_LOG_INFO(logger) << "client close: " << *client;
            break;
        }
        else if (res < 0)
        {
            TINY_LOG_ERROR(logger) << "client error res = " << res << " errno = " << errno
                << " errstr = " << strerror(errno);
            break; 
        }
        ba->setPosition(ba->getPosition() + res);
        ba->setPosition(0);
        if (m_type == 1)
        {
            //text
            std::cout << ba->toString();
            //TINY_LOG_INFO(logger) << ba->toString();
        }
        else    
        {
            //binary
            std::cout << ba->toHexString();
            //TINY_LOG_INFO(logger) << ba->toHexString();
        }
        client->send(&iovec[0], iovec.size());
        ba->setPosition(ba->getPosition() + res);
    }
}

int type = 1;
void run()
{
   Ref<EchoServer> echo(new EchoServer(type));
   Ref<Address> addr = Address::LookupAny("0.0.0.0:8020");
   while (!echo->bind(addr))
   {
       sleep(2);
   }
   echo->start();
}


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        TINY_LOG_INFO(logger) << "used as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
        return 0;
    }
    if (strcmp(argv[1], "-b") == 0)
    {
        type = 2;
    }
    IOManager iom(2);
    iom.schedule(&run);
    return 0;
}