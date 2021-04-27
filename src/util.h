#pragma once
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <cstdio>
#include <vector>
#include <string>

namespace TinyServer
{

pid_t GetThreadId();
u_int32_t GetFiberId();

void BackTrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

std::string BackTraceToString(int size = 64, int skip = 2, std::string prefix = "");

uint64_t GetCurrentMs();

uint64_t GetCurrentUs();

std::string TimeToStr(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");

} // namespace TinyServer

