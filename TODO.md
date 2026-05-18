# ZeroStyle 改进 TODO

本文档根据当前代码审阅结论整理，目标是继续保持 ZeroStyle 的核心定位：

- 不替代标准库。
- 不做 `ZString` / `ZVector` 这类无语义封装。
- 以 alias、轻量工具、错误处理风格和项目接入规范为主。
- 默认低侵入；个人项目可通过开关启用更短的书写风格。

---

## 优先级说明

- **P0**：建议优先处理，影响库的长期可维护性或外部接入安全。
- **P1**：推荐处理，影响可扩展性、一致性或易用性。
- **P2**：可选优化，不阻塞当前使用。

---

# P0：宏命名收敛与短名字开关

## 目标

当前 `Macros.h` 默认暴露了较短的全局宏：

```cpp
NODISCARD
NODISCARD_TYPE
NORETURN
MAYBE_UNUSED
DEPRECATED
DEPRECATED_MSG
FALLTHROUGH
NO_UNIQUE_ADDRESS
GUARDED_BY
PT_GUARDED_BY
REQUIRES
EXCLUDES
ACQUIRE
RELEASE
```

这些宏虽然适合个人项目，但宏不受 namespace 保护，默认短名字容易和宿主项目、平台头文件或第三方库冲突。

新的策略：

- 默认只提供 `ZERO_` 前缀宏。
- 个人项目需要短名字时，显式定义开关宏启用。
- 短名字宏属于个人项目便利层，不作为低侵入默认接口。

## TODO

- [x] 将标准属性宏改为默认前缀形式。

```cpp
#define ZERO_NODISCARD [[nodiscard]]
#define ZERO_NODISCARD_TYPE [[nodiscard]]
#define ZERO_NORETURN [[noreturn]]
#define ZERO_MAYBE_UNUSED [[maybe_unused]]
#define ZERO_DEPRECATED [[deprecated]]
#define ZERO_DEPRECATED_MSG(Message) [[deprecated(Message)]]
#define ZERO_FALLTHROUGH [[fallthrough]]
#define ZERO_NO_UNIQUE_ADDRESS [[no_unique_address]]
```

- [x] 将线程安全注解宏改为默认前缀形式。

```cpp
#if defined(__clang__) && !defined(SWIG)
#    define ZERO_TSA(x) __attribute__((x))
#else
#    define ZERO_TSA(x)
#endif

#define ZERO_GUARDED_BY(x)    ZERO_TSA(guarded_by(x))
#define ZERO_PT_GUARDED_BY(x) ZERO_TSA(pt_guarded_by(x))
#define ZERO_REQUIRES(...)    ZERO_TSA(requires_capability(__VA_ARGS__))
#define ZERO_EXCLUDES(...)    ZERO_TSA(locks_excluded(__VA_ARGS__))
#define ZERO_ACQUIRE(...)     ZERO_TSA(acquire_capability(__VA_ARGS__))
#define ZERO_RELEASE(...)     ZERO_TSA(release_capability(__VA_ARGS__))
```

- [x] 增加个人项目短名字开关宏。

建议名称：

```cpp
ZERO_ENABLE_SHORT_MACROS
```

启用方式：

```cpp
#define ZERO_ENABLE_SHORT_MACROS
#include <Zero/Macros.h>
```

或通过 CMake：

```cmake
target_compile_definitions(MyProject PRIVATE ZERO_ENABLE_SHORT_MACROS)
```

- [x] 在 `Macros.h` 中用开关启用短名字。

```cpp
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
```

- [x] 更新示例代码：
  - 公共示例优先使用 `ZERO_NODISCARD` 等前缀宏。
  - 个人项目示例展示 `ZERO_ENABLE_SHORT_MACROS` 后使用 `NODISCARD`、`REQUIRES` 等短宏。

- [x] 更新文档：
  - `README.md` 说明默认宏策略。
  - `style.md` 说明个人项目可启用短名字宏。
  - `docs/header-only-policy.md` 说明宏是全局预处理符号，默认必须低侵入。

## 验收标准

- [x] 默认包含 `Zero/Macros.h` 时，不再定义 `NODISCARD`、`REQUIRES` 等短宏。
- [x] 定义 `ZERO_ENABLE_SHORT_MACROS` 后，短宏可用。
- [x] 已有测试和示例在默认模式下通过。
- [x] 新增一个测试或示例验证短宏开关可用。

---

# P0：`TResult` 避免继承 `std::expected`

## 目标

当前 C++23 分支中：

```cpp
class TResult final : public std::expected<TValue, TError>
```

不建议长期继承标准库类型。标准库类型通常不是为继承设计的，并且会让 `TResult` 的 public API 暴露出一部分 `std::expected` 行为，造成 C++20 fallback 与 C++23 后端语义不一致。

建议改为组合实现。

## TODO

- [x] 将 C++23 后端改为成员存储。

```cpp
#if ZERO_HAS_EXPECTED
    std::expected<TValue, TError> Storage;
#else
    std::variant<TValue, TError> Storage;
#endif
```

- [x] 统一 `TResult` public API：
  - `Ok(TValue)`
  - `Err(TError)`
  - `IsOk()`
  - `IsErr()`
  - `operator bool()`
  - `Value() const&`
  - `TakeValue() &&`
  - `Failure() const&`

- [x] 统一失败访问策略。
  - 建议 `Value()` 在无值时使用 `ZERO_ASSERT(IsOk())`。
  - 建议 `Failure()` 在无错误时使用 `ZERO_ASSERT(IsErr())`。
  - 避免 C++23 后端抛 `std::bad_expected_access`、C++20 后端断言的差异。

- [ ] 增加 `ValueOr`、`Map`、`AndThen` 等函数前先确认是否真的需要。
  - 暂时不建议过早扩展。
  - 先保持轻量、稳定、容易理解。

## 验收标准

- [x] C++20 fallback 与 C++23 `std::expected` 后端测试均通过。
- [x] `TResult<T>` 不再继承 `std::expected<T, E>`。
- [x] `TResult` 的公开行为不依赖调用者是否使用 C++23。
- [x] README 和 `docs/error-handling.md` 不再描述继承 `std::expected`。

---

# P1：补全容器 alias 的模板参数

## 目标

当前 alias 多数只暴露最常用模板参数，例如：

```cpp
template <typename TValue>
using TVector = std::vector<TValue>;
```

这对个人代码够用，但会限制 allocator、自定义底层容器、自定义比较器、自定义 hash 等高级用法。

## TODO

- [x] 为序列容器补全 allocator / container 参数。

```cpp
template <typename TValue, typename TAllocator = std::allocator<TValue>>
using TVector = std::vector<TValue, TAllocator>;

template <typename TValue, typename TAllocator = std::allocator<TValue>>
using TDeque = std::deque<TValue, TAllocator>;

template <typename TValue, typename TAllocator = std::allocator<TValue>>
using TList = std::list<TValue, TAllocator>;

template <typename TValue, typename TContainer = std::deque<TValue>>
using TQueue = std::queue<TValue, TContainer>;

template <typename TValue, typename TContainer = std::deque<TValue>>
using TStack = std::stack<TValue, TContainer>;
```

- [x] 为关联容器补全 allocator 参数。

```cpp
template <
    typename TKey,
    typename TValue,
    typename TCompare = std::less<TKey>,
    typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
using TMap = std::map<TKey, TValue, TCompare, TAllocator>;

template <
    typename TKey,
    typename TValue,
    typename THash = std::hash<TKey>,
    typename TEqual = std::equal_to<TKey>,
    typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
using THashMap = std::unordered_map<TKey, TValue, THash, TEqual, TAllocator>;
```

- [x] 为 set / unordered_set 补全 allocator 参数。

```cpp
template <
    typename TValue,
    typename TCompare = std::less<TValue>,
    typename TAllocator = std::allocator<TValue>>
using TSet = std::set<TValue, TCompare, TAllocator>;

template <
    typename TValue,
    typename THash = std::hash<TValue>,
    typename TEqual = std::equal_to<TValue>,
    typename TAllocator = std::allocator<TValue>>
using THashSet = std::unordered_set<TValue, THash, TEqual, TAllocator>;
```

- [x] 为智能指针 alias 视情况补全 deleter。

```cpp
template <typename TValue, typename TDeleter = std::default_delete<TValue>>
using TUniquePtr = std::unique_ptr<TValue, TDeleter>;
```

## 验收标准

- [x] 原有 `TVector<T>`、`TMap<K, V>` 等写法不受影响。
- [x] 可以使用自定义 allocator / deleter / comparator / hash。
- [x] 新增静态断言或编译期测试验证 alias 等价性。

---

# P1：改善 `TVoidResult` 易用性

## 目标

当前 `TVoidResult<>` 是：

```cpp
template <typename TError = SError>
using TVoidResult = TResult<SUnit, TError>;
```

调用成功结果时可能需要写：

```cpp
return TVoidResult<>::Ok(SUnit{});
```

这不够顺手。

## TODO

- [x] 增加 `OkVoid()` 辅助函数。

```cpp
template <typename TError = SError>
NODISCARD TVoidResult<TError> OkVoid()
{
    return TVoidResult<TError>::Ok(SUnit{});
}
```

- [ ] 可选：为 `TResult<SUnit, TError>` 做偏特化，让 `TVoidResult<>::Ok()` 可用。
  - 如果实现复杂度变高，暂不做。
  - 优先保持 `OkVoid()` 简单方案。

## 验收标准

- [x] 可以写：

```cpp
TVoidResult<> SaveConfig()
{
    return OkVoid();
}
```

- [x] C++20 与 C++23 后端测试均通过。

---

# P1：`StringView` 工具补充生命周期说明与 owning 版本

## 目标

`SplitView(StringView, char)` 返回的 `StringView` 指向输入文本，调用方必须保证输入文本生命周期足够长。

危险示例：

```cpp
auto Parts = SplitView(String("a,b,c"), ',');
// Parts 中的 StringView 会悬空
```

## TODO

- [x] 在 `StringUtils.h` 中为 `SplitView` 增加明确注释。

```cpp
// 返回的 StringView 指向输入 Text。
// 调用方必须保证 Text 指向的数据在返回结果使用期间仍然有效。
```

- [x] 增加 owning 版本 `Split`。

```cpp
NODISCARD inline TVector<String> Split(
    StringView Text,
    char       Delimiter,
    bool       bKeepEmpty = false)
{
    TVector<String> Parts;

    while (true)
    {
        const std::size_t Position = Text.find(Delimiter);
        const StringView Part = Text.substr(0, Position);

        if (bKeepEmpty || !Part.empty())
        {
            Parts.emplace_back(Part);
        }

        if (Position == StringView::npos)
        {
            break;
        }

        Text.remove_prefix(Position + 1);
    }

    return Parts;
}
```

- [x] 更新文档：
  - `SplitView`：零拷贝，但需要生命周期保证。
  - `Split`：拥有字符串副本，安全返回。

## 验收标准

- [x] `SplitView` 与 `Split` 均有测试。
- [x] 测试覆盖空字段保留与丢弃。
- [x] 文档明确说明生命周期约束。

---

# P1：CMake 降低对宿主项目的侵入

## 目标

ZeroStyle 作为 header-only / interface 风格库，不应修改宿主项目的全局 CMake 状态，也不应默认向消费者强推额外 warning。

## TODO

- [x] 避免在库本身强行设置全局 C++ 标准。

不推荐：

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 23)
```

推荐：

```cmake
target_compile_features(ZeroStyle INTERFACE cxx_std_20)
```

- [x] C++23 测试通过 preset 或测试 target 控制，不影响库消费者。

- [x] 将 Clang thread safety warning 做成可选项。

```cmake
option(ZERO_ENABLE_THREAD_SAFETY_ANALYSIS
    "Enable Clang thread safety analysis for consumers"
    OFF)

if(ZERO_ENABLE_THREAD_SAFETY_ANALYSIS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ZeroStyle INTERFACE -Wthread-safety)
endif()
```

- [x] 确认 `ZERO_ENABLE_WARNINGS` 只影响本项目测试 / 示例，或文档明确说明它会影响消费者。

## 验收标准

- [x] `add_subdirectory(ZeroStyle)` 不会意外改变宿主项目全局 C++ 标准。
- [x] 默认不会给消费者添加 `-Wthread-safety`。
- [x] CI 仍覆盖 MSVC / GCC / Clang / C++20 / C++23。

---

# P1：公共头文件接入规范

## 目标

ZeroStyle 可以改善项目内部书写体验，但公共头文件仍应保持边界清晰，避免把风格库的短名字不受控地扩散给所有包含者。

## TODO

- [x] 明确公共头文件推荐写法。

```cpp
#pragma once

#include <ZeroStyle.h>

namespace MyProject {

using Zero::String;
using Zero::StringView;
using Zero::Path;
using Zero::TVector;
using Zero::TOptional;
using Zero::TResult;

class SConfig final
{
public:
    ZERO_NODISCARD TResult<TVector<String>> LoadPaths(const Path& RootPath);
};

}  // namespace MyProject
```

- [x] 明确 `.cpp` 文件允许更自由的局部使用方式。

```cpp
#define ZERO_ENABLE_SHORT_MACROS
#include <ZeroStyle.h>

using namespace Zero;

NODISCARD TVector<String> BuildNames()
{
    return {};
}
```

- [x] 文档中强调：
  - 不在公共头文件写 `using namespace Zero;`。
  - 不在公共头文件默认启用短名字宏，除非该项目明确把短宏作为公共风格契约。
  - 推荐在项目公共头 `ProjectCore.h` 中集中 re-export 需要的 alias。

## 验收标准

- [x] README 有“公共头文件 / .cpp 文件”的对比示例。
- [x] examples 中有 `ProjectCore.h` 推荐写法。
- [x] 文档明确短名字宏只适合个人项目或受控项目边界。

---

# P2：保留标准库关键语义工具的显式 `std::`

## 目标

并不是所有 `std::` 都值得隐藏。某些工具属于 C++ 语义关键点，显式写出 `std::` 更清楚。

## TODO

- [x] 文档中明确不建议 alias / 包装以下工具：

```cpp
std::move
std::forward
std::swap
std::sort
std::find
std::ranges::find
std::visit
std::get
std::holds_alternative
```

- [x] 除非增加明确项目语义，否则不新增：

```cpp
Zero::Move
Zero::Forward
Zero::Sort
Zero::Find
```

- [x] 仅在需要统一项目行为时才封装算法，例如：
  - 固定排序规则。
  - 统一错误处理。
  - 统一日志 / 诊断。
  - 明确业务语义。

## 验收标准

- [x] `docs/extension-policy.md` 中增加“不要为了少写 `std::` 而包装语义关键工具”的规则。
- [x] 代码库中没有无语义的 `Move`、`Forward`、`Sort`、`Find` 包装。

---

# P2：补充语义类型设计规范

## 目标

alias 用于统一风格，封装用于表达不变量。需要文档明确区分二者。

## TODO

- [x] 在 `docs/extension-policy.md` 增加判断规则。

```text
只是为了少写 std::：使用 alias，不要封装。
存在业务语义、不变量或错误规则：可以封装。
封装后会遮蔽标准库行为且无新语义：不要加入。
```

- [x] 增加推荐示例。

适合 alias：

```cpp
using String = std::string;
template <typename T>
using TVector = std::vector<T>;
```

适合封装：

```cpp
class ZEmailAddress;
class ZAssetPath;
class ZNormalizedPath;
class ZConfigDocument;
```

不适合封装：

```cpp
class ZString : public std::string {};
template <typename T>
class ZVector : public std::vector<T> {};
```

## 验收标准

- [x] 文档能清楚说明 alias 与 wrapper 的边界。
- [x] 新增工具进入 core 前有明确准入标准。

---

# P2：测试体系增强

## 目标

当前冒烟测试已经能覆盖基础行为。后续如果工具继续增加，建议引入更正式的测试框架。

## TODO

- [ ] 选择轻量测试框架。
  - 可选：Catch2。
  - 可选：doctest。
  - 如果继续保持零依赖，可以暂时保留 `assert`。

- [x] 增加以下测试项：
  - 短宏默认不启用。
  - `ZERO_ENABLE_SHORT_MACROS` 后短宏可用。
  - `TResult` C++20 / C++23 行为一致。
  - 容器 alias 自定义 allocator / comparator / hash 能编译。
  - `SplitView` 与 `Split` 的空字段行为。

## 验收标准

- [ ] CI 中测试输出更清楚。
- [x] 新增功能都有对应测试。
- [x] C++20 与 C++23 分支都被覆盖。

---

# 建议执行顺序

## 第一轮：低风险、高收益

- [x] 宏改为 `ZERO_` 前缀默认接口。
- [x] 增加 `ZERO_ENABLE_SHORT_MACROS`。
- [x] 更新 README / style / examples 中的宏说明。
- [x] 给 `SplitView` 增加生命周期注释。
- [x] 增加 `Split` owning 版本。

## 第二轮：核心语义稳定

- [x] `TResult` 改为组合实现。
- [x] 统一 C++20 fallback 与 C++23 后端行为。
- [x] 增加 `OkVoid()`。
- [x] 补全相关测试。

## 第三轮：工程与扩展性

- [x] 容器 alias 补全模板参数。
- [x] CMake 降低对宿主项目侵入。
- [x] 更新 extension policy。
- [ ] 视需要引入正式测试框架。

---

# 总体完成标准

完成以上改进后，ZeroStyle 应满足：

- 默认接入低侵入，不污染短宏名。
- 个人项目可显式开启短宏，保持书写舒适。
- alias 层保持透明，不改变标准库语义。
- 封装只用于表达不变量、错误模型或业务语义。
- `TResult` 行为在 C++20 / C++23 下保持一致。
- CMake 不意外影响宿主项目。
- 文档能明确指导何时 alias、何时封装、何时保留 `std::`。
