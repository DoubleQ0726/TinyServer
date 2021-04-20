#include "TinyServer.h"
using namespace TinyServer;
Ref<Logger> logger = TINY_LOG_ROOT;

void test_fiber()
{
    TINY_LOG_INFO(logger) << "test_fiber";
    sleep(1);
    static int s_count = 5;
    if (--s_count >= 0)
    {
        Scheduler::GetThis()->schedule(test_fiber, GetThreadId());
    } 
}

int main()
{
    TINY_LOG_INFO(logger) << "main begin";
    Scheduler sc(3, false, "test");
    sc.start();                 //开启多线程多协程
    sleep(2);
    sc.schedule(test_fiber);    //注册协程任务
    sc.stop();                  //在此函数中执行协程函数
    TINY_LOG_INFO(logger) << "main end";
    return 0;
}