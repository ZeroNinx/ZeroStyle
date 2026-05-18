# ZeroStyle Lightweight Enhancement TODO

本文档记录 ZeroStyle 的轻量增强状态。当前阶段目标是：保持标准库兼容、header-only 优先、低侵入、易接入，同时沉淀个人 C++ 项目的统一风格和常用工具。

## 当前状态

ZeroStyle 的轻量 core 已基本收尾。

已具备：

- 项目定位、风格指南和接入文档。
- `include/` 公共头布局。
- CMake `INTERFACE` target：`Zero::ZeroStyle`。
- `add_subdirectory()`、`FetchContent`、`find_package()` 接入路径。
- Windows/MSVC、GCC、Clang presets。
- Windows/MSVC、GCC、Clang CI。
- 基础 header-only core 工具。
- basic-console 示例。
- package-consumer 示例。

## 已完成

### 定位边界

- [x] README 明确项目定位：ZeroStyle 是个人 C++ 风格层与小型工具库，不是 Qt/UE 式运行时框架。
- [x] 保持标准库类型优先，不新增无语义的 `ZString`、`ZVector`、`ZPath` 等包装。
- [x] Header-only core 只收纳 alias、concept、宏、traits、constexpr/inline 小工具、轻量结果类型。
- [x] 需要全局状态、后台线程、平台 API、文件句柄、第三方库后端的能力放到可选模块或示例中，不进入 core。
- [x] 增加 `docs/extension-policy.md`，说明什么可以进 core，什么不可以进 core。

### 工程接入

- [x] 扩充 `README.md`：加入快速接入、CMake 示例、设计原则、适用/不适用场景。
- [x] 补充 CMake install/export，允许其他项目通过 `find_package(ZeroStyle)` 使用。
- [x] 增加 `CMakePresets.json`，覆盖 MSVC Debug/Release、GCC、Clang、C++20、C++23。
- [x] 添加 `examples/basic-console`，展示项目公共头、`NODISCARD`、`TResult`、alias 的推荐用法。
- [x] 添加 `examples/package-consumer`，验证安装包消费路径。
- [x] 添加 CI，覆盖 Windows/MSVC、Linux/GCC、Linux/Clang。

### Header-Only Core

- [x] `Config.h`：C++ 版本、`std::expected` 检测、基础断言入口。
- [x] `Macros.h`：标准属性宏、线程安全注解。
- [x] `Types.h`：基础值类型、标准库容器、智能指针 alias。
- [x] `Error.h`：`SUnit`、`EErrorCode`、`SError`、`MakeError`。
- [x] `Result.h`：`TResult<T>`、`TVoidResult`。
- [x] `Concepts.h`：常用 concept。
- [x] `Diagnostics.h`：`ZERO_CHECK`、`ZERO_ENSURE`、`ZERO_UNREACHABLE`。
- [x] `Scope.h`：`TScopeExit` / `MakeScopeExit`。
- [x] `Flags.h`：显式 opt-in 的 `enum class` 位运算辅助。
- [x] `StringUtils.h`：少量字符串工具，如 `Trim`、`StartsWith`、`EndsWith`、`SplitView`。
- [x] `PathUtils.h`：少量路径工具，如扩展名判断、规范化展示、词法规范化。
- [x] `Format.h`：`SError` 到字符串的轻量格式化函数，不强依赖 `fmt`。

### 风格接入

- [x] README 说明新项目如何通过 `add_subdirectory()` 接入 `Zero::ZeroStyle`。
- [x] README 说明只需要宏时可单独 include `Zero/Macros.h`。
- [x] 推荐项目代码使用属性宏，不直接写标准属性。
- [x] 不强制风格检查；不保留扫描脚本。
- [x] 增加 `NODISCARD_TYPE`，用于类型级 `NODISCARD`。
- [x] 在 `style.md` 中补充宏接入与属性宏使用规范。

### 测试覆盖

- [x] 为 `TResult` 增加 copyable、move-only、error path、value path 测试。
- [x] 分别测试 C++20 fallback 与 C++23 `std::expected` 后端。
- [x] 为 `SError`、`MakeError`、`FormatError` 增加基础测试。
- [x] 为 `Types.h` alias 增加轻量编译期测试。
- [x] 为 concepts 增加正反例静态断言。
- [x] 为 `Scope.h`、`Flags.h`、`StringUtils.h`、`PathUtils.h` 增加冒烟测试。

### 工程质量

- [x] 将 warning 策略收敛到 CMake 选项：`ZERO_ENABLE_WARNINGS`。
- [x] MSVC 默认启用 `/W4`，Clang/GCC 默认启用 `-Wall -Wextra -Wpedantic`。
- [x] 将 `/WX` 或 `-Werror` 做成可选项：`ZERO_WARNINGS_AS_ERRORS`。
- [x] 保持 `.clang-format` 为唯一格式化来源。
- [x] 在 CI 中运行 build、test、install 和 package consumer 验证。

### 文档

- [x] `docs/error-handling.md`：说明何时用 `TResult`、何时用 `TOptional`、何时允许异常。
- [x] `docs/header-only-policy.md`：说明 header-only 的边界、include 成本、inline 规则。
- [x] `docs/extension-policy.md`：说明新增工具进入 core 的判断标准。
- [x] `docs/cmake-integration.md`：说明 `add_subdirectory`、`FetchContent`、install package。
- [x] `docs/examples.md`：集中展示推荐写法。

## 保留待办

这些事项不阻塞当前轻量 core 收尾，可按实际需求再做。

- [ ] 引入正式测试框架，替代只靠 `assert` 的冒烟测试。
- [ ] 为 `ZERO_CHECK` / `ZERO_ENSURE` 设计可替换失败策略，避免测试进程不可控崩溃。
- [ ] 视需要增加 `.clang-tidy`，但默认不引入过重规则。

## 可选模块

以下能力暂不进入 header-only core。只有在有明确使用场景、低侵入设计和测试边界时再单独开模块。

- [ ] `ZeroLog`：日志接口和可选后端。core 可只提供轻量前端或适配点。
- [ ] `ZeroIO`：文件读写、目录遍历、平台差异处理。
- [ ] `ZeroThread`：线程池、任务队列、同步工具。
- [ ] `ZeroJson`：第三方 JSON 库适配，不自造完整 JSON 实现。
- [ ] `ZeroTest`：测试辅助工具，例如临时目录、错误断言辅助。

## 非目标

这些不是未完成任务，而是当前项目边界。

- 不做自定义字符串体系。
- 不做自定义容器体系。
- 不做对象系统、反射系统、GC 或全局运行时。
- 不做强制依赖注入框架。
- 不把日志、线程、IO 后端塞进核心 header-only。
- 不为了风格统一包装所有标准库 API。

## 准入规则

新增能力进入 core 前，先检查：

- 是否能与标准库自然组合？
- 是否主要由 alias、template、concept、constexpr 或 inline 小函数组成？
- 是否不需要全局状态？
- 是否不依赖平台 API？
- 是否不会显著增加所有包含者的编译成本？
- 是否不强迫用户接受某个运行时模型？
- 是否能用简短文档解释清楚使用边界？

如果以上多数答案是否定的，应放入可选模块、示例或项目模板，而不是 core。
