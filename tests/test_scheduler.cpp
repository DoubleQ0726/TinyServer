#include "TinyServer.h"
using namespace TinyServer;
Ref<Logger> logger = TINY_LOG_ROOT;

void test_fiber()
{
    TINY_LOG_INFO(logger) << "test_fiber";
}

int main()
{
    TINY_LOG_INFO(logger) << "main begin";
    Scheduler sc;
    sc.schedule(test_fiber);
    sc.start();
    sc.stop();
    TINY_LOG_INFO(logger) << "main end";
    return 0;
}