#pragma once
#include <string>
#include <cassert>
#include "util.h"

#define TINY_ASSERT(x)\
    if (!(x)) \
    { \
        TINY_LOG_ERROR(TINY_LOG_ROOT) << "ASSERTION: " << #x \
            << "\nbacktrace\n" \
            << BackTraceToString(100, 2, "    "); \
        assert(x); \
    }

#define TINY_ASSERT_P(x, w) \
    if(!(x)) { \
        TINY_LOG_ERROR(TINY_LOG_ROOT) << "ASSERTION: " << #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << BackTraceToString(100, 2, "    "); \
        assert(x); \
    }
