#pragma once

// =============================================================================
// Zero/Macros.h — 属性宏与线程安全注解
// =============================================================================

// --- Clang Thread Safety Analysis (TSA) ---
//
// 用法示例：
//   mutable std::mutex Mutex;
//   int Counter ZERO_GUARDED_BY(Mutex) = 0;      // 变量需持锁才能访问
//   void Increment() ZERO_REQUIRES(Mutex);        // 调用方必须持锁
//   void Post(Event E) ZERO_EXCLUDES(Mutex);      // 函数内部自行加锁，禁止重入
//
// 非 Clang 编译器下宏展开为空，不影响编译。
// 启用静态分析：-Wthread-safety（Clang 3.6+）

#if defined(__clang__) && !defined(SWIG)
#    define ZERO_TSA(x) __attribute__((x))
#else
#    define ZERO_TSA(x)
#endif

/// 成员变量：由互斥量 x 保护
#define ZERO_GUARDED_BY(x)    ZERO_TSA(guarded_by(x))
/// 指针成员：指向的数据由互斥量 x 保护
#define ZERO_PT_GUARDED_BY(x) ZERO_TSA(pt_guarded_by(x))
/// 函数前置条件：调用方必须持有互斥量
#define ZERO_REQUIRES(...)    ZERO_TSA(requires_capability(__VA_ARGS__))
/// 函数前置条件：调用方不得持有互斥量（函数内部自行加锁）
#define ZERO_EXCLUDES(...)    ZERO_TSA(locks_excluded(__VA_ARGS__))
/// 函数效果：执行后持有互斥量
#define ZERO_ACQUIRE(...)     ZERO_TSA(acquire_capability(__VA_ARGS__))
/// 函数效果：执行后释放互斥量
#define ZERO_RELEASE(...)     ZERO_TSA(release_capability(__VA_ARGS__))

// --- 属性快捷宏 ---

/// [[nodiscard]] 快捷宏。返回错误、状态、资源句柄的函数应使用此宏。
#define ZERO_NODISCARD [[nodiscard]]

/// 类型级 [[nodiscard]] 快捷宏。用于不应被调用点忽略的轻量值类型。
#define ZERO_NODISCARD_TYPE [[nodiscard]]

/// [[noreturn]] 快捷宏。用于声明不会返回调用点的函数。
#define ZERO_NORETURN [[noreturn]]

/// [[maybe_unused]] 快捷宏。用于明确允许未使用的变量、参数、函数或类型。
#define ZERO_MAYBE_UNUSED [[maybe_unused]]

/// [[deprecated]] 快捷宏。用于标记不推荐继续使用的 API。
#define ZERO_DEPRECATED [[deprecated]]

/// 带说明文本的 [[deprecated("message")]] 快捷宏。
#define ZERO_DEPRECATED_MSG(Message) [[deprecated(Message)]]

/// [[fallthrough]] 快捷宏。用于 switch case 的有意贯穿。
#define ZERO_FALLTHROUGH [[fallthrough]]

/// [[no_unique_address]] 快捷宏。用于允许空成员不占用额外存储。
#define ZERO_NO_UNIQUE_ADDRESS [[no_unique_address]]

// --- 个人项目短名字宏 --------------------------------------------------------
//
// 默认不暴露短宏，避免污染宿主项目的全局预处理符号。
// 需要个人项目风格短名字时，在包含 Zero/Macros.h 或 ZeroStyle.h 前定义：
//
//   #define ZERO_ENABLE_SHORT_MACROS
//
// 或通过 CMake 为目标添加 ZERO_ENABLE_SHORT_MACROS。

#ifdef ZERO_ENABLE_SHORT_MACROS

#    ifndef NODISCARD
#        define NODISCARD ZERO_NODISCARD
#    endif

#    ifndef NODISCARD_TYPE
#        define NODISCARD_TYPE ZERO_NODISCARD_TYPE
#    endif

#    ifndef NORETURN
#        define NORETURN ZERO_NORETURN
#    endif

#    ifndef MAYBE_UNUSED
#        define MAYBE_UNUSED ZERO_MAYBE_UNUSED
#    endif

#    ifndef DEPRECATED
#        define DEPRECATED ZERO_DEPRECATED
#    endif

#    ifndef DEPRECATED_MSG
#        define DEPRECATED_MSG(Message) ZERO_DEPRECATED_MSG(Message)
#    endif

#    ifndef FALLTHROUGH
#        define FALLTHROUGH ZERO_FALLTHROUGH
#    endif

#    ifndef NO_UNIQUE_ADDRESS
#        define NO_UNIQUE_ADDRESS ZERO_NO_UNIQUE_ADDRESS
#    endif

#    ifndef GUARDED_BY
#        define GUARDED_BY(x) ZERO_GUARDED_BY(x)
#    endif

#    ifndef PT_GUARDED_BY
#        define PT_GUARDED_BY(x) ZERO_PT_GUARDED_BY(x)
#    endif

#    ifndef REQUIRES
#        define REQUIRES(...) ZERO_REQUIRES(__VA_ARGS__)
#    endif

#    ifndef EXCLUDES
#        define EXCLUDES(...) ZERO_EXCLUDES(__VA_ARGS__)
#    endif

#    ifndef ACQUIRE
#        define ACQUIRE(...) ZERO_ACQUIRE(__VA_ARGS__)
#    endif

#    ifndef RELEASE
#        define RELEASE(...) ZERO_RELEASE(__VA_ARGS__)
#    endif

#endif  // ZERO_ENABLE_SHORT_MACROS
