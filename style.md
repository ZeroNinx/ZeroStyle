# C++ 代码风格指南：Zero 前缀版

本文档定义本项目使用的 C++ 代码风格。它是一套面向个人项目的现代 C++ 风格：命名与视觉风格参考 Unreal Engine，但在类型前缀上做更细的区分；工程纪律参考 Google / Chromium；API 可读性参考 Qt；错误处理参考 Rust 的 `Result` 思路。

本规范的核心目标是：**代码一眼能读懂，接口一眼能用对，错误一眼能处理，工程一眼能维护。**

---

## 目录

1. [总体原则](#总体原则)
2. [前缀体系](#前缀体系)
3. [语言标准与标准库边界](#语言标准与标准库边界)
4. [文件组织](#文件组织)
5. [格式化规则](#格式化规则)
6. [命名规则](#命名规则)
7. [类型设计](#类型设计)
8. [函数与 API 设计](#函数与-api-设计)
9. [变量与成员](#变量与成员)
10. [错误处理](#错误处理)
11. [资源管理与所有权](#资源管理与所有权)
12. [宏、模板与编译期代码](#宏模板与编译期代码)
13. [注释与文档](#注释与文档)
14. [测试代码风格](#测试代码风格)
15. [完整示例](#完整示例)

---

## 总体原则

### 保持一致性

项目自有代码统一使用 PascalCase。类型、函数、变量、成员变量都采用同一主视觉风格，再通过前缀区分语义类别。

好的做法：

```cpp
class ZTextureLoader
{
public:
    TResult<ZTexture> LoadFromFile(const Path& FilePath);

private:
    Path SourcePath;
    bool bLoaded = false;
};
```

不好的做法：

```cpp
class texture_loader {
public:
    result<texture> load_from_file(const std::filesystem::path& path);

private:
    std::filesystem::path source_path_;
    bool loaded_ = false;
};
```

同一项目中混用 `snake_case`、`camelCase`、尾随下划线、`m_` 等风格会降低可读性。除非与第三方库、标准库或平台 API 对接，否则项目自有代码应遵守本文档。

---

### 可读性优先于炫技

不要为了展示语言技巧而写复杂代码。优先选择容易调试、容易审查、容易维护的写法。

好的做法：

```cpp
if (!Database.IsOpen())
{
    return TResult<Path>::Err(MakeError(EErrorCode::NotOpen, "database is not open"));
}
```

不好的做法：

```cpp
return Database.IsOpen() ? ResolveImpl(Name) : TResult<Path>::Err(MakeError(EErrorCode::NotOpen, ""));
```

三目表达式、模板元编程、宏技巧和高度压缩的链式调用都应谨慎使用。

---

### 显式优先于隐式

重要行为应显式表达，包括所有权、生命周期、错误、输出方向和状态变化。

好的做法：

```cpp
NODISCARD TResult<Path> ResolveAssetPath(StringView Name) const;
```

不好的做法：

```cpp
Path Get(StringView Name) const;
```

`ResolveAssetPath` 比 `Get` 更清楚，`TResult<T>` 比隐藏失败条件更清楚，`NODISCARD` 能提醒调用者处理结果。

---

## 前缀体系

本风格使用明确的前缀区分类型类别。前缀不是装饰，而是类型语义的一部分。

| 前缀 | 用途 | 示例 |
|---|---|---|
| `Z` | 普通 `class`，通常拥有行为、不变量、生命周期或资源 | `ZAssetDatabase`、`ZWindow` |
| `S` | `struct`，主要表示数据、配置、结果、消息、描述符 | `SAssetRecord`、`SWindowDesc` |
| `T` | 模板类、模板别名、泛型工具 | `TResult`、`TVector`、`TOptional` |
| `I` | 纯接口 | `IFileSystem`、`IAssetProvider` |
| `E` | `enum class` | `EErrorCode`、`ETextureFormat` |
| `C` | `concept` | `CStringLike`、`CHashable` |
| `b` | 布尔变量 | `bOpen`、`bLoaded` |
| `Out` | 输出参数推荐前缀 | `OutColor`、`OutRecord` |

输入参数默认使用普通 `PascalCase` 语义名，不强制使用 `In` 前缀。必须使用非返回值输出参数时，推荐使用 `Out` 前缀。遇到输入输出参数时，优先改成返回值、结果对象或成员函数。

好的做法：

```cpp
class ZAddress;
struct SAddressRecord;
template <typename TValue>
class TResult;
class IAddressResolver;
enum class EAddressKind;
template <typename TValue>
concept CStringLike = std::convertible_to<TValue, StringView>;
```

使用时：

```cpp
ZAddress Address;
SAddressRecord AddressRecord;
TResult<ZAddress> AddressResult;
IAddressResolver* Resolver = nullptr;
EAddressKind Kind = EAddressKind::Home;
```

这种写法允许变量名和类型核心名接近，但仍能视觉区分：

```cpp
ZAddress Address;
SAddressRecord Record;
TVector<ZAddress> Addresses;
```

---

## 语言标准与标准库边界

### C++ 标准

项目默认使用 **C++20** 或更新标准。C++23 可用时允许使用 `std::expected` 后端。

---

### 不做无语义封装

标准库基础设施不为风格统一而额外封装成空壳类。只在需要额外语义、不变量、校验、生命周期或接口约束时定义新的 `Z` / `S` / `T` 类型。

好的做法：

```cpp
using String = std::string;
using StringView = std::string_view;
using Path = std::filesystem::path;

template <typename TValue>
using TVector = std::vector<TValue>;

template <typename TValue>
using TOptional = std::optional<TValue>;
```

不好的做法：

```cpp
class ZString
{
private:
    std::string Value;
};
```

如果没有额外语义，`ZString` 只是增加构造、比较、哈希、互转、调试和模板报错成本。

可以封装的情况：

```cpp
class ZEmailAddress
{
public:
    static TResult<ZEmailAddress> Parse(StringView Text);
    NODISCARD StringView ToString() const noexcept;

private:
    explicit ZEmailAddress(String Text);

private:
    String Text;
};
```

这里 `ZEmailAddress` 有校验语义，不是任意字符串。

---

### 基础别名策略

基础值类型别名不加 `Z` / `S` / `T`：

```cpp
using int32 = std::int32_t;
using uint64 = std::uint64_t;
using float32 = float;
using String = std::string;
using StringView = std::string_view;
using Path = std::filesystem::path;
```

模板型标准库别名使用 `T` 前缀：

```cpp
template <typename TValue>
using TVector = std::vector<TValue>;

template <typename TValue>
using TOptional = std::optional<TValue>;

template <typename TKey, typename TValue>
using THashMap = std::unordered_map<TKey, TValue>;
```

推荐：

```cpp
String Name;
Path ConfigPath;
TVector<ZTexture> Textures;
TOptional<SAssetRecord> Record;
THashMap<String, ZAddress> Addresses;
```

不推荐：

```cpp
ZString Name;
ZPath ConfigPath;
Vector<ZTexture> Textures;
Optional<SAssetRecord> Record;
```

---

### 编译警告

项目代码应在高警告等级下保持干净。建议开启：

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

所有代码必须由 `clang-format` 或等效工具格式化。推荐基础配置：

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

文件名使用 `PascalCase`，与主要类型名一致。若文件是模块聚合头，可以使用模块名。

好的做法：

```text
ZAssetDatabase.h
ZAssetDatabase.cpp
SAssetRecord.h
IFileSystem.h
ZeroStyle.h
```

可以接受：

```text
Result.h      // 模块名，内部提供 TResult
Types.h       // 模块名，内部提供基础别名
Concepts.h    // 模块名，内部提供 C 前缀 concepts
```

例外：第三方库、构建脚本、平台约定文件可以使用其生态惯例。

---

### 头文件保护

优先使用 `#pragma once`。

```cpp
#pragma once

#include <filesystem>
#include <string>
```

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
#include "ZAssetDatabase.h"

#include <cassert>
#include <filesystem>
#include <string>

#include <fmt/format.h>

#include "Core/Result.h"
#include "IO/IFileSystem.h"
```

---

### 前向声明

能用前向声明解决的地方，优先使用前向声明，减少头文件依赖。

```cpp
class IFileSystem;

class ZAssetDatabase
{
public:
    explicit ZAssetDatabase(const IFileSystem& FileSystemRef);

private:
    const IFileSystem& FileSystem;
};
```

---

## 格式化规则

### 缩进

使用 4 个空格缩进，不使用 Tab。

```cpp
if (bReady)
{
    Start();
}
```

---

### 花括号

使用 Allman 风格，花括号独占一行。

```cpp
void ZAssetDatabase::Close()
{
    bOpen = false;
}
```

---

### 行宽

推荐行宽不超过 100 列。复杂函数签名应换行。

```cpp
NODISCARD TResult<Path> ResolveAssetPath(
    StringView Name,
    EResolveFlags Flags) const;
```

---

### 指针与引用

`*` 和 `&` 靠近类型。

```cpp
ZTexture* FindTexture(StringView Name);
const ZTexture& GetDefaultTexture() const;
```

---

### 空行

用空行分隔逻辑块，不要过度压缩。

```cpp
const auto Record = Manifest.FindAsset(Name);

if (!Record.has_value())
{
    return TResult<Path>::Err(MakeError(EErrorCode::AssetNotFound, "asset not found"));
}

return TResult<Path>::Ok(BasePath / Record->RelativePath);
```

---

## 命名规则

### 类型名

项目自有类型使用前缀加 `PascalCase`。

```cpp
class ZAssetDatabase;
struct SAssetRecord;
template <typename TValue>
class TResult;
class IFileSystem;
enum class ETextureFormat;
template <typename TValue>
concept CStringLike = std::convertible_to<TValue, StringView>;
```

类型别名分两类：

```cpp
using String = std::string;                // 基础值类型，无前缀
using Path = std::filesystem::path;        // 基础值类型，无前缀

template <typename TValue>
using TVector = std::vector<TValue>;       // 模板别名，T 前缀
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

变量名使用 `PascalCase`。因为类型带前缀，变量可以自然使用核心语义名。

好的做法：

```cpp
ZAddress Address;
SAddressRecord Record;
TResult<Path> PathResult;
int32 RetryCount = 0;
```

可以接受：

```cpp
ZTexture Texture;
ZWindow Window;
```

在无前缀类型上仍应避免变量名与类型名完全相同：

```cpp
String Text;
Path ConfigPath;
```

不推荐：

```cpp
String String;
Path Path;
```

---

### 成员变量

成员变量使用 `PascalCase`，不使用 `m_` 或尾随下划线。输入参数使用普通 `PascalCase` 语义名；当参数名会与成员变量冲突时，使用 `New`、`Initial`、`Source`、`File` 等语义词，而不是强制添加 `In`。

```cpp
class ZUser
{
public:
    explicit ZUser(ZAddress NewAddress)
        : Address(std::move(NewAddress))
    {
    }

    void SetAddress(ZAddress NewAddress)
    {
        Address = std::move(NewAddress);
    }

private:
    ZAddress Address;
};
```

这里 `ZAddress` 是类型，`Address` 是成员，`NewAddress` 是语义化输入参数。

---

### 布尔变量

布尔变量使用 `b` 前缀加 `PascalCase`。

```cpp
bool bOpen = false;
bool bLoaded = false;
bool bUseVSync = true;
bool bHasFocus = false;
```

条件表达式会更容易阅读：

```cpp
if (bOpen && bHasFocus && !bPendingClose)
{
    Update();
}
```

---

### 接口名

纯接口类使用 `I` 前缀。

```cpp
class IFileSystem
{
public:
    virtual ~IFileSystem() = default;

    NODISCARD virtual TResult<String> ReadTextFile(const Path& FilePath) const = 0;
};
```

`I` 只用于抽象接口，不用于普通基类。

---

### 枚举

枚举类型使用 `E` 前缀，枚举值使用 `PascalCase`。

```cpp
enum class ETextureFormat
{
    Unknown,
    Rgba8,
    Bgra8,
    Rgba16F,
};
```

---

### 模板参数

模板类型参数使用 `T` 前缀，并带语义名。

好的做法：

```cpp
template <typename TValue, typename TError>
class TResult final;

template <typename TKey, typename TValue>
using THashMap = std::unordered_map<TKey, TValue>;
```

不好的做法：

```cpp
template <typename T>
class TResult;
```

`TValue`、`TError`、`TKey` 比单字母 `T` 更清楚。

---

### 常量

常量使用 `PascalCase`。全局常量优先放入命名空间并使用 `inline constexpr`。

```cpp
inline constexpr int32 MaxAssetNameLength = 128;
inline constexpr StringView DefaultManifestName = "Assets.manifest";
```

`ALL_CAPS` 仅用于宏。

---

### 命名空间

命名空间使用 `PascalCase` 或简短项目名。头文件中禁止使用 `using namespace`。

```cpp
namespace Zero {

class ZAssetDatabase
{
};

}  // namespace Zero
```

`.cpp` 中可以有限使用具名 `using`：

```cpp
using Zero::String;
using Zero::TVector;
```

不推荐：

```cpp
using namespace std;
using namespace fmt;
```

---

## 类型设计

### class 与 struct

当类型主要表示数据集合且成员可以公开访问时，使用 `struct`，并加 `S` 前缀。当类型维护不变量、管理资源或提供行为时，使用 `class`，并加 `Z` 前缀。

```cpp
struct SAssetRecord
{
    String Name;
    Path RelativePath;
    bool bPreload = false;
};
```

```cpp
class ZAssetDatabase
{
public:
    NODISCARD TVoidResult<> Open(Path ManifestPath);
    NODISCARD bool IsOpen() const noexcept;

private:
    ZAssetManifest Manifest;
    bool bOpen = false;
};
```

不好的做法：

```cpp
class ZAssetRecord
{
public:
    String Name;
    Path RelativePath;
};
```

如果只是数据，`struct S...` 更直接。

---

### final

`final` 用于表达明确的继承边界，不作为默认样板。

只有当类型确实不应被继承，或需要禁止进一步派生以保护不变量、资源生命周期、ABI/插件边界时，才使用 `final`。普通数据 `struct`、示例类型、尚未稳定的业务类不强制写 `final`。

```cpp
class ZNativeFileSystem final : public IFileSystem
{
public:
    TResult<String> ReadTextFile(const Path& FilePath) const override;
};
```

不推荐：

```cpp
struct SAssetRecord final
{
    String Name;
    Path RelativePath;
};
```

更轻量：

```cpp
struct SAssetRecord
{
    String Name;
    Path RelativePath;
};
```

---

### 构造函数

单参数构造函数默认使用 `explicit`。

```cpp
class ZAssetDatabase
{
public:
    explicit ZAssetDatabase(const IFileSystem& FileSystemRef);
};
```

---

### 拷贝与移动

拥有资源、绑定外部依赖或维护复杂状态的类，应显式声明拷贝和移动策略。

```cpp
class ZAssetDatabase
{
public:
    ZAssetDatabase(const ZAssetDatabase&) = delete;
    ZAssetDatabase& operator=(const ZAssetDatabase&) = delete;

    ZAssetDatabase(ZAssetDatabase&&) = delete;
    ZAssetDatabase& operator=(ZAssetDatabase&&) = delete;
};
```

---

### 访问控制顺序

类成员顺序如下：

1. `public` 类型别名、构造函数、析构函数。
2. `public` API。
3. `protected` 成员。
4. `private` 辅助函数。
5. `private` 数据成员。

```cpp
class ZTextureLoader
{
public:
    explicit ZTextureLoader(IFileSystem& FileSystemRef);

    NODISCARD TResult<ZTexture> LoadFromFile(const Path& FilePath);
    NODISCARD bool IsLoaded() const noexcept;

private:
    NODISCARD static TResult<STextureInfo> ParseHeader(std::span<const std::byte> Data);

    IFileSystem& FileSystem;
    STextureInfo Info;
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

### 输出参数推荐使用 Out 前缀

当必须使用非返回值输出参数时，推荐使用 `Out` 前缀。普通输入参数不使用 `In` 前缀。

```cpp
NODISCARD bool ParseColor(StringView Text, SColor& OutColor);
```

避免：

```cpp
bool ParseColor(StringView Text, SColor& Color);
```

更好的做法是直接返回 `TResult<T>`：

```cpp
NODISCARD TResult<SColor> ParseColor(StringView Text);
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
enum class EOpenMode
{
    ReadOnly,
    ReadWrite,
};

enum class ECreatePolicy
{
    MustExist,
    CreateIfMissing,
};

Window.Open(EOpenMode::ReadWrite, ECreatePolicy::CreateIfMissing);
```

---

### 使用标准属性宏

返回错误、状态、资源句柄、查找结果的函数必须使用属性宏。公共头文件优先使用默认的 `ZERO_` 前缀宏。

```cpp
ZERO_NODISCARD TResult<ZTexture> LoadTexture(const Path& FilePath);
ZERO_NODISCARD bool ContainsAsset(StringView Name) const;
ZERO_NODISCARD TOptional<SAssetRecord> FindAsset(StringView Name) const;
```

类型本身不应被调用点忽略时，使用 `ZERO_NODISCARD_TYPE`。

```cpp
struct ZERO_NODISCARD_TYPE SParseToken
{
    String Text;
};
```

项目代码不直接写 `[[nodiscard]]`，统一使用 `ZERO_NODISCARD` 或 `ZERO_NODISCARD_TYPE`。

个人项目如果希望保持更短的书写风格，可以在包含 ZeroStyle 前显式定义 `ZERO_ENABLE_SHORT_MACROS`。短宏只适合个人项目或受控边界，不作为默认低侵入接口。

```cpp
#define ZERO_ENABLE_SHORT_MACROS
#include "ZeroStyle.h"

NODISCARD TResult<ZTexture> LoadTexture(const Path& FilePath);
```

其他标准属性也统一使用 ZeroStyle 提供的属性宏，保持项目视觉风格一致。

| 宏 | 对应标准属性 | 用途 |
|---|---|---|
| `ZERO_NODISCARD` | `[[nodiscard]]` | 函数返回值不应被忽略 |
| `ZERO_NODISCARD_TYPE` | `[[nodiscard]]` | 类型值不应被忽略 |
| `ZERO_NORETURN` | `[[noreturn]]` | 函数不会返回调用点 |
| `ZERO_MAYBE_UNUSED` | `[[maybe_unused]]` | 明确允许实体未使用 |
| `ZERO_DEPRECATED` | `[[deprecated]]` | 标记 API 已废弃 |
| `ZERO_DEPRECATED_MSG("reason")` | `[[deprecated("reason")]]` | 带说明的废弃标记 |
| `ZERO_FALLTHROUGH` | `[[fallthrough]]` | `switch` 中有意贯穿 |
| `ZERO_NO_UNIQUE_ADDRESS` | `[[no_unique_address]]` | 允许空成员不占用额外存储 |

好的做法：

```cpp
ZERO_NORETURN void AbortWithMessage(StringView Message);

void VisitToken(ZERO_MAYBE_UNUSED StringView Token);

switch (Kind)
{
case ETokenKind::Whitespace:
    SkipWhitespace();
    ZERO_FALLTHROUGH;
case ETokenKind::Comment:
    SkipTrivia();
    break;
default:
    break;
}
```

不推荐：

```cpp
[[noreturn]] void AbortWithMessage(StringView Message);
[[maybe_unused]] StringView Token;
```

---

### 项目公共头

推荐每个项目建立自己的公共基础头，例如 `ProjectCore.h`，在项目命名空间内选择性转发 ZeroStyle 类型。公共头文件不要写 `using namespace Zero;`。

```cpp
#pragma once

#include "ZeroStyle.h"

namespace Project {

using Zero::Path;
using Zero::String;
using Zero::StringView;
using Zero::TOptional;
using Zero::TResult;
using Zero::TVector;
using Zero::int32;

}  // namespace Project
```

业务头文件包含项目公共头后，可以使用项目自己的风格入口。

```cpp
#pragma once

#include "ProjectCore.h"

namespace Project {

struct SConfig
{
    String Name;
};

NODISCARD TResult<SConfig> ParseConfig(String Text);

}  // namespace Project
```

---

### const 和 noexcept

不会修改对象状态的成员函数应标记 `const`。保证不抛异常的轻量函数可以标记 `noexcept`。

```cpp
NODISCARD bool IsOpen() const noexcept;
NODISCARD const Path& ManifestPath() const noexcept;
```

---

### 函数长度

函数应保持短小。超过约 50 行的函数应考虑拆分。超过约 100 行的函数通常需要重构。

```cpp
TVoidResult<> ZAssetDatabase::Open(Path ManifestPath)
{
    auto TextResult = FileSystem.ReadTextFile(ManifestPath);

    if (TextResult.IsErr())
    {
        return TVoidResult<>::Err(TextResult.Failure());
    }

    auto ManifestResult = ParseManifest(TextResult.Value(), ManifestPath);

    if (ManifestResult.IsErr())
    {
        return TVoidResult<>::Err(ManifestResult.Failure());
    }

    ApplyManifest(std::move(ManifestPath), std::move(ManifestResult).TakeValue());
    return TVoidResult<>::Ok(SUnit{});
}
```

---

## 变量与成员

### 局部变量

局部变量使用 `PascalCase`，并尽量靠近首次使用处声明。

```cpp
const auto Record = Manifest.FindAsset(Name);

if (!Record.has_value())
{
    return TResult<Path>::Err(MakeError(EErrorCode::AssetNotFound, "asset not found"));
}
```

---

### 初始化

优先在声明处初始化变量。成员变量应使用默认成员初始化器。

```cpp
struct STextureInfo
{
    int32 Width = 0;
    int32 Height = 0;
    ETextureFormat Format = ETextureFormat::Unknown;
    bool bHasMipmaps = false;
};
```

---

### auto

`auto` 可用于类型明显或类型冗长的场景。不要用 `auto` 隐藏重要类型。

好的做法：

```cpp
auto PathResult = Database.ResolveAssetPath("PlayerIcon");
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

项目代码默认不使用异常表达可预期错误。可预期错误通过 `TResult<T>` 或 `TOptional<T>` 返回。

```cpp
NODISCARD TResult<String> ReadTextFile(const Path& FilePath) const;
```

异常可用于不可恢复错误、第三方库边界或程序启动阶段，但不应作为普通控制流。

---

### 使用 TResult<T>

可能失败且需要错误信息的函数返回 `TResult<TValue, TError>`。默认错误类型是 `SError`。

```cpp
NODISCARD TResult<ZAssetManifest> ParseManifest(
    StringView Text,
    const Path& ManifestPath);
```

调用方必须处理成功和失败：

```cpp
auto ManifestResult = ParseManifest(Text, ManifestPath);

if (ManifestResult.IsErr())
{
    return TResult<ZAssetDatabase>::Err(ManifestResult.Failure());
}

ZAssetManifest Manifest = std::move(ManifestResult).TakeValue();
```

---

### std::expected 升级路径

若项目启用 C++23 且标准库提供 `std::expected`，`TResult<TValue, TError>` 会在内部使用 `std::expected` 存储；公开 API 仍保持 ZeroStyle 的 PascalCase 接口，避免 C++20 与 C++23 下调用方式不一致。

```cpp
auto PathResult = Database.ResolveAssetPath("PlayerIcon");

if (PathResult.IsErr())
{
    LogError(PathResult.Failure().Message);
    return;
}

auto AssetPath = std::move(PathResult).TakeValue();
```

### 使用 TOptional<T> 表达“没有”

当失败只有“有或没有”，且不需要错误原因时，使用 `TOptional<T>`。

```cpp
NODISCARD TOptional<SAssetRecord> FindAsset(StringView Name) const;
```

如果找不到不是错误，只是查询结果为空，`TOptional` 更合适。

---

### 错误信息

错误类型应包含机器可读的错误码和人类可读的信息。

```cpp
enum class EErrorCode
{
    FileNotFound,
    InvalidManifest,
    DuplicateAsset,
    AssetNotFound,
};

struct SError
{
    EErrorCode Code;
    String Message;
    Path ContextPath;
};
```

---

### 不忽略错误

禁止无理由忽略 `TResult<T>`。

```cpp
auto SaveResult = Config.Save();

if (SaveResult.IsErr())
{
    LogError(FormatError(SaveResult.Failure()));
    return SaveResult;
}
```

如果确实要忽略，应显式说明理由：

```cpp
(void)Cache.TryWarmUp();  // Best-effort optimization. Failure is non-fatal.
```

---

## 资源管理与所有权

### 使用 RAII

资源应由对象生命周期管理，不手动配对 `Init` / `Destroy`。

```cpp
class ZFileHandle final
{
public:
    ~ZFileHandle();

private:
    NativeHandle Handle = InvalidHandle;
};
```

---

### 使用智能指针表达所有权

独占所有权使用 `TUniquePtr<T>`，共享所有权使用 `TSharedPtr<T>`，非拥有引用使用指针或引用。

```cpp
TUniquePtr<ZTexture> CreateTexture(STextureDesc Desc);

class ZRenderer
{
public:
    explicit ZRenderer(IFileSystem& FileSystemRef);

private:
    IFileSystem& FileSystem;  // Non-owning.
};
```

裸指针可以用于非拥有、可空引用，但必须语义清晰。

---

### 避免全局可变状态

全局可变状态会降低可测试性和可推理性。优先使用依赖注入。

```cpp
class ZAssetDatabase
{
public:
    explicit ZAssetDatabase(const IFileSystem& FileSystemRef);

private:
    const IFileSystem& FileSystem;
};
```

---

## 宏、模板与编译期代码

### 宏

宏应尽量少用。宏名必须使用 `ALL_CAPS`。

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

不要用宏制造伪语言。

---

### constexpr 与 consteval

能在编译期表达的常量和简单函数，优先使用 `constexpr`。

```cpp
constexpr int32 Kilobytes(int32 Value)
{
    return Value * 1024;
}
```

---

### 模板

模板应服务于类型安全和复用，不应过度抽象。

```cpp
template <typename TValue>
class TResult final
{
};
```

模板参数过多时，应考虑拆分职责或引入配置结构体。

---

### Concepts

用 `concept` 约束模板参数，替代未约束的 `typename T`。

```cpp
template <Zero::CStringLike TName>
NODISCARD TOptional<SAssetRecord> FindAsset(TName&& Name) const;
```

`ZeroStyle.h` 提供以下常用 concept：

| Concept | 含义 |
|---|---|
| `CStringLike` | 可转换为 `std::string_view` |
| `CPathLike` | 可转换为 `std::filesystem::path` |
| `CHashable` | 可用作无序容器键 |
| `CErrorType` | 拥有 `Code` 与 `Message` 成员 |

---

## 注释与文档

### 注释解释原因，不重复代码

好的做法：

```cpp
// Keep the old manifest active until parsing succeeds so Reload() is atomic.
ZAssetManifest NewManifest = std::move(ManifestResult).TakeValue();
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

```cpp
// Opens and parses an asset manifest.
//
// Returns InvalidManifest if the manifest syntax is invalid.
// Returns FileNotFound if the file system cannot read ManifestPath.
// On failure, the previous open manifest remains unchanged.
NODISCARD TVoidResult<> Open(Path ManifestPath);
```

---

### 线程安全

类声明处应注明线程安全性；函数签名可进一步细化。

```cpp
// Thread-safe.
class ZEventBus
{
public:
    void Publish(SEvent Event) ZERO_EXCLUDES(Mutex);
    void Subscribe(ZEventHandler Handler) ZERO_EXCLUDES(Mutex);

private:
    mutable std::mutex Mutex;
    TVector<ZEventHandler> Handlers ZERO_GUARDED_BY(Mutex);
};
```

```cpp
// Not thread-safe. External synchronization required.
class ZAssetDatabase
{
public:
    NODISCARD TVoidResult<> Open(Path ManifestPath);
};
```

`ZERO_GUARDED_BY` / `ZERO_EXCLUDES` / `ZERO_REQUIRES` 宏由 `ZeroStyle.h` 提供。Clang 线程安全分析默认不强加给消费者，需要通过 CMake 选项显式启用。

---

## 测试代码风格

测试代码可以比生产代码更直接，但仍应遵守命名、错误处理和格式化规则。

好的做法：

```cpp
TEST(ZAssetDatabaseTest, OpenReturnsInvalidManifestForMalformedLine)
{
    ZMemoryFileSystem FileSystem;
    FileSystem.AddTextFile("Assets.manifest", "BrokenLine");

    ZAssetDatabase Database(FileSystem);
    auto OpenResult = Database.Open("Assets.manifest");

    ASSERT_TRUE(OpenResult.IsErr());
    EXPECT_EQ(OpenResult.Failure().Code, EErrorCode::InvalidManifest);
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

- `Z` class 前缀。
- `S` struct 前缀。
- `T` 模板类型和模板别名前缀。
- `I` 接口前缀。
- `E` 枚举前缀。
- `C` concept 前缀。
- `b` 布尔前缀。
- `Out` 输出参数推荐前缀。
- `TResult<T>` 显式错误处理。
- 基础标准库类型只做 alias，不做无语义封装。

```cpp
#pragma once

#include "ZeroStyle.h"

#include <string>
#include <utility>

namespace Project {

struct SAssetRecord
{
    String Name;
    Path RelativePath;
    bool bPreload = false;
};

class IFileSystem
{
public:
    virtual ~IFileSystem() = default;

    NODISCARD virtual TResult<String> ReadTextFile(const Path& FilePath) const = 0;
};

class ZAssetManifest
{
public:
    NODISCARD bool ContainsAsset(StringView Name) const
    {
        return RecordsByName.contains(String(Name));
    }

    NODISCARD TOptional<SAssetRecord> FindAsset(StringView Name) const
    {
        const auto It = RecordsByName.find(String(Name));

        if (It == RecordsByName.end())
        {
            return std::nullopt;
        }

        return It->second;
    }

    NODISCARD TVoidResult<> AddRecord(SAssetRecord Record)
    {
        if (Record.Name.empty())
        {
            return TVoidResult<>::Err(MakeError(
                EErrorCode::InvalidManifest,
                "asset name must not be empty"));
        }

        const String Name = Record.Name;
        const auto [It, bInserted] = RecordsByName.emplace(Name, std::move(Record));
        (void)It;

        if (!bInserted)
        {
            return TVoidResult<>::Err(MakeError(
                EErrorCode::DuplicateAsset,
                "duplicate asset name: " + Name));
        }

        return TVoidResult<>::Ok(SUnit{});
    }

private:
    THashMap<String, SAssetRecord> RecordsByName;
};

class ZAssetDatabase
{
public:
    explicit ZAssetDatabase(const IFileSystem& FileSystemRef)
        : FileSystem(FileSystemRef)
    {
    }

    ZAssetDatabase(const ZAssetDatabase&) = delete;
    ZAssetDatabase& operator=(const ZAssetDatabase&) = delete;

    ZAssetDatabase(ZAssetDatabase&&) = delete;
    ZAssetDatabase& operator=(ZAssetDatabase&&) = delete;

    NODISCARD bool IsOpen() const noexcept
    {
        return bOpen;
    }

    NODISCARD TResult<Path> ResolveAssetPath(StringView Name) const
    {
        if (!bOpen)
        {
            return TResult<Path>::Err(MakeError(
                EErrorCode::NotOpen,
                "database is not open"));
        }

        const auto Record = Manifest.FindAsset(Name);

        if (!Record.has_value())
        {
            return TResult<Path>::Err(MakeError(
                EErrorCode::AssetNotFound,
                "asset not found: " + String(Name),
                CurrentManifestPath));
        }

        return TResult<Path>::Ok(CurrentManifestPath.parent_path() / Record->RelativePath);
    }

private:
    const IFileSystem& FileSystem;
    Path CurrentManifestPath;
    ZAssetManifest Manifest;
    bool bOpen = false;
};

}  // namespace Project
```

---

## 总结

本风格可以概括为：

```text
PascalCase Modern C++
UE-inspired hard prefixes
No meaningless standard-library wrappers
Google/Chromium-like engineering discipline
Qt-like API readability
Rust-like explicit error handling
```

最终目标不是“像 UE”，而是让个人项目中的 C++ 代码满足这些效果：

- 类型、函数、变量在视觉上清晰。
- `class`、`struct`、模板、接口、枚举、concept 能从名字直接区分。
- 布尔值、输入输出方向、错误处理有明确提示。
- API 从调用点就能读懂。
- 标准库能力照用，不重新发明基础设施。
- 依赖关系清晰，模块容易测试和维护。
