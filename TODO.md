# ZeroStyle Lightweight Enhancement TODO

本文档用于记录 ZeroStyle 的轻量增强方向。目标不是把 ZeroStyle 做成底层框架，而是保持标准库兼容、header-only 优先、低侵入、易接入，同时沉淀个人 C++ 项目的统一风格和常用工具。

## 定位边界

- [ ] 明确 README 中的项目定位：ZeroStyle 是个人 C++ 风格层与小型工具库，不是 Qt/UE 式运行时框架。
- [ ] 保持标准库类型优先，不新增无语义的 `ZString`、`ZVector`、`ZPath` 等包装。
- [ ] Header-only core 只收纳 alias、concept、宏、traits、constexpr/inline 小工具、轻量结果类型。
- [ ] 需要全局状态、后台线程、平台 API、文件句柄、第三方库后端的能力放到可选模块或示例中，不进入 core。
- [ ] 为“什么可以进 core，什么不可以进 core”写一页 `docs/extension-policy.md`。

## 近期优先级

- [ ] 扩充 `README.md`：加入快速接入、CMake 示例、设计原则、适用/不适用场景。
- [ ] 补充 CMake install/export，允许其他项目通过 `find_package(ZeroStyle)` 或包导出使用。
- [ ] 增加 `CMakePresets.json`，提供常用配置：MSVC Debug/Release、Clang、测试开关。
- [ ] 引入正式测试框架，替代只靠 `assert` 的冒烟测试。
- [ ] 添加 `examples/basic-console`，展示 `ZeroStyle.h`、`NODISCARD`、`TResult`、alias 的推荐用法。
- [ ] 添加 CI，至少覆盖 Windows MSVC + C++20，后续再补 Clang/GCC 和 C++23。

## Header-Only Core

- [x] `Config.h`：C++ 版本、`std::expected` 检测、基础断言入口。
- [x] `Macros.h`：`NODISCARD`、线程安全注解。
- [x] `Types.h`：基础值类型、标准库容器、智能指针 alias。
- [x] `Error.h`：`SUnit`、`EErrorCode`、`SError`、`MakeError`。
- [x] `Result.h`：`TResult<T>`、`TVoidResult`。
- [x] `Concepts.h`：常用 concept。
- [ ] `Diagnostics.h`：`ZERO_CHECK`、`ZERO_ENSURE`、`ZERO_UNREACHABLE` 等轻量诊断宏。
- [ ] `Scope.h`：`TScopeExit` / `MakeScopeExit`，用于局部 RAII 清理。
- [ ] `Flags.h`：`enum class` 位运算辅助，要求显式 opt-in。
- [ ] `StringUtils.h`：少量字符串工具，如 `Trim`、`StartsWith`、`EndsWith`、`SplitView`。
- [ ] `PathUtils.h`：少量路径工具，如扩展名判断、规范化展示、相对路径辅助。
- [ ] `Format.h`：`SError` 到字符串的轻量格式化函数，不强依赖 `fmt`。

## 风格接入

- [ ] 在 README 中说明新项目如何通过 `add_subdirectory()` 接入 `Zero::ZeroStyle`。
- [ ] 在 README 中说明只需要宏时可单独 include `Zero/Macros.h`。
- [ ] 建立规则：项目代码统一使用 `NODISCARD`，不直接写 `[[nodiscard]]`。
- [ ] 增加检查脚本或 CI 命令：扫描项目代码中的直接 `[[nodiscard]]` 使用。
- [ ] 视需要增加 `NODISCARD_TYPE`，用于类型级 `[[nodiscard]]`。
- [ ] 在 `style.md` 中补充宏接入与属性宏使用规范。

## 测试增强

- [ ] 为 `TResult` 增加 copyable、move-only、error path、value path 测试。
- [ ] 分别测试 C++20 fallback 与 C++23 `std::expected` 后端。
- [ ] 为 `SError`、`MakeError` 增加基础测试。
- [ ] 为 `Types.h` alias 增加轻量编译期测试。
- [ ] 为 concepts 增加正反例静态断言。
- [ ] 为 `ZERO_CHECK` / `ZERO_ENSURE` 设计可测试的失败策略，避免测试进程不可控崩溃。

## 工程质量

- [ ] 将 warning 策略收敛到 CMake 选项，如 `ZERO_ENABLE_WARNINGS`。
- [ ] MSVC 默认启用 `/W4`，Clang/GCC 默认启用 `-Wall -Wextra`。
- [ ] 将 `/WX` 或 `-Werror` 做成可选项，不默认强制用户项目。
- [ ] 保持 `.clang-format` 为唯一格式化来源。
- [ ] 视需要增加 `.clang-tidy`，但默认不引入过重规则。
- [ ] 在 CI 中运行 format check、build、test。

## 可选模块

以下能力暂不进入 header-only core，除非有明确的低侵入设计。

- [ ] `ZeroLog`：日志接口和可选后端。core 可只提供轻量前端或适配点。
- [ ] `ZeroIO`：文件读写、目录遍历、平台差异处理。
- [ ] `ZeroThread`：线程池、任务队列、同步工具。
- [ ] `ZeroJson`：第三方 JSON 库适配，不自造完整 JSON 实现。
- [ ] `ZeroTest`：测试辅助工具，例如临时目录、错误断言辅助。

## 文档

- [ ] `docs/error-handling.md`：说明何时用 `TResult`、何时用 `TOptional`、何时允许异常。
- [ ] `docs/header-only-policy.md`：说明 header-only 的边界、include 成本、inline 规则。
- [ ] `docs/extension-policy.md`：说明新增工具进入 core 的判断标准。
- [ ] `docs/cmake-integration.md`：说明 `add_subdirectory`、`FetchContent`、install package。
- [ ] `docs/examples.md`：集中展示推荐写法。

## 暂不做

- [ ] 不做自定义字符串体系。
- [ ] 不做自定义容器体系。
- [ ] 不做对象系统、反射系统、GC 或全局运行时。
- [ ] 不做强制依赖注入框架。
- [ ] 不把日志、线程、IO 后端塞进核心 header-only。
- [ ] 不为了风格统一包装所有标准库 API。

## 判断标准

新增能力进入 core 前，先检查：

- [ ] 是否能与标准库自然组合？
- [ ] 是否主要由 alias、template、concept、constexpr 或 inline 小函数组成？
- [ ] 是否不需要全局状态？
- [ ] 是否不依赖平台 API？
- [ ] 是否不会显著增加所有包含者的编译成本？
- [ ] 是否不强迫用户接受某个运行时模型？
- [ ] 是否能用简短文档解释清楚使用边界？

如果以上多数答案是否定的，应放入可选模块、示例或项目模板，而不是 core。
