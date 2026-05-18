# Header-Only Policy

ZeroStyle core 默认保持 header-only。这个选择的目标是降低接入成本，让项目可以通过 `add_subdirectory()`、`FetchContent` 或安装包直接使用。

## 适合 Header-Only

- alias。
- concepts。
- traits。
- 宏。
- 小型 RAII 类型。
- `constexpr` / `inline` 函数。
- 模板工具。
- 不需要隐藏平台实现细节的轻量类型。

## 不适合 Header-Only

- 需要后台线程的组件。
- 需要文件句柄、socket、窗口句柄等平台资源的组件。
- 需要全局注册表或全局生命周期管理的组件。
- 编译成本高的大型实现。
- 强依赖第三方库的实现。
- ABI 或实现细节需要隐藏的功能。

## Include 成本

新增头文件应保持依赖窄：

- 优先包含标准库中实际需要的最小头文件。
- 不在公共头里包含重型第三方库。
- 不把可选模块的头文件加入 `ZeroStyle.h`。
- 聚合头 `ZeroStyle.h` 只包含 core 基础能力。

## Inline 规则

- 非模板函数放在头文件中时必须是 `inline`。
- `constexpr` 函数天然适合头文件。
- 函数体过大时优先考虑可选 compiled 模块，而不是继续塞进 core。
