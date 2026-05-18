# ZeroStyle

ZeroStyle 是一套面向个人项目的轻量 C++ 风格层与 header-only 工具库。

它的目标不是重新发明标准库，也不是构建一套底层运行时框架，而是在保持标准库兼容的前提下，为 C++ 项目提供统一的命名规范、常用 alias、轻量工具类型和工程约定。

## 设计目标

- 标准库优先，不做无语义封装。
- Header-only 优先，方便复制、接入和审查。
- 低侵入，不强迫项目接受特定运行时模型。
- 风格统一，通过 `include/ZeroStyle.h` 和 `include/Zero/` 下的独立头文件提供基础能力。
- 可扩展，但新增能力必须保持边界清晰。

## 当前内容

- `include/Zero/Types.h`：基础整数、字符串、路径、容器、智能指针 alias。
- `include/Zero/Macros.h`：`ZERO_NODISCARD`、`ZERO_NORETURN`、`ZERO_MAYBE_UNUSED`、Clang 线程安全注解等宏。
- `include/Zero/Config.h`：C++ 版本检测、`std::expected` 检测、基础断言入口。
- `include/Zero/Error.h`：`SUnit`、`EErrorCode`、`SError`、`MakeError`。
- `include/Zero/Result.h`：`TResult<T>`、`TVoidResult`、`OkVoid()`、`ValueOr`、`Map`、
  `AndThen`，C++23 可用时内部使用 `std::expected`。
- `include/Zero/Concepts.h`：常用 C++20 concepts。
- `include/Zero/Diagnostics.h`：`ZERO_CHECK`、`ZERO_ENSURE`、`ZERO_UNREACHABLE`。
- `include/Zero/Scope.h`：`TScopeExit`、`MakeScopeExit`。
- `include/Zero/Flags.h`：显式 opt-in 的 `enum class` 位运算辅助。
- `include/Zero/StringUtils.h`：`Trim`、`StartsWith`、`EndsWith`、`SplitView`、`Split`。
- `include/Zero/PathUtils.h`：路径展示、扩展名和词法规范化辅助。
- `include/Zero/Format.h`：`EErrorCode` 与 `SError` 的轻量字符串格式化。
- `include/ZeroStyle.h`：聚合入口头文件。

## CMake 接入

作为子目录接入：

```cmake
add_subdirectory(path/to/ZeroStyle)

target_link_libraries(MyTarget
    PRIVATE
        Zero::ZeroStyle
)
```

推荐在项目里建立一个公共基础头，例如 `ProjectCore.h`，由它选择性转发 ZeroStyle 中需要的类型。公共头文件不要写 `using namespace Zero;`。

```cpp
// ProjectCore.h
#pragma once

#include "ZeroStyle.h"

namespace MyProject {

using Zero::Path;
using Zero::String;
using Zero::StringView;
using Zero::TResult;
using Zero::TVector;
using Zero::int32;

}  // namespace MyProject
```

业务头文件通过项目公共头获得统一风格：

```cpp
#include "ProjectCore.h"

namespace MyProject {

ZERO_NODISCARD TResult<String> LoadText(const Path& FilePath);

}  // namespace MyProject
```

默认只暴露 `ZERO_` 前缀宏，避免污染宿主项目的全局预处理符号。个人项目如果希望使用短宏，可以在包含 ZeroStyle 前显式启用：

```cpp
#define ZERO_ENABLE_SHORT_MACROS
#include "ZeroStyle.h"

NODISCARD Zero::TResult<Zero::String> LoadText();
```

如果只需要某一部分，也可以单独包含：

```cpp
#include "Zero/Macros.h"
```

## 构建测试

```bat
build.bat --tests
build.bat --examples
```

或直接使用 CMake：

```bash
cmake -S . -B build -DZERO_BUILD_TESTS=ON -DZERO_BUILD_EXAMPLES=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

也可以使用 CMake presets：

```bash
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

当前 presets 默认提供 Visual Studio 2022 开发配置，也提供 `ninja-gcc`、`ninja-gcc-cxx23`、`ninja-clang`、`ninja-clang-cxx23` 等 Linux/Unix 风格配置。

## 示例

- `examples/basic-console`：展示项目公共头 `ProjectCore.h`、`ZERO_NODISCARD`、`ZERO_NODISCARD_TYPE`、`TResult` 和 alias 的默认接入方式。

## 文档

- [C++ 风格指南](style.md)
- [CMake 接入](docs/cmake-integration.md)
- [错误处理](docs/error-handling.md)
- [Header-only 边界](docs/header-only-policy.md)
- [扩展策略](docs/extension-policy.md)
- [示例说明](docs/examples.md)
