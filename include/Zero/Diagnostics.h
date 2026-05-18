#pragma once

// =============================================================================
// Zero/Diagnostics.h — 轻量诊断宏
// =============================================================================
//
// ZERO_CHECK:       运行时必须满足的条件，失败时终止进程。
// ZERO_ENSURE:      返回 bool 的软检查，失败时触发断言并返回 false。
// ZERO_UNREACHABLE: 标记不可达路径，触达时终止进程。

#include "Config.h"
#include "Macros.h"

#include <cstdlib>

namespace Zero::Detail {

NORETURN inline void FailCheck(
    const char* Expression,
    const char* File,
    int         Line) noexcept
{
    (void)Expression;
    (void)File;
    (void)Line;

    ZERO_ASSERT(false && "ZERO_CHECK failed");
    std::abort();
}

inline bool FailEnsure(
    const char* Expression,
    const char* File,
    int         Line) noexcept
{
    (void)Expression;
    (void)File;
    (void)Line;

    ZERO_ASSERT(false && "ZERO_ENSURE failed");
    return false;
}

NORETURN inline void FailUnreachable(
    const char* File,
    int         Line) noexcept
{
    (void)File;
    (void)Line;

    ZERO_ASSERT(false && "ZERO_UNREACHABLE reached");
    std::abort();
}

}  // namespace Zero::Detail

#define ZERO_CHECK(Expression)                                      \
    do                                                             \
    {                                                              \
        if (!(Expression))                                         \
        {                                                          \
            ::Zero::Detail::FailCheck(                             \
                #Expression,                                      \
                __FILE__,                                         \
                __LINE__);                                        \
        }                                                          \
    } while (false)

#define ZERO_ENSURE(Expression) \
    (!!(Expression) || ::Zero::Detail::FailEnsure(#Expression, __FILE__, __LINE__))

#define ZERO_UNREACHABLE() ::Zero::Detail::FailUnreachable(__FILE__, __LINE__)
