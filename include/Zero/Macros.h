#pragma once

// =============================================================================
// Zero/Macros.h — 属性宏与线程安全注解
// =============================================================================

// --- Clang Thread Safety Analysis (TSA) ---
//
// 用法示例：
//   mutable std::mutex Mutex;
//   int Counter GUARDED_BY(Mutex) = 0;      // 变量需持锁才能访问
//   void Increment() REQUIRES(Mutex);        // 调用方必须持锁
//   void Post(Event E) EXCLUDES(Mutex);      // 函数内部自行加锁，禁止重入
//
// 非 Clang 编译器下宏展开为空，不影响编译。
// 启用静态分析：-Wthread-safety（Clang 3.6+）

#if defined(__clang__) && !defined(SWIG)
#    define ZERO_TSA(x) __attribute__((x))
#else
#    define ZERO_TSA(x)
#endif

/// 成员变量：由互斥量 x 保护
#define GUARDED_BY(x)    ZERO_TSA(guarded_by(x))
/// 指针成员：指向的数据由互斥量 x 保护
#define PT_GUARDED_BY(x) ZERO_TSA(pt_guarded_by(x))
/// 函数前置条件：调用方必须持有互斥量
#define REQUIRES(...)    ZERO_TSA(requires_capability(__VA_ARGS__))
/// 函数前置条件：调用方不得持有互斥量（函数内部自行加锁）
#define EXCLUDES(...)    ZERO_TSA(locks_excluded(__VA_ARGS__))
/// 函数效果：执行后持有互斥量
#define ACQUIRE(...)     ZERO_TSA(acquire_capability(__VA_ARGS__))
/// 函数效果：执行后释放互斥量
#define RELEASE(...)     ZERO_TSA(release_capability(__VA_ARGS__))

// --- 属性快捷宏 ---

/// [[nodiscard]] 快捷宏。返回错误、状态、资源句柄的函数应使用此宏。
#define NODISCARD [[nodiscard]]

/// 类型级 [[nodiscard]] 快捷宏。用于不应被调用点忽略的轻量值类型。
#define NODISCARD_TYPE [[nodiscard]]

/// [[noreturn]] 快捷宏。用于声明不会返回调用点的函数。
#define NORETURN [[noreturn]]

/// [[maybe_unused]] 快捷宏。用于明确允许未使用的变量、参数、函数或类型。
#define MAYBE_UNUSED [[maybe_unused]]

/// [[deprecated]] 快捷宏。用于标记不推荐继续使用的 API。
#define DEPRECATED [[deprecated]]

/// 带说明文本的 [[deprecated("message")]] 快捷宏。
#define DEPRECATED_MSG(Message) [[deprecated(Message)]]

/// [[fallthrough]] 快捷宏。用于 switch case 的有意贯穿。
#define FALLTHROUGH [[fallthrough]]

/// [[no_unique_address]] 快捷宏。用于允许空成员不占用额外存储。
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
