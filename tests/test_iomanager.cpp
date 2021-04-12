#include "TinyServer.h"
#include "iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>

using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;
void test_fiber()
{
    TINY_LOG_INFO(logger) << "test_fiber";
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    ::fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "112.80.248.75", &addr.sin_addr.s_addr);
    ::connect(sock, (const sockaddr*)&addr, sizeof(addr));
    TINY_LOG_INFO(logger) << "connect!";
    IOManager::GetThis()->addEvent(sock, IOManager::READ, [](){
        TINY_LOG_INFO(logger) <<"read callback";
    });
    IOManager::GetThis()->addEvent(sock, IOManager::WRITE, [=](){
        TINY_LOG_INFO(logger) <<"write callback";
        IOManager::GetThis()->cancelEvent(sock, IOManager::READ);
        close(sock);
    });
}

void test1()
{
    IOManager iom(2, false);
    iom.schedule(&test_fiber);
}

int main()
{
    test1();
    return 0;
}