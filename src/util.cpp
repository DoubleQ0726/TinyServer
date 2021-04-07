#include "util.h"
#include "log.h"
#include <execinfo.h>

namespace TinyServer
{
Ref<Logger> logger = TINY_LOG_NAME("system");

pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}
u_int32_t GetFiberId()
{
    return 0;
}

void BackTrace(std::vector<std::string>& bt, int size, int skip)
{
    void** buffer = (void**)malloc(sizeof(void*) * size);
    size_t nptrs = ::backtrace(buffer, size);
    char** string = ::backtrace_symbols(buffer, nptrs);
    if (!string)
    {
        TINY_LOG_ERROR(logger) << "backtrace symbols error";
        return;
    }
    for (size_t i = skip; i < nptrs; ++i)
    {
        bt.push_back(string[i]);
    }
    free(string);
    free(buffer);
}

std::string BackTraceToString(int size, int skip, std::string prefix)
{
    std::vector<std::string> bt;
    BackTrace(bt, size, skip);
    std::stringstream ss;
    for (auto& item : bt)
    {
        ss << prefix << item << std::endl;
    }
    return ss.str();
}

}


