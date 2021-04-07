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

void BackTrace(std::vector<std::string>& bt, int size, int skip = 1);

std::string BackTraceToString(int size, int skip = 2, std::string prefix = "");

} // namespace TinyServer

