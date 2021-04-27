#include "daemon.h"
#include "iomanager.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

Ref<Timer> timer;
int server_main(int argc, char** argv)
{
    TINY_LOG_INFO(logger) << ProcessInfoMgr::GetInstance()->toString(); 
    IOManager iom(1);
    timer = iom.addTimer(1000, [](){
        TINY_LOG_INFO(logger) << "OnTimer";
        static int count = 0;
        if (++count > 10)
        {
            timer->cancle();
        }
    }, true);
    return 0;
}

int main(int argc, char** argv)
{
    return start_daemon(argc, argv, server_main, argc != 1);
}