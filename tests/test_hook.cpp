#include "hook.h"
#include "iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

void test_sleep()
{
    IOManager iom(1);

    iom.schedule([](){
        sleep(2);
        TINY_LOG_INFO(logger) << "sleep 2";
    });

    iom.schedule([](){
        sleep(3);
        TINY_LOG_INFO(logger) << "sleep 3";
    });
    TINY_LOG_INFO(logger) << "test_sleep";
}

void test_socket()
{
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    //::fcntl(sock, F_SETFL, O_NONBLOCK);   ???

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "112.80.248.75", &addr.sin_addr.s_addr);
    TINY_LOG_INFO(logger) << "begin connect";
    int res = ::connect(sock, (const sockaddr*)&addr, sizeof(addr));
    TINY_LOG_INFO(logger) << "connect res = " << res << " errno = " << errno; 
    if (res)
        return;
    
    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    res = send(sock, data, sizeof(data), 0);
    TINY_LOG_INFO(logger) << "send res = " << res <<" errno " << errno;

    std::string buff;
    buff.resize(4096);

    res = recv(sock, &buff[0], buff.size(), 0);
    TINY_LOG_INFO(logger) << "recv res = " << res <<" errno " << errno;

    if (res <= 0)
        return;
    buff.resize(res);
    TINY_LOG_INFO(logger) << buff;
}

int main()
{
    //test_sleep();
    //test_socket();
    IOManager iom;
    iom.schedule(test_socket);
    return 0;
}