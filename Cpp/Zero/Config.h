#pragma once

// =============================================================================
// Zero/Config.h — 编译环境检测与基础断言
// =============================================================================

#include <cassert>

// --- C++ 版本检测 ---
// MSVC 默认情况下 __cplusplus 可能不是实际语言标准版本，优先使用 _MSVC_LANG。

#if defined(_MSVC_LANG) && !defined(__clang__)
#    define ZERO_CPLUSPLUS _MSVC_LANG
#else
#    define ZERO_CPLUSPLUS __cplusplus
#endif

#if ZERO_CPLUSPLUS >= 202302L
#    define ZERO_HAS_CXX23 1
#else
#    define ZERO_HAS_CXX23 0
#endif

// --- std::expected 检测 ---
// C++23 标准模式不保证当前标准库一定已经提供 <expected>，因此单独检测。

#if ZERO_HAS_CXX23 && defined(__has_include)
#    if __has_include(<expected>)
#        include <expected>
#        if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
#            define ZERO_HAS_EXPECTED 1
#        else
#            define ZERO_HAS_EXPECTED 0
#        endif
#    else
#        define ZERO_HAS_EXPECTED 0
#    endif
#else
#    define ZERO_HAS_EXPECTED 0
#endif

// --- 调试断言 ---
// 可在包含此文件前 #define ZERO_ASSERT(Expr) 覆盖默认实现。

#ifndef ZERO_ASSERT
#    define ZERO_ASSERT(Expr) assert(Expr)
#endif
