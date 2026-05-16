#pragma once

// =============================================================================
// Zero/Config.h — 编译环境检测与基础断言
// =============================================================================

#include <cassert>

// --- C++ 版本检测 ---

#if __cplusplus >= 202302L
#    define ZERO_HAS_CXX23 1
#else
#    define ZERO_HAS_CXX23 0
#endif

#if ZERO_HAS_CXX23
#    include <expected>
#endif

// --- 调试断言 ---
// 可在包含此文件前 #define ZERO_ASSERT(Expr) 覆盖默认实现。

#ifndef ZERO_ASSERT
#    define ZERO_ASSERT(Expr) assert(Expr)
#endif
