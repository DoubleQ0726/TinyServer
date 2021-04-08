#include "TinyServer.h"

using namespace TinyServer;

Ref<Logger> logger = TINY_LOG_ROOT;

void run_in_fiber()
{
    TINY_LOG_INFO(logger) << "run_in_fiber begin";
    Fiber::YieldToReady();
    TINY_LOG_INFO(logger) << "run_in_fiber end";
    Fiber::YieldToReady();
}

void test_fiber()
{
    TINY_LOG_INFO(logger) << "main -1";
    {
    Fiber::GetThis();
    TINY_LOG_INFO(logger) << "main begin";
    Ref<Fiber> fiber(new Fiber(&run_in_fiber));
    fiber->swapIn();
    TINY_LOG_INFO(logger) << "main after swapIn";
    fiber->swapIn();
    TINY_LOG_INFO(logger) << "main end";
    //此处必须再切回子协程让其运行run_in_fiber(), 将第一个Fiber::YieldToReady()增加的引用计数减一，才可以最终将引用计数降为0而析构
    fiber->swapIn();
    }
    TINY_LOG_INFO(logger) << "main end2";
}

int main()
{
    Thread::SetName("main");
    std::vector<Ref<Thread>> threads;
    for (int i = 0; i < 3; ++i)
    {
        threads.push_back(Ref<Thread>(new Thread(&test_fiber, "name_" + std::to_string(i))));
    }

    for (int i = 0; i < 3; ++i)
    {
        threads[i]->join();
    }
    return 0;
}