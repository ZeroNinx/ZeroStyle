# ZeroStyle TODO

ZeroStyle 当前保持零依赖、header-only、标准库兼容、低侵入的定位。暂不引入 Catch2、doctest 等测试框架；现阶段继续使用 CTest + 小型可执行文件覆盖行为。

## 已收敛

- [x] 默认宏改为 `ZERO_` 前缀，避免污染宿主项目全局宏名。
- [x] 增加 `ZERO_ENABLE_SHORT_MACROS`，个人项目可显式启用短宏。
- [x] `TResult` 不再继承 `std::expected`，C++23 仅作为内部存储后端。
- [x] `TResult` C++20 / C++23 公开 API 保持一致。
- [x] 增加 `OkVoid()`，简化 `TVoidResult<>` 成功返回。
- [x] 容器 alias 补全 allocator / comparator / hash / deleter 等模板参数。
- [x] `SplitView` 增加生命周期说明。
- [x] 增加 owning 版本 `Split`。
- [x] CMake 不再设置全局 `CMAKE_CXX_STANDARD`。
- [x] Clang thread-safety warning 改为显式选项。
- [x] 文档补充 alias / wrapper 边界和标准库语义工具保留规则。
- [x] 测试覆盖短宏默认不启用、短宏开关、`TResult`、alias 参数、`SplitView` / `Split`。
- [x] 拆分 smoke executable，改善 CTest 输出可读性。
- [x] 增加 `ValueOr`、`Map`、`AndThen`。
- [x] 增加 `TResult<SUnit, TError>` 偏特化。
- [x] 增加轻量 `.clang-tidy`，不作为消费者要求。

## 当前保留待办

- [x] 评估并补充 `ValueOr`、`Map`、`AndThen`。
  - 已覆盖 C++20 fallback 和 C++23 后端。
  - `Map` 返回 `void` 时转换为 `TVoidResult<TError>`，不引入 `TResult<void>`。
- [x] 增加 `TResult<SUnit, TError>` 偏特化。
  - `OkVoid()` 保留。
  - `TVoidResult<>::Ok()` 可直接使用。
- [x] 改善 CTest 输出可读性。
  - 保持零依赖。
  - 已拆分为多个小型 smoke executable，而不是引入测试框架。
- [x] 视需要增加 `.clang-tidy`。
  - 默认不启用重规则。
  - 不把 clang-tidy 作为消费者要求。

## 可选模块

以下能力不进入 core，只有在具体项目需要时再单独设计。

- [ ] `ZeroLog`：日志接口和可选后端。
- [ ] `ZeroIO`：文件读写、目录遍历、平台差异处理。
- [ ] `ZeroThread`：线程池、任务队列、同步工具。
- [ ] `ZeroJson`：第三方 JSON 库适配，不自造完整 JSON 实现。
- [ ] `ZeroTest`：零依赖测试辅助，例如临时目录、错误断言辅助。

## 非目标

- 不做自定义字符串体系。
- 不做自定义容器体系。
- 不做对象系统、反射系统、GC 或全局运行时。
- 不做强制依赖注入框架。
- 不把日志、线程、IO 后端塞进核心 header-only。
- 不为了风格统一包装所有标准库 API。
- 不为了少写 `std::` 增加 `Zero::Move`、`Zero::Forward`、`Zero::Sort`、`Zero::Find` 等无语义封装。

## Core 准入规则

新增能力进入 core 前，先检查：

- 是否能与标准库自然组合？
- 是否主要由 alias、template、concept、constexpr 或 inline 小函数组成？
- 是否不需要全局状态？
- 是否不依赖平台 API？
- 是否不会显著增加所有包含者的编译成本？
- 是否不强迫用户接受某个运行时模型？
- 是否能用简短文档解释清楚使用边界？

多数答案为“否”时，应放入可选模块、示例或项目模板，而不是 core。
