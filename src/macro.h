#pragma once
#include <string>
#include <cassert>
#include "util.h"

#if defined __GNUC__ || defined __llvm__
#   define TINY_LICKLY(x)       __builtin_expect(!!(x), 1)
#   define TINY_UNLICKLY(x)       __builtin_expect(!!(x), 0)
#else
#   define TINY_LICKLY(x)       (x)
#   define TINY_UNLICKLY(x)       (x)
#endif

#define TINY_ASSERT(x)\
    if (TINY_UNLICKLY(!(x))) \
    { \
        TINY_LOG_ERROR(TINY_LOG_ROOT) << "ASSERTION: " << #x \
            << "\nbacktrace\n" \
            << BackTraceToString(100, 2, "    "); \
        assert(x); \
    }

#define TINY_ASSERT_P(x, w) \
    if(TINY_UNLICKLY(!(x))) { \
        TINY_LOG_ERROR(TINY_LOG_ROOT) << "ASSERTION: " << #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << BackTraceToString(100, 2, "    "); \
        assert(x); \
    }
