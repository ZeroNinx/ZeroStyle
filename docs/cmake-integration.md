# CMake Integration

ZeroStyle 提供 header-only `INTERFACE` target：

```cmake
add_subdirectory(path/to/ZeroStyle)

target_link_libraries(MyTarget
    PRIVATE
        Zero::ZeroStyle
)
```

链接 `Zero::ZeroStyle` 不会产生实际库文件，但会传递 include path 和 C++ 标准要求。

ZeroStyle 不设置全局 `CMAKE_CXX_STANDARD`。默认只通过 `Zero::ZeroStyle` 的 `target_compile_features` 表达 C++20/C++23 要求，避免影响宿主项目的其他 target。

## 项目公共头

推荐项目建立自己的公共基础头，选择性转发 ZeroStyle 类型：

```cpp
#pragma once

#include "ZeroStyle.h"

namespace MyProject {

using Zero::StdPath;
using Zero::StdString;
using Zero::TResult;
using Zero::TVector;
using Zero::int32;

}  // namespace MyProject
```

公共头文件不要写 `using namespace Zero;`。

公共头文件也不建议默认启用短名字宏，除非项目明确把短宏作为公共风格契约。

`.cpp` 文件可以更自由地使用短宏：

```cpp
#define ZERO_ENABLE_SHORT_MACROS
#include "ProjectCore.h"

using namespace Zero;

NODISCARD TVector<StdString> BuildNames()
{
    return {};
}
```

## FetchContent

如果项目不想把 ZeroStyle 固定为子目录，也可以用 CMake `FetchContent`：

```cmake
include(FetchContent)

FetchContent_Declare(
    ZeroStyle
    GIT_REPOSITORY <your-zero-style-repository-url>
    GIT_TAG        main
)

FetchContent_MakeAvailable(ZeroStyle)

target_link_libraries(MyTarget
    PRIVATE
        Zero::ZeroStyle
)
```

个人项目中也可以把 `GIT_REPOSITORY` 换成本地路径或固定 tag。正式项目建议固定 commit 或版本 tag，不建议长期跟随浮动分支。

## Presets

常用开发配置：

```bash
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

C++23 配置：

```bash
cmake --preset dev-cxx23
cmake --build --preset dev-cxx23
ctest --preset dev-cxx23
```

Clang 线程安全分析默认关闭。需要时可显式启用：

```cmake
set(ZERO_ENABLE_THREAD_SAFETY_ANALYSIS ON)
add_subdirectory(path/to/ZeroStyle)
```

## Install Package

安装：

```bash
cmake --install build/dev --prefix install
```

消费已安装包：

```cmake
find_package(ZeroStyle CONFIG REQUIRED)

target_link_libraries(MyTarget
    PRIVATE
        Zero::ZeroStyle
)
```
