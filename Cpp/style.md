# C++ 代码风格指南

本文档定义本项目使用的 C++ 代码风格。它不是对某个既有规范的完整复刻，而是一套面向普通现代 C++ 工程的混合风格：

- 命名与视觉风格参考 Unreal Engine C++ 的通用部分。
- 工程纪律参考 Google / Chromium 风格。
- API 可读性参考 Qt。
- 错误处理参考 Rust 的 `Result<T, E>` 思路。
- 类型系统、资源管理和工具链习惯遵循现代 C++。

本规范的核心目标是：**代码一眼能读懂，接口一眼能用对，错误一眼能处理，工程一眼能维护。**

---

## 目录

1. [总体原则](#总体原则)
2. [语言标准与工具链](#语言标准与工具链)
3. [文件组织](#文件组织)
4. [格式化规则](#格式化规则)
5. [命名规则](#命名规则)
6. [类型设计](#类型设计)
7. [函数与 API 设计](#函数与-api-设计)
8. [变量与成员](#变量与成员)
9. [错误处理](#错误处理)
10. [资源管理与所有权](#资源管理与所有权)
11. [宏、模板与编译期代码](#宏模板与编译期代码)
12. [注释与文档](#注释与文档)
13. [测试代码风格](#测试代码风格)
14. [完整示例](#完整示例)

---

## 总体原则

### 保持一致性

一致性优先于个人偏好。局部代码应当服从所在模块的既有风格；新模块应当遵守本文档。

好的做法：

```cpp
class TextureLoader final
{
public:
    bool LoadFromFile(const Path& InPath);

private:
    Path SourcePath;
    bool bLoaded = false;
};
```

不好的做法：

```cpp
class texture_loader {
public:
    bool loadFromFile(const Path& path);

private:
    Path source_path_;
    bool m_bLoaded = false;
};
```

同一项目中混用 `snake_case`、`camelCase`、`PascalCase` 会降低可读性。除非是与第三方库、标准库或平台 API 对接，否则项目自有代码应统一使用本规范。

---

### 可读性优先于炫技

不要为了展示语言技巧而写复杂代码。优先选择容易调试、容易审查、容易维护的写法。

好的做法：

```cpp
if (!Database.IsOpen())
{
    return Result<Path>::Err(MakeError(ErrorCode::NotOpen, "database is not open"));
}
```

不好的做法：

```cpp
return Database.IsOpen() ? ResolveImpl(Name) : Result<Path>::Err(MakeError(ErrorCode::NotOpen, ""));
```

三目表达式、模板元编程、宏技巧和高度压缩的链式调用都应谨慎使用。

---

### 显式优先于隐式

重要行为应当显式表达，包括所有权、生命周期、错误、输入输出方向和状态变化。

好的做法：

```cpp
NODISCARD Result<Path> ResolveAssetPath(StringView InName) const;
```

不好的做法：

```cpp
Path Get(StringView Name) const;
```

`ResolveAssetPath` 比 `Get` 更清楚，`Result<T>` 比隐藏失败条件更清楚，`NODISCARD` 能提醒调用者处理结果。

---

## 语言标准与工具链

### C++ 标准

项目默认使用 **C++20** 或更新标准。优先使用标准库能力，不为风格一致而重新封装标准库。

好的做法：

```cpp
Vector<Texture> Textures;
Path ConfigPath;
StringView Name;
Optional<AssetRecord> Record;
```

不好的做法：

```cpp
// 自造行为不透明的容器，或命名与标准库语义冲突
MyArray<Texture> Textures;   // 自定义容器，与 std::vector 行为不同
Maybe<AssetRecord> Record;   // 非标准命名，混淆使用方
```

不要发明与标准库语义不同的替代品；项目通过 `ZeroStyle.h` 的 PascalCase 别名使用标准类型。

---

### 编译警告

项目代码应在高警告等级下保持干净。

建议开启：

```text
-Wall
-Wextra
-Wconversion
-Wshadow
-Wnon-virtual-dtor
-Wold-style-cast
-Werror 可在 CI 或核心模块中启用
```

Windows / MSVC 工程建议至少启用 `/W4`，核心模块可以启用 `/WX`。

---

### 自动格式化

所有代码必须由 `clang-format` 或等效工具格式化。不要手动维护对齐风格。

推荐基础配置：

```yaml
BasedOnStyle: Chromium
IndentWidth: 4
ColumnLimit: 100
BreakBeforeBraces: Allman
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
PointerAlignment: Left
AccessModifierOffset: -4
NamespaceIndentation: None
SortIncludes: true
```

---

## 文件组织

### 文件名

文件名使用 `PascalCase`，与主要类型名一致。

好的做法：

```text
AssetDatabase.h
AssetDatabase.cpp
TextureLoader.h
TextureLoader.cpp
IFileSystem.h
```

不好的做法：

```text
asset_database.h
texture_loader.cpp
filesystem_interface.h
```

例外：第三方库、构建脚本、平台约定文件可以使用其生态惯例。

---

### 头文件保护

优先使用 `#pragma once`。

好的做法：

```cpp
#pragma once

#include <filesystem>
#include <string>
```

不好的做法：

```cpp
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H
// ...
#endif
```

传统 include guard 不是禁止，但新代码默认使用 `#pragma once`。

---

### include 顺序

include 顺序如下：

1. 当前 `.cpp` 对应的头文件。
2. C 标准库头文件。
3. C++ 标准库头文件。
4. 第三方库头文件。
5. 项目内部头文件。

好的做法：

```cpp
#include "AssetDatabase.h"

#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "Core/Result.h"
#include "IO/IFileSystem.h"
```

不好的做法：

```cpp
#include "Core/Result.h"
#include <vector>
#include "AssetDatabase.h"
#include <filesystem>
#include <fmt/format.h>
```

---

### 前向声明

能用前向声明解决的地方，优先使用前向声明，减少头文件依赖。

好的做法：

```cpp
class IFileSystem;

class AssetDatabase final
{
public:
    explicit AssetDatabase(const IFileSystem& InFileSystem);

private:
    const IFileSystem& FileSystem;
};
```

不好的做法：

```cpp
#include "NativeFileSystem.h"
#include "MemoryFileSystem.h"

class AssetDatabase
{
    NativeFileSystem FileSystem;
};
```

头文件应暴露最小依赖。

---

## 格式化规则

### 缩进

使用 4 个空格缩进，不使用 Tab。

好的做法：

```cpp
if (bReady)
{
    Start();
}
```

不好的做法：

```cpp
if (bReady)
{
  Start();
}
```

---

### 花括号

使用 Allman 风格，花括号独占一行。

好的做法：

```cpp
void AssetDatabase::Close()
{
    bOpen = false;
}
```

不好的做法：

```cpp
void AssetDatabase::Close() {
    bOpen = false;
}
```

该风格牺牲少量垂直空间，换取块结构清晰。

---

### 行宽

推荐行宽不超过 100 列。复杂函数签名应换行。

好的做法：

```cpp
NODISCARD Result<Path> ResolveAssetPath(
    StringView InName,
    ResolveFlags InFlags) const;
```

不好的做法：

```cpp
NODISCARD Result<Path> ResolveAssetPath(StringView InName, ResolveFlags InFlags) const;
```

---

### 指针与引用

`*` 和 `&` 靠近类型。

好的做法：

```cpp
Texture* FindTexture(StringView InName);
const Texture& GetDefaultTexture() const;
```

不好的做法：

```cpp
Texture *FindTexture(StringView InName);
const Texture &GetDefaultTexture() const;
```

---

### 空行

用空行分隔逻辑块，不要过度压缩。

好的做法：

```cpp
auto Record = Manifest.FindAsset(InName);

if (!Record.has_value())
{
    return Result<Path>::Err(MakeError(ErrorCode::AssetNotFound, "asset not found"));
}

return Result<Path>::Ok(BasePath / Record->RelativePath);
```

不好的做法：

```cpp
auto Record = Manifest.FindAsset(InName);
if (!Record.has_value()) { return Result<Path>::Err(MakeError(ErrorCode::AssetNotFound, "asset not found")); }
return Result<Path>::Ok(BasePath / Record->RelativePath);
```

---

## 命名规则

本项目使用 **PascalCase 作为主视觉风格**。这能明显区分项目自有 API 与标准库 API。

### 类型名

类、结构体、枚举、类型别名使用 `PascalCase`。

好的做法：

```cpp
class AssetDatabase;
struct AssetRecord;
enum class TextureFormat;
using AssetName = String;
```

不好的做法：

```cpp
class asset_database;
struct asset_record;
enum class textureFormat;
using asset_name = String;
```

---

### 函数名

函数名使用 `PascalCase`，通常使用动词或动词短语。

好的做法：

```cpp
Open();
Close();
Reload();
FindAsset();
ContainsAsset();
ResolveAssetPath();
```

不好的做法：

```cpp
open();
get();
process();
handle();
doWork();
```

函数名应表达行为，不应依赖调用者猜测。

---

### 变量名

变量名使用 `PascalCase`。

好的做法：

```cpp
Path ConfigPath;
AssetRecord CurrentRecord;
int RetryCount = 0;
```

不好的做法：

```cpp
Path config_path;
AssetRecord currentRecord;
int retry_count = 0;
```

变量名应避免与类型名完全相同。

好的做法：

```cpp
Texture MainTexture;
Window MainWindow;
Config RuntimeConfig;
```

不好的做法：

```cpp
Texture Texture;
Window Window;
Config Config;
```

---

### 布尔变量

布尔变量使用 `b` 前缀加 `PascalCase`。

好的做法：

```cpp
bool bOpen = false;
bool bLoaded = false;
bool bUseVSync = true;
bool bHasFocus = false;
```

不好的做法：

```cpp
bool open = false;
bool isLoaded = false;
bool use_vsync = true;
bool m_bHasFocus = false;
```

`b` 前缀让条件表达式更容易阅读。

好的做法：

```cpp
if (bOpen && bHasFocus && !bPendingClose)
{
    Update();
}
```

不好的做法：

```cpp
if (open && focus && !pending)
{
    Update();
}
```

---

### 接口名

纯接口类使用 `I` 前缀。

好的做法：

```cpp
class IFileSystem
{
public:
    virtual ~IFileSystem() = default;

    NODISCARD virtual Result<String> ReadTextFile(
        const Path& InPath) const = 0;
};
```

不好的做法：

```cpp
class FileSystemInterface
{
public:
    virtual String read_file(const Path& path) = 0;
};
```

`I` 前缀只用于抽象接口，不用于普通基类。

---

### 常量

常量使用 `PascalCase`。全局常量优先放入命名空间并使用 `inline constexpr`。

好的做法：

```cpp
inline constexpr int MaxAssetNameLength = 128;
inline constexpr StringView DefaultManifestName = "Assets.manifest";
```

可以接受但不推荐混用：

```cpp
inline constexpr int kMaxAssetNameLength = 128;
```

不好的做法：

```cpp
#define MAX_ASSET_NAME_LENGTH 128
const int max_asset_name_length = 128;
```

`ALL_CAPS` 仅用于宏。

---

### 枚举

枚举类型和枚举值均使用 `PascalCase`，不使用 `E` 前缀。

好的做法：

```cpp
enum class TextureFormat
{
    Unknown,
    Rgba8,
    Bgra8,
    Rgba16F,
};
```

不好的做法：

```cpp
enum class ETextureFormat
{
    TF_Unknown,
    TF_RGBA8,
};
```

普通工程不需要 UE 的 `E` 前缀。

---

### 模板参数

模板类型参数可以使用 `T` 前缀，但模板类本身不使用 `T` 前缀。

好的做法：

```cpp
template <typename TValue>
class Result final
{
public:
    NODISCARD static Result Ok(TValue InValue);
};
```

不好的做法：

```cpp
template <typename T>
class TResult
{
};
```

`TValue` 比单独的 `T` 更有语义。`TResult` 会让普通 C++ 工程看起来像 UE 基础库。

---

### 命名空间

命名空间使用 `PascalCase` 或简短项目名。大型项目建议使用根命名空间。

好的做法：

```cpp
namespace Aurora {

class AssetDatabase final
{
};

}  // namespace Aurora
```

可以接受：

```cpp
namespace aurora {
}
```

不好的做法：

```cpp
using namespace std;
```

头文件中禁止使用 `using namespace`。

---

### using 声明（.cpp）

头文件禁止在命名空间或全局作用域使用 `using namespace` 和 `using X::Y`。`.cpp` 文件中的策略如下：

好的做法：

```cpp
// .cpp 文件顶部：具名类型别名，减少重复书写
using ErrorMap = HashMap<String, Error>;

// 函数内直接使用别名，无需额外 using
void ProcessFiles(const Vector<Path>& InPaths)
{
    for (const Path& Entry : InPaths)
    {
        // ...
    }
}
```

可以接受（仅 .cpp，具名 using）：

```cpp
using Zero::String;
using Zero::Vector;
```

不好的做法（即使在 .cpp 中）：

```cpp
using namespace std;   // 大范围引入，掩盖名称来源
using namespace fmt;
```

原则：`using` 引入的名称范围越小越好。函数内 `using` 优先于文件级 `using`；具名 `using X::Y` 优先于 `using namespace X`。

---

## 类型设计

### 类与结构体

当类型主要表示数据集合且成员可以公开访问时，使用 `struct`。当类型维护不变量或提供行为时，使用 `class`。

好的做法：

```cpp
struct AssetRecord final
{
    String Name;
    Path RelativePath;
    bool bPreload = false;
};
```

好的做法：

```cpp
class AssetDatabase final
{
public:
    NODISCARD VoidResult Open(Path InManifestPath);
    NODISCARD bool IsOpen() const noexcept;

private:
    AssetManifest Manifest;
    bool bOpen = false;
};
```

不好的做法：

```cpp
class AssetRecord
{
public:
    String Name;
    Path RelativePath;
};
```

如果只是数据，`struct` 更直接。

---

### final

默认将非继承设计的类标记为 `final`。

好的做法：

```cpp
class NativeFileSystem final : public IFileSystem
{
public:
    Result<String> ReadTextFile(const Path& InPath) const override;
};
```

不好的做法：

```cpp
class NativeFileSystem : public IFileSystem
{
};
```

除非明确需要继承扩展，否则使用 `final` 能减少误用。

---

### 构造函数

单参数构造函数默认使用 `explicit`。

好的做法：

```cpp
class AssetDatabase final
{
public:
    explicit AssetDatabase(const IFileSystem& InFileSystem);
};
```

不好的做法：

```cpp
class AssetDatabase
{
public:
    AssetDatabase(const IFileSystem& InFileSystem);
};
```

隐式构造容易造成难以察觉的类型转换。

---

### 拷贝与移动

拥有资源、绑定外部依赖或维护复杂状态的类，应显式声明拷贝和移动策略。

好的做法：

```cpp
class AssetDatabase final
{
public:
    AssetDatabase(const AssetDatabase&) = delete;
    AssetDatabase& operator=(const AssetDatabase&) = delete;

    AssetDatabase(AssetDatabase&&) = delete;
    AssetDatabase& operator=(AssetDatabase&&) = delete;
};
```

不好的做法：

```cpp
class AssetDatabase
{
    const IFileSystem& FileSystem;
    AssetManifest Manifest;
};
```

让编译器隐式生成拷贝操作，可能复制出语义不清晰的对象。

---

### 访问控制顺序

类成员顺序如下：

1. `public` 类型别名、构造函数、析构函数。
2. `public` API。
3. `protected` 成员。
4. `private` 辅助函数。
5. `private` 数据成员。

好的做法：

```cpp
class TextureLoader final
{
public:
    explicit TextureLoader(IFileSystem& InFileSystem);

    NODISCARD Result<Texture> LoadFromFile(const Path& InPath);
    NODISCARD bool IsLoaded() const noexcept;

private:
    NODISCARD static Result<TextureInfo> ParseHeader(std::span<const std::byte> InData);

    IFileSystem& FileSystem;
    TextureInfo Info;
    bool bLoaded = false;
};
```

---

## 函数与 API 设计

### API 应该像自然语言

API 名称应当能从调用点读出含义。

好的做法：

```cpp
Database.Open("Game/Assets.manifest");
Database.Reload();
Database.ContainsAsset("PlayerIcon");
Database.ResolveAssetPath("PlayerIcon");
```

不好的做法：

```cpp
Database.Init("Game/Assets.manifest");
Database.DoReload();
Database.Check("PlayerIcon");
Database.Get("PlayerIcon");
```

`Get`、`Set`、`Process`、`Handle`、`Execute` 等泛化名称应谨慎使用。

---

### 输出参数推荐 Out 前缀

当必须使用输出参数时，推荐使用 `Out` 前缀，便于在调用点区分输入与输出。

推荐：

```cpp
NODISCARD bool ParseColor(StringView Text, Color& OutColor);
```

避免：

```cpp
bool ParseColor(StringView Text, Color& Color);  // 参数名与类型名冲突
```

更好的做法是直接返回 `Result<T>`，彻底消除输出参数：

```cpp
NODISCARD Result<Color> ParseColor(StringView Text);
```

---

### 避免布尔参数陷阱

布尔参数在调用点含义不清晰时，应改用枚举。

不好的做法：

```cpp
Window.Open(true, false);
```

好的做法：

```cpp
enum class OpenMode
{
    ReadOnly,
    ReadWrite,
};

enum class CreatePolicy
{
    MustExist,
    CreateIfMissing,
};

Window.Open(OpenMode::ReadWrite, CreatePolicy::CreateIfMissing);
```

如果布尔参数确实清楚，可以保留。

可以接受：

```cpp
Widget.SetVisible(true);
AudioDevice.SetMuted(false);
```

---

### 使用 NODISCARD

返回错误、状态、资源句柄、查找结果的函数必须使用 `NODISCARD`。

好的做法：

```cpp
NODISCARD Result<Texture> LoadTexture(const Path& InPath);
NODISCARD bool ContainsAsset(StringView InName) const;
NODISCARD Optional<AssetRecord> FindAsset(StringView InName) const;
```

不好的做法：

```cpp
Result<Texture> LoadTexture(const Path& InPath);
bool ContainsAsset(StringView InName) const;
```

调用方忽略这些返回值通常是 bug。

---

### const 和 noexcept

不会修改对象状态的成员函数应标记 `const`。保证不抛异常的轻量函数可以标记 `noexcept`。

好的做法：

```cpp
NODISCARD bool IsOpen() const noexcept;
NODISCARD const Path& ManifestPath() const noexcept;
```

不好的做法：

```cpp
bool IsOpen();
Path ManifestPath();
```

---

### 函数长度

函数应保持短小。超过约 50 行的函数应考虑拆分。超过约 100 行的函数通常需要重构。

好的做法：

```cpp
VoidResult AssetDatabase::Open(Path InManifestPath)
{
    auto TextResult = FileSystem.ReadTextFile(InManifestPath);

    if (TextResult.IsErr())
    {
        return VoidResult::Err(TextResult.Failure());
    }

    auto ManifestResult = ParseManifest(TextResult.Value(), InManifestPath);

    if (ManifestResult.IsErr())
    {
        return VoidResult::Err(ManifestResult.Failure());
    }

    ApplyManifest(std::move(InManifestPath), std::move(ManifestResult).TakeValue());
    return VoidResult::Ok(Unit{});
}
```

不好的做法：

```cpp
void AssetDatabase::OpenAndParseAndValidateAndLoadAndNotify(...)
{
    // 数百行混合文件 IO、解析、校验、状态更新、事件通知。
}
```

---

## 变量与成员

### 成员变量

成员变量使用 `PascalCase`，不使用 `m_` 或尾随下划线。

好的做法：

```cpp
class Window final
{
private:
    int Width = 0;
    int Height = 0;
    bool bVisible = false;
};
```

不好的做法：

```cpp
class Window
{
private:
    int m_Width = 0;
    int height_ = 0;
    bool m_bVisible = false;
};
```

参数使用 `In` 前缀后，不需要通过 `m_` 或 `_` 区分成员。

---

### 局部变量

局部变量也使用 `PascalCase`，并尽量靠近首次使用处声明。

好的做法：

```cpp
const auto Record = Manifest.FindAsset(InName);

if (!Record.has_value())
{
    return Result<Path>::Err(MakeError(ErrorCode::AssetNotFound, "asset not found"));
}
```

不好的做法：

```cpp
Optional<AssetRecord> record;
// 很多无关代码……
record = manifest.find_asset(name);
```

---

### 初始化

优先在声明处初始化变量。成员变量应使用默认成员初始化器。

好的做法：

```cpp
struct TextureInfo final
{
    int Width = 0;
    int Height = 0;
    TextureFormat Format = TextureFormat::Unknown;
    bool bHasMipmaps = false;
};
```

不好的做法：

```cpp
struct TextureInfo
{
    int Width;
    int Height;
    TextureFormat Format;
    bool bHasMipmaps;
};
```

未初始化状态会制造隐性 bug。

---

### auto

`auto` 可用于类型明显或类型冗长的场景。不要用 `auto` 隐藏重要类型。

好的做法：

```cpp
auto Result = Database.ResolveAssetPath("PlayerIcon");
const auto It = RecordsByName.find(Name);
```

不好的做法：

```cpp
auto Data = LoadSomething();
auto X = Factory.Create();
```

当返回类型不明显时，应写出类型或改进函数名。

---

## 错误处理

### 不使用异常作为常规错误处理

项目代码默认不使用异常表达可预期错误。可预期错误通过 `Result<T>` 或 `std::optional<T>` 返回。

好的做法：

```cpp
NODISCARD Result<String> ReadTextFile(const Path& InPath) const;
```

不好的做法：

```cpp
String ReadTextFile(const Path& Path)
{
    if (!Exists(Path))
    {
        throw std::runtime_error("file not found");
    }
}
```

异常可用于不可恢复错误、第三方库边界或程序启动阶段，但不应作为普通控制流。

---

### 使用 Result<T>

可能失败且需要错误信息的函数返回 `Result<T>`。

好的做法：

```cpp
NODISCARD Result<AssetManifest> ParseManifest(
    StringView InText,
    const Path& InManifestPath);
```

不好的做法：

```cpp
AssetManifest ParseManifest(StringView Text);
```

调用方必须处理成功和失败：

```cpp
auto ManifestResult = ParseManifest(Text, Path);

if (ManifestResult.IsErr())
{
    return Result<AssetDatabase>::Err(ManifestResult.Failure());
}

AssetManifest Manifest = std::move(ManifestResult).TakeValue();
```

---

### std::expected（C++23 升级路径）

若项目升级到 C++23，`std::expected<T, E>` 可替代自定义 `Result<T>`，减少维护成本。

`ZeroStyle.h` 在检测到 C++23 时自动将 `Result<T>` 实现为继承自 `std::expected<T, E>` 的适配类，保留 PascalCase API 的同时解锁标准接口。

好的做法（PascalCase API，C++23 下同样有效）：

```cpp
auto PathResult = Database.ResolveAssetPath("PlayerIcon");

if (PathResult.IsErr())
{
    LogError(PathResult.Failure().Message);
    return;
}

auto Path = std::move(PathResult).TakeValue();
```

C++23 原生写法（可与上述写法混用）：

```cpp
if (!PathResult)                           // operator bool
{
    LogError(PathResult.error().Message);  // std::expected 接口
    return;
}

auto Path = *PathResult;                   // operator*
```

迁移时无需修改既有的 `IsOk()` / `Failure()` 调用；两套接口均有效。

---

### 使用 optional 表达“没有”

当失败只有“有或没有”，且不需要错误原因时，使用 `std::optional<T>`。

好的做法：

```cpp
NODISCARD Optional<AssetRecord> FindAsset(StringView InName) const;
```

不好的做法：

```cpp
NODISCARD Result<AssetRecord> FindAsset(StringView InName) const;
```

如果找不到不是错误，只是查询结果为空，`optional` 更合适。

---

### 错误信息

错误类型应包含机器可读的错误码和人类可读的信息。

好的做法：

```cpp
enum class ErrorCode
{
    FileNotFound,
    InvalidManifest,
    DuplicateAsset,
    AssetNotFound,
};

struct Error final
{
    ErrorCode Code;
    String Message;
    Path ContextPath;
};
```

不好的做法：

```cpp
return "failed";
return -1;
```

纯字符串或数字错误码都不够清晰。

---

### 不忽略错误

禁止无理由忽略 `Result<T>`。

好的做法：

```cpp
auto SaveResult = Config.Save();

if (SaveResult.IsErr())
{
    LogError(FormatError(SaveResult.Failure()));
    return SaveResult;
}
```

不好的做法：

```cpp
Config.Save();
```

如果确实要忽略，应显式说明理由。

可以接受：

```cpp
(void)Cache.TryWarmUp();  // Best-effort optimization. Failure is non-fatal.
```

---

## 资源管理与所有权

### 使用 RAII

资源应由对象生命周期管理，不手动配对 `Init` / `Destroy`。

好的做法：

```cpp
class FileHandle final
{
public:
    ~FileHandle();

private:
    NativeHandle Handle = InvalidHandle;
};
```

不好的做法：

```cpp
OpenFile(&Handle);
// 很多代码……
CloseFile(Handle);
```

---

### 使用智能指针表达所有权

独占所有权使用 `std::unique_ptr`，共享所有权使用 `std::shared_ptr`，非拥有引用使用指针或引用。

好的做法：

```cpp
UniquePtr<Texture> CreateTexture(TextureDesc InDesc);

class Renderer final
{
public:
    explicit Renderer(IFileSystem& InFileSystem);

private:
    IFileSystem& FileSystem;  // Non-owning.
};
```

不好的做法：

```cpp
Texture* CreateTexture();
IFileSystem* FileSystem;
```

裸指针可以用于非拥有、可空引用，但必须语义清晰。

---

### 避免全局可变状态

全局可变状态会降低可测试性和可推理性。优先使用依赖注入。

好的做法：

```cpp
class AssetDatabase final
{
public:
    explicit AssetDatabase(const IFileSystem& InFileSystem);

private:
    const IFileSystem& FileSystem;
};
```

不好的做法：

```cpp
extern FileSystem GFileSystem;

void LoadAssets()
{
    GFileSystem.ReadTextFile("Assets.manifest");
}
```

---

## 宏、模板与编译期代码

### 宏

宏应尽量少用。宏名必须使用 `ALL_CAPS`。

好的做法：

```cpp
#define PROJECT_CHECK(Expression) \
    do \
    { \
        if (!(Expression)) \
        { \
            AbortWithMessage(#Expression); \
        } \
    } while (false)
```

不好的做法：

```cpp
#define foreach_asset(asset) for (auto& asset : Assets)
#define CreateProperty(Type, Name) Type Name
```

不要用宏制造伪语言。

---

### constexpr 与 consteval

能在编译期表达的常量和简单函数，优先使用 `constexpr`。

好的做法：

```cpp
constexpr int Kilobytes(int InValue)
{
    return InValue * 1024;
}
```

不好的做法：

```cpp
#define KILOBYTES(Value) ((Value) * 1024)
```

---

### 模板

模板应服务于类型安全和复用，不应过度抽象。

好的做法：

```cpp
template <typename TValue>
class Result final
{
};
```

不好的做法：

```cpp
template <typename TPolicy, typename TAllocator, typename TErrorStrategy, typename TTraits>
class UniversalManager
{
};
```

如果模板参数太多，应考虑拆分职责。

---

### Concepts（C++20）

用 `concept` 约束模板参数，替代未约束的 `typename T`。

好的做法：

```cpp
template <Zero::StringLike TName>
NODISCARD Optional<AssetRecord> FindAsset(TName&& InName) const;
```

不好的做法：

```cpp
// 调用方无法从签名得知 TName 必须能转为 StringView
template <typename TName>
NODISCARD Optional<AssetRecord> FindAsset(TName&& InName) const;
```

使用 `concept` 的时机：

- 模板参数有明确语义约束时（如“必须是字符串类型”）。
- 提供重载时用于消歧义。
- 不要为单一实例化的函数强行引入 concept，直接写具体类型更清晰。

`ZeroStyle.h` 提供以下常用 concept：

| Concept        | 含义                                  |
|----------------|---------------------------------------|
| `StringLike`   | 可转换为 `std::string_view`           |
| `PathLike`     | 可转换为 `std::filesystem::path`      |
| `Hashable`     | 可用作无序容器键                      |
| `ErrorType`    | 拥有 `Code` 与 `Message` 成员         |

---

## 注释与文档

### 注释解释原因，不重复代码

好的做法：

```cpp
// Keep the old manifest active until parsing succeeds so Reload() is atomic.
AssetManifest NewManifest = std::move(ManifestResult).TakeValue();
```

不好的做法：

```cpp
// Set bOpen to true.
bOpen = true;
```

注释应解释“为什么”，而不是复述“做了什么”。

---

### 公共 API 文档

复杂公共 API 应说明失败条件、所有权和线程安全。

好的做法：

```cpp
// Opens and parses an asset manifest.
//
// Returns InvalidManifest if the manifest syntax is invalid.
// Returns FileNotFound if the file system cannot read InManifestPath.
// On failure, the previous open manifest remains unchanged.
NODISCARD VoidResult Open(Path InManifestPath);
```

不好的做法：

```cpp
// Open manifest.
VoidResult Open(Path InManifestPath);
```

---

### 线程安全

类声明处应注明线程安全性；函数签名可进一步细化。

好的做法：

```cpp
// Thread-safe.
class EventBus final
{
public:
    void Publish(Event InEvent)            EXCLUDES(Mutex);
    void Subscribe(EventHandler InHandler) EXCLUDES(Mutex);

private:
    mutable std::mutex         Mutex;
    Vector<EventHandler>  Handlers GUARDED_BY(Mutex);
};
```

好的做法：

```cpp
// Not thread-safe. External synchronization required.
class AssetDatabase final
{
public:
    NODISCARD VoidResult Open(Path InManifestPath);
};
```

类本身不保证线程安全，但个别方法安全时，在方法上单独注明：

```cpp
// Not thread-safe. External synchronization required.
class RenderQueue final
{
public:
    // Thread-safe.
    void Enqueue(RenderCommand InCommand);

    // Not thread-safe. Must be called from render thread only.
    void Flush();
};
```

`GUARDED_BY` / `EXCLUDES` / `REQUIRES` 宏由 `ZeroStyle.h` 提供，Clang 下启用 `-Wthread-safety` 可获得静态分析支持，其余编译器下宏展开为空。

---

## 测试代码风格

测试代码可以比生产代码更直接，但仍应遵守命名、错误处理和格式化规则。

好的做法：

```cpp
TEST(AssetDatabaseTest, OpenReturnsInvalidManifestForMalformedLine)
{
    MemoryFileSystem FileSystem;
    FileSystem.AddTextFile("Assets.manifest", "BrokenLine");

    AssetDatabase Database(FileSystem);
    auto Result = Database.Open("Assets.manifest");

    ASSERT_TRUE(Result.IsErr());
    EXPECT_EQ(Result.Failure().Code, ErrorCode::InvalidManifest);
}
```

不好的做法：

```cpp
TEST(asset_database, bad)
{
    auto r = db.open("x");
    EXPECT_FALSE(r.ok());
}
```

测试名应描述行为和预期。

---

## 完整示例

下面的示例展示本风格的核心约定：

- `PascalCase` 命名。
- `b` 布尔前缀。
- `I` 接口前缀。
- `In` / `Out` 参数方向。
- `Result<T>` 显式错误处理。
- `NODISCARD` 防止忽略错误。
- 依赖注入和小接口。
- Allman 花括号。

```cpp
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Project {

struct Unit final
{
};

enum class ErrorCode
{
    Unknown,
    FileNotFound,
    InvalidManifest,
    DuplicateAsset,
    AssetNotFound,
    NotOpen,
};

struct Error final
{
    ErrorCode Code = ErrorCode::Unknown;
    std::string Message;
    std::filesystem::path Path;
};

template <typename TValue>
class Result final
{
public:
    NODISCARD static Result Ok(TValue InValue)
    {
        return Result(std::move(InValue));
    }

    NODISCARD static Result Err(Error InError)
    {
        return Result(std::move(InError));
    }

    NODISCARD bool IsOk() const noexcept
    {
        return std::holds_alternative<TValue>(Storage);
    }

    NODISCARD bool IsErr() const noexcept
    {
        return !IsOk();
    }

    NODISCARD const TValue& Value() const&
    {
        return std::get<TValue>(Storage);
    }

    NODISCARD TValue&& TakeValue() &&
    {
        return std::move(std::get<TValue>(Storage));
    }

    NODISCARD const Error& Failure() const&
    {
        return std::get<Error>(Storage);
    }

private:
    explicit Result(TValue InValue)
        : Storage(std::move(InValue))
    {
    }

    explicit Result(Error InError)
        : Storage(std::move(InError))
    {
    }

    std::variant<TValue, Error> Storage;
};

using VoidResult = Result<Unit>;

class IFileSystem
{
public:
    virtual ~IFileSystem() = default;

    NODISCARD virtual Result<std::string> ReadTextFile(
        const std::filesystem::path& InPath) const = 0;
};

struct AssetRecord final
{
    std::string Name;
    std::filesystem::path RelativePath;
    bool bPreload = false;
};

class AssetManifest final
{
public:
    NODISCARD bool ContainsAsset(std::string_view InName) const
    {
        return RecordsByName.contains(std::string(InName));
    }

    NODISCARD std::optional<AssetRecord> FindAsset(std::string_view InName) const
    {
        const auto It = RecordsByName.find(std::string(InName));

        if (It == RecordsByName.end())
        {
            return std::nullopt;
        }

        return It->second;
    }

    NODISCARD VoidResult AddRecord(AssetRecord InRecord)
    {
        if (InRecord.Name.empty())
        {
            return VoidResult::Err(Error{
                .Code = ErrorCode::InvalidManifest,
                .Message = "asset name must not be empty",
            });
        }

        const std::string Name = InRecord.Name;
        const auto [It, bInserted] = RecordsByName.emplace(Name, std::move(InRecord));
        (void)It;

        if (!bInserted)
        {
            return VoidResult::Err(Error{
                .Code = ErrorCode::DuplicateAsset,
                .Message = "duplicate asset name: " + Name,
            });
        }

        return VoidResult::Ok(Unit{});
    }

private:
    std::unordered_map<std::string, AssetRecord> RecordsByName;
};

class AssetDatabase final
{
public:
    explicit AssetDatabase(const IFileSystem& InFileSystem)
        : FileSystem(InFileSystem)
    {
    }

    AssetDatabase(const AssetDatabase&) = delete;
    AssetDatabase& operator=(const AssetDatabase&) = delete;

    AssetDatabase(AssetDatabase&&) = delete;
    AssetDatabase& operator=(AssetDatabase&&) = delete;

    NODISCARD bool IsOpen() const noexcept
    {
        return bOpen;
    }

    NODISCARD VoidResult Open(std::filesystem::path InManifestPath);

    NODISCARD Result<std::filesystem::path> ResolveAssetPath(
        std::string_view InName) const
    {
        if (!bOpen)
        {
            return Result<std::filesystem::path>::Err(Error{
                .Code = ErrorCode::NotOpen,
                .Message = "database is not open",
            });
        }

        const auto Record = Manifest.FindAsset(InName);

        if (!Record.has_value())
        {
            return Result<std::filesystem::path>::Err(Error{
                .Code = ErrorCode::AssetNotFound,
                .Message = "asset not found: " + std::string(InName),
                .ContextPath = CurrentManifestPath,
            });
        }

        return Result<std::filesystem::path>::Ok(
            CurrentManifestPath.parent_path() / Record->RelativePath);
    }

private:
    const IFileSystem& FileSystem;
    std::filesystem::path CurrentManifestPath;
    AssetManifest Manifest;
    bool bOpen = false;
};

}  // namespace Project
```

---

## 总结

本风格可以概括为：

```text
PascalCase Modern C++
UE-inspired semantic hints
Google/Chromium-like engineering discipline
Qt-like API readability
Rust-like explicit error handling
```

最重要的不是某个单独规则，而是这些规则合在一起形成的工程效果：

- 类型、函数、变量在视觉上清晰。
- 布尔值、接口、输入输出方向有明确提示。
- API 从调用点就能读懂。
- 错误不会被隐藏或静默忽略。
- 依赖关系清晰，模块容易测试。
- 与标准库保持自然边界，不重新发明基础设施。
