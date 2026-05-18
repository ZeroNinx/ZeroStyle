# Extension Policy

ZeroStyle 的 core 只接受低侵入、标准库兼容、header-only 友好的能力。新增工具进入 core 前，先判断它是否真的适合作为所有项目的默认基础设施。

## 可以进入 Core

- 标准库 alias。
- C++20 concepts。
- traits、type utilities。
- `constexpr` / `consteval` 小函数。
- `inline` 小工具函数。
- 不持有全局状态的轻量 RAII 类型。
- 不依赖平台 API 的宏和编译期配置。
- 与标准库类型自然组合的结果、错误、范围、flags 工具。

## 不进入 Core

- 自定义字符串体系。
- 自定义容器体系。
- 对象系统、反射系统、GC 或全局运行时。
- 线程池、任务队列、日志后端、文件监听等运行时组件。
- Windows、Linux、macOS 平台 API 封装。
- 强依赖第三方库的功能。
- 会明显增加所有包含者编译成本的重型头文件。
- 只是为了少写 `std::` 而包装的语义关键工具。

## Alias 与封装

只是为了统一书写风格或减少模板噪音时，使用 alias，不做封装：

```cpp
using String = std::string;

template <typename TValue>
using TVector = std::vector<TValue>;
```

存在业务语义、不变量或错误规则时，才考虑封装：

```cpp
class ZEmailAddress;
class ZAssetPath;
class ZNormalizedPath;
class ZConfigDocument;
```

不推荐无语义封装：

```cpp
class ZString : public std::string {};

template <typename TValue>
class ZVector : public std::vector<TValue> {};
```

也不建议为了少写 `std::` 而包装这些 C++ 语义关键工具：

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

只有在需要固定项目行为、统一错误处理、统一日志/诊断或表达明确业务语义时，才封装标准算法。

## 默认放到可选模块

以下能力可以做，但不应该默认进入 core：

- `ZeroLog`：日志前端、sink、文件日志。
- `ZeroIO`：文件读写、目录遍历、平台差异处理。
- `ZeroThread`：线程池、任务队列。
- `ZeroJson`：第三方 JSON 库适配。
- `ZeroTest`：测试辅助工具。

## 判断问题

新增能力前先回答：

- 它是否能与标准库自然组合？
- 它是否主要由 alias、template、concept、constexpr 或 inline 小函数组成？
- 它是否不需要全局状态？
- 它是否不依赖平台 API？
- 它是否不会显著增加所有包含者的编译成本？
- 它是否不强迫用户接受某个运行时模型？
- 它是否能用简短文档解释清楚使用边界？

多数答案为“否”时，应放入可选模块、示例或项目模板，而不是 core。
