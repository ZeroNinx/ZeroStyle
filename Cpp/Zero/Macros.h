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
