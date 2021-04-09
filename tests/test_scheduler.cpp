#include "TinyServer.h"
using namespace TinyServer;
Ref<Logger> logger = TINY_LOG_ROOT;

int main()
{
    Scheduler sc;
    sc.start();
    sc.stop();
    return 0;
}