#include "util.h"
#include "log.h"
#include <execinfo.h>
#include "fiber.h"
#include <sys/time.h>

namespace TinyServer
{
Ref<Logger> logger = TINY_LOG_NAME("system");

pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}
u_int32_t GetFiberId()
{
    return Fiber::GetFiberId();
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

uint64_t GetCurrentMs()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurrentUs()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}

std::string TimeToStr(time_t ts, const std::string& format)
{
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
}

}


