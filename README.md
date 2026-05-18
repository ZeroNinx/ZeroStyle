# ZeroStyle

ZeroStyle 是一套面向个人项目的轻量 C++ 风格层与 header-only 工具库。

它的目标不是重新发明标准库，也不是构建一套底层运行时框架，而是在保持标准库兼容的前提下，为 C++ 项目提供统一的命名规范、常用 alias、轻量工具类型和工程约定。

## 设计目标

- 标准库优先，不做无语义封装。
- Header-only 优先，方便复制、接入和审查。
- 低侵入，不强迫项目接受特定运行时模型。
- 风格统一，通过 `ZeroStyle.h` 和 `Zero/` 下的独立头文件提供基础能力。
- 可扩展，但新增能力必须保持边界清晰。

## 当前内容

- `Zero/Types.h`：基础整数、字符串、路径、容器、智能指针 alias。
- `Zero/Macros.h`：`NODISCARD`、Clang 线程安全注解等宏。
- `Zero/Config.h`：C++ 版本检测、`std::expected` 检测、基础断言入口。
- `Zero/Error.h`：`SUnit`、`EErrorCode`、`SError`、`MakeError`。
- `Zero/Result.h`：`TResult<T>`、`TVoidResult`，C++23 可用时适配 `std::expected`。
- `Zero/Concepts.h`：常用 C++20 concepts。
- `ZeroStyle.h`：聚合入口头文件。

## CMake 接入

作为子目录接入：

```cmake
add_subdirectory(path/to/ZeroStyle)

target_link_libraries(MyTarget
    PRIVATE
        Zero::ZeroStyle
)
```

使用：

```cpp
#include "ZeroStyle.h"

using namespace Zero;

NODISCARD TResult<String> LoadText(const Path& FilePath);
```

如果只需要某一部分，也可以单独包含：

```cpp
#include "Zero/Macros.h"
```

## 构建测试

```bat
build.bat --tests
```

或直接使用 CMake：

```bash
cmake -S . -B build -DZERO_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## 文档

- [C++ 风格指南](style.md)
