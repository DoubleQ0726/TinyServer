#include "TinyServer.h"
#include "macro.h"
using namespace TinyServer;

Ref<Logger> logger = TINY_LOG_ROOT;

void test_assert()
{
    //TINY_LOG_INFO(logger) << "\n" <<BackTraceToString(10);
    //TINY_ASSERT(0 == 1);
    TINY_ASSERT_P(0 == 1, "abcd xx");
}

int main()
{
    test_assert();
    return 0;
}