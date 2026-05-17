# Python 代码风格指南：PascalCase / Zero 风格版

本文档定义一套面向个人项目的 Python 代码风格。它参考 `ZeroStyle` 的 C++ 规范：项目自有代码统一使用 `PascalCase` 作为主视觉风格，并通过前缀表达类型语义；同时保留 Python 必要的运行时约定、类型标注、导入规则和工具链纪律。

Python 生态默认推荐函数、变量、模块使用 `snake_case`。本规范是有意偏离默认生态的个人项目风格，因此要特别注意边界：**项目自有代码可以统一 PascalCase；标准库、第三方库、框架钩子、魔术方法、命令行入口、序列化字段、ORM 字段和协议要求的名字必须尊重对方约定。**

本规范的核心目标是：**代码一眼能读懂，接口一眼能用对，错误一眼能处理，边界一眼能分清。**

---

## 目录

1. [总体原则](#总体原则)
2. [前缀体系](#前缀体系)
3. [Python 版本与生态边界](#python-版本与生态边界)
4. [文件组织](#文件组织)
5. [格式化规则](#格式化规则)
6. [命名规则](#命名规则)
7. [类型设计](#类型设计)
8. [函数与 API 设计](#函数与-api-设计)
9. [变量与成员](#变量与成员)
10. [错误处理](#错误处理)
11. [资源管理与上下文管理器](#资源管理与上下文管理器)
12. [泛型、协议与类型标注](#泛型协议与类型标注)
13. [注释与文档](#注释与文档)
14. [测试代码风格](#测试代码风格)
15. [工具链建议](#工具链建议)
16. [完整示例](#完整示例)

---

## 总体原则

### 保持一致性

项目自有代码统一使用 `PascalCase`。类型、函数、变量、成员变量都采用同一主视觉风格，再通过前缀区分语义类别。

好的做法：

```python
class ZTextureLoader:
    def LoadFromFile(self, FilePath: Path) -> TResult[ZTexture, SError]:
        ...

    def IsLoaded(self) -> bool:
        return self._bLoaded
```

不好的做法：

```python
class texture_loader:
    def load_from_file(self, file_path: Path) -> ZTexture:
        ...

    def isLoaded(self) -> bool:
        return self.loaded_
```

同一项目中混用 `snake_case`、`camelCase`、尾随下划线、`m_` 等风格会降低可读性。除非与 Python 生态、第三方库或框架约定对接，否则项目自有代码应遵守本文档。

---

### 可读性优先于炫技

不要为了展示语言技巧而写复杂代码。优先选择容易调试、容易审查、容易维护的写法。

好的做法：

```python
if not Database.IsOpen():
    return TResult.Err(MakeError(EErrorCode.NotOpen, "database is not open"))
```

不好的做法：

```python
return Database.ResolveAssetPath(Name) if Database.IsOpen() else TResult.Err(MakeError(EErrorCode.NotOpen, ""))
```

列表推导、生成器表达式、装饰器、元类、反射和猴子补丁都应谨慎使用。表达清晰比压缩行数更重要。

---

### 显式优先于隐式

重要行为应显式表达，包括错误、所有权、输出方向、可变性、I/O、副作用和边界转换。

好的做法：

```python
def ResolveAssetPath(self, Name: String) -> TResult[Path, SError]:
    ...
```

不好的做法：

```python
def Get(self, Name):
    ...
```

`ResolveAssetPath` 比 `Get` 更清楚，`TResult[Path, SError]` 比隐藏失败条件更清楚，类型标注能帮助编辑器和静态检查工具发现误用。

---

## 前缀体系

本风格使用明确的前缀区分类型类别。前缀不是装饰，而是类型语义的一部分。

| 前缀 | 用途 | 示例 |
|---|---|---|
| `Z` | 普通 `class`，通常拥有行为、不变量、生命周期或资源 | `ZAssetDatabase`、`ZWindow` |
| `S` | 数据结构，通常使用 `dataclass`，主要表示配置、记录、消息、描述符、错误对象 | `SAssetRecord`、`SWindowDesc`、`SError` |
| `T` | 泛型类、泛型别名、类型变量、类型工具 | `TResult`、`TValue`、`TOptional` |
| `I` | 接口或依赖边界，通常使用 `Protocol` 或 `ABC` | `IFileSystem`、`IAssetProvider` |
| `E` | 枚举，继承 `Enum`、`IntEnum` 或 `StrEnum` | `EErrorCode`、`ETextureFormat` |
| `C` | 泛型约束、结构化协议、能力约束 | `CPathLike`、`CSerializable` |
| `b` | 布尔变量或布尔字段 | `bOpen`、`bLoaded`、`bUseCache` |
| `Out` | 输出参数或被函数填充的可变对象，非必要不使用 | `OutRecord`、`OutErrors` |
| `_` | Python 非公开成员标记，主词仍保持 PascalCase | `_Manifest`、`_bOpen` |

输入参数默认使用普通 `PascalCase` 语义名，不强制使用 `In` 前缀。必须使用被调用方修改的参数时，推荐使用 `Out` 前缀。遇到输入输出参数时，优先改成返回值、结果对象或数据类。

好的做法：

```python
class ZAddress:
    ...

@dataclass(slots=True)
class SAddressRecord:
    Name: String
    PostalCode: String

TValue = TypeVar("TValue")

class IAddressResolver(Protocol):
    def ResolveAddress(self, Text: String) -> TResult[ZAddress, SError]:
        ...

class EAddressKind(Enum):
    Home = auto()
    Work = auto()
```

使用时：

```python
Address = ZAddress()
AddressRecord = SAddressRecord(Name="Zero", PostalCode="000000")
AddressResult = Resolver.ResolveAddress("Somewhere")
Kind = EAddressKind.Home
bValid = AddressResult.IsOk()
```

这种写法允许变量名和类型核心名接近，但仍能视觉区分：

```python
Address: ZAddress
Record: SAddressRecord
Addresses: list[ZAddress]
```

---

## Python 版本与生态边界

### Python 版本

项目默认使用 **Python 3.11 或更新版本**。如果项目完全由自己控制，推荐使用 Python 3.12+，以便使用更清晰的泛型别名语法。

Python 3.12+ 推荐：

```python
type TOptional[TValue] = TValue | None
type TList[TValue] = list[TValue]
type THashMap[TKey, TValue] = dict[TKey, TValue]
```

Python 3.11 兼容写法：

```python
TValue = TypeVar("TValue")
TKey = TypeVar("TKey")

TOptional: TypeAlias = Optional[TValue]
TList: TypeAlias = list[TValue]
THashMap: TypeAlias = dict[TKey, TValue]
```

---

### 生态边界

以下名字必须遵守 Python 或第三方生态约定，不强行 PascalCase：

```python
__init__
__enter__
__exit__
__iter__
__next__
__len__
__str__
__repr__
self
cls
```

框架或工具要求的函数名也应保持原样：

```python
def pytest_configure(config):
    ...

async def lifespan(app):
    ...
```

序列化、数据库、HTTP、JSON、CLI、环境变量等外部字段名不要为了风格强行改名。内部使用 PascalCase，边界处做显式映射。

```python
@dataclass(slots=True)
class SUserRecord:
    UserId: int
    DisplayName: String

    @staticmethod
    def FromJson(Data: dict[str, object]) -> "SUserRecord":
        return SUserRecord(
            UserId=int(Data["user_id"]),
            DisplayName=str(Data["display_name"]),
        )

    def ToJson(self) -> dict[str, object]:
        return {
            "user_id": self.UserId,
            "display_name": self.DisplayName,
        }
```

---

### 不做无语义封装

标准库基础设施不为风格统一而额外封装成空壳类。只在需要额外语义、不变量、校验、生命周期或接口约束时定义新的 `Z` / `S` / `T` 类型。

好的做法：

```python
String: TypeAlias = str
Bytes: TypeAlias = bytes
PathLike: TypeAlias = str | Path
```

不好的做法：

```python
class ZString:
    def __init__(self, Value: str) -> None:
        self.Value = Value
```

如果没有额外语义，`ZString` 只是增加构造、比较、序列化、调试和类型转换成本。

可以封装的情况：

```python
class ZEmailAddress:
    def __init__(self, Text: String) -> None:
        if "@" not in Text:
            raise ValueError("invalid email address")
        self._Text = Text

    @staticmethod
    def Parse(Text: String) -> TResult["ZEmailAddress", SError]:
        if "@" not in Text:
            return TResult.Err(MakeError(EErrorCode.InvalidArgument, "invalid email address"))
        return TResult.Ok(ZEmailAddress(Text))

    def ToString(self) -> String:
        return self._Text
```

这里 `ZEmailAddress` 有校验语义，不是任意字符串。

---

## 文件组织

### 文件名

自有源码文件推荐使用 `PascalCase.py`，并尽量与主要类型名一致。

好的做法：

```text
ZAssetDatabase.py
ZAssetManifest.py
SAssetRecord.py
IFileSystem.py
Result.py
Types.py
Errors.py
```

如果项目需要发布到 PyPI、暴露给大量 Python 用户，或者要符合框架自动发现约定，可以在公开包边界使用生态惯例的小写模块名；内部代码仍可保持 PascalCase。

```text
zero_style/
    __init__.py
    AssetDatabase.py
    Result.py
```

---

### 包结构

推荐按职责组织，而不是按“工具类”堆叠。

```text
Project/
    __init__.py
    Core/
        __init__.py
        Types.py
        Result.py
        Error.py
    IO/
        __init__.py
        IFileSystem.py
        ZNativeFileSystem.py
    Asset/
        __init__.py
        SAssetRecord.py
        ZAssetManifest.py
        ZAssetDatabase.py
    Tests/
        TestAssetDatabase.py
```

包目录是否使用 PascalCase 取决于项目边界：

- 个人项目、应用内模块：可以使用 `PascalCase` 包目录。
- 公开 Python 包：建议包目录使用小写，降低跨平台和工具链摩擦。

---

### 导入顺序

导入顺序如下：

1. `from __future__ import ...`。
2. 标准库。
3. 第三方库。
4. 项目内部模块。

好的做法：

```python
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Protocol

import orjson

from Project.Core.Error import EErrorCode, SError, MakeError
from Project.Core.Result import TResult
from Project.IO.IFileSystem import IFileSystem
```

不要使用通配符导入：

```python
from Project.Core.Result import *  # 不推荐
```

---

### `__init__.py`

`__init__.py` 应保持轻量，只做稳定 API 的显式导出，不执行重逻辑。

```python
from Project.Asset.SAssetRecord import SAssetRecord
from Project.Asset.ZAssetDatabase import ZAssetDatabase
from Project.Asset.ZAssetManifest import ZAssetManifest

__all__ = [
    "SAssetRecord",
    "ZAssetDatabase",
    "ZAssetManifest",
]
```

---

## 格式化规则

### 缩进

使用 4 个空格缩进，不使用 Tab。

```python
if bReady:
    Start()
```

---

### 行宽

推荐行宽不超过 100 列。复杂函数签名应换行。

```python
def ResolveAssetPath(
    self,
    Name: String,
    Flags: EResolveFlags,
) -> TResult[Path, SError]:
    ...
```

---

### 空行

用空行分隔逻辑块，不要过度压缩。

```python
Record = self._Manifest.FindAsset(Name)

if Record is None:
    return TResult.Err(MakeError(EErrorCode.AssetNotFound, "asset not found"))

return TResult.Ok(self._BasePath / Record.RelativePath)
```

---

### 字符串

普通字符串推荐使用双引号。需要避免转义时可以使用单引号。多行文档字符串使用三双引号。

```python
Message = "asset not found"
Pattern = r"\d+"
```

---

### 表达式

复杂表达式应拆分成有语义的局部变量。

不好的做法：

```python
return TResult.Ok(self._CurrentManifestPath.parent / self._Manifest.FindAsset(Name).RelativePath)
```

好的做法：

```python
Record = self._Manifest.FindAsset(Name)

if Record is None:
    return TResult.Err(MakeError(EErrorCode.AssetNotFound, f"asset not found: {Name}"))

AssetPath = self._CurrentManifestPath.parent / Record.RelativePath
return TResult.Ok(AssetPath)
```

---

## 命名规则

### 类型名

项目自有类型使用前缀加 `PascalCase`。

```python
class ZAssetDatabase:
    ...

@dataclass(slots=True)
class SAssetRecord:
    ...

class TResult(Generic[TValue, TError]):
    ...

class IFileSystem(Protocol):
    ...

class ETextureFormat(Enum):
    ...

class CPathLike(Protocol):
    ...
```

---

### 函数名

项目自有函数和方法使用 `PascalCase`，通常使用动词或动词短语。

好的做法：

```python
Open()
Close()
Reload()
FindAsset()
ContainsAsset()
ResolveAssetPath()
```

不好的做法：

```python
open()
get()
process()
handle()
doWork()
```

函数名应表达行为，不应依赖调用者猜测。

Python 特殊方法必须使用原生名称：

```python
class ZFileHandle:
    def __enter__(self) -> "ZFileHandle":
        return self

    def __exit__(self, ExcType, ExcValue, Traceback) -> None:
        self.Close()
```

---

### 变量名

变量名使用 `PascalCase`。因为类型带前缀，变量可以自然使用核心语义名。

好的做法：

```python
Address = ZAddress()
Record = SAssetRecord(Name="PlayerIcon", RelativePath=Path("UI/Player.png"))
PathResult = Database.ResolveAssetPath("PlayerIcon")
RetryCount = 0
```

在无前缀类型上仍应避免变量名与类型名完全相同：

```python
Text = "hello"
ConfigPath = Path("Config.toml")
```

不推荐：

```python
String = "hello"
Path = Path("Config.toml")
```

---

### 成员变量

公开数据字段使用 `PascalCase`。非公开成员使用 `_` 加 `PascalCase`；布尔非公开成员使用 `_b` 加 `PascalCase`。

```python
class ZUser:
    def __init__(self, NewAddress: ZAddress) -> None:
        self._Address = NewAddress
        self._bActive = True

    def SetAddress(self, NewAddress: ZAddress) -> None:
        self._Address = NewAddress
```

不使用 `m_`、尾随下划线或混合风格：

```python
self.m_Address = Address      # 不推荐
self.address_ = Address       # 不推荐
self.address = Address        # 不推荐，除非是外部协议要求
```

---

### 布尔变量

布尔变量使用 `b` 前缀加 `PascalCase`。

```python
bOpen = False
bLoaded = False
bUseVSync = True
bHasFocus = False
```

条件表达式会更容易阅读：

```python
if bOpen and bHasFocus and not bPendingClose:
    Update()
```

查询函数使用 `Is` / `Has` / `Can` / `Should` 等动词，不强制加 `b`：

```python
def IsOpen(self) -> bool:
    return self._bOpen

def HasAsset(self, Name: String) -> bool:
    return Name in self._RecordsByName
```

---

### 枚举

枚举类型使用 `E` 前缀，枚举值使用 `PascalCase`。

```python
class ETextureFormat(Enum):
    Unknown = auto()
    Rgba8 = auto()
    Bgra8 = auto()
    Rgba16F = auto()
```

如果枚举值需要序列化为稳定字符串，可以使用 `StrEnum`：

```python
class EOpenMode(StrEnum):
    ReadOnly = "read_only"
    ReadWrite = "read_write"
```

内部名字保持 PascalCase，外部值遵守外部协议。

---

### 常量

项目自有常量使用 `PascalCase`。环境变量名、平台宏式常量、第三方约定常量可以使用 `ALL_CAPS`。

```python
MaxAssetNameLength = 128
DefaultManifestName = "Assets.manifest"
```

可以接受：

```python
ENV_CONFIG_PATH = "ZERO_CONFIG_PATH"
```

---

### 模块私有名字

模块内部辅助函数或变量使用 `_` 加 `PascalCase`。

```python
def _ParseManifestLine(Line: String) -> TResult[SAssetRecord, SError]:
    ...

_DefaultEncoding = "utf-8"
```

`__双下划线` 只用于 Python 特殊方法或确实需要名称改写的极少数场景。

---

## 类型设计

### class 与 dataclass

当类型主要表示数据集合且字段可以公开访问时，使用 `dataclass`，并加 `S` 前缀。

```python
@dataclass(slots=True)
class SAssetRecord:
    Name: String
    RelativePath: Path
    bPreload: bool = False
```

当类型维护不变量、管理资源或提供行为时，使用普通 `class`，并加 `Z` 前缀。

```python
class ZAssetDatabase:
    def __init__(self, FileSystem: IFileSystem) -> None:
        self._FileSystem = FileSystem
        self._Manifest = ZAssetManifest()
        self._bOpen = False

    def IsOpen(self) -> bool:
        return self._bOpen
```

不好的做法：

```python
class ZAssetRecord:
    Name: String
    RelativePath: Path
```

如果只是数据，`@dataclass` + `S...` 更直接。

---

### dataclass 默认选项

推荐默认使用 `slots=True`，减少动态属性误用。

```python
@dataclass(slots=True)
class STextureInfo:
    Width: int = 0
    Height: int = 0
    Format: ETextureFormat = ETextureFormat.Unknown
    bHasMipmaps: bool = False
```

只读值对象推荐使用 `frozen=True`。

```python
@dataclass(frozen=True, slots=True)
class SAssetId:
    Value: String
```

需要复杂校验时使用 `__post_init__` 或改用 `Z` 类型。

```python
@dataclass(frozen=True, slots=True)
class SAssetName:
    Value: String

    def __post_init__(self) -> None:
        if not self.Value:
            raise ValueError("asset name must not be empty")
```

---

### 继承

默认避免深继承。需要依赖边界时优先使用 `Protocol`，需要共享实现时再使用普通基类。

```python
class IFileSystem(Protocol):
    def ReadTextFile(self, FilePath: Path) -> TResult[String, SError]:
        ...
```

普通业务类不要因为“可能以后扩展”而预先设计复杂继承层级。

---

### 属性与方法

无副作用、计算轻量、像字段一样读取的值可以使用 `@property`。属性名仍使用 PascalCase。

```python
class ZAssetDatabase:
    @property
    def bOpen(self) -> bool:
        return self._bOpen
```

但对于可能失败、可能有 I/O、可能较重、可能改变状态的行为，应使用方法。

```python
def ResolveAssetPath(self, Name: String) -> TResult[Path, SError]:
    ...
```

---

## 函数与 API 设计

### API 应该像自然语言

API 名称应当能从调用点读出含义。

好的做法：

```python
Database.Open(Path("Game/Assets.manifest"))
Database.Reload()
Database.ContainsAsset("PlayerIcon")
Database.ResolveAssetPath("PlayerIcon")
```

不好的做法：

```python
Database.Init(Path("Game/Assets.manifest"))
Database.DoReload()
Database.Check("PlayerIcon")
Database.Get("PlayerIcon")
```

`Get`、`Set`、`Process`、`Handle`、`Execute` 等泛化名称应谨慎使用。

---

### 参数命名

参数使用 `PascalCase` 语义名。遇到成员变量冲突时，使用 `New`、`Initial`、`Source`、`Target`、`File` 等语义词。

```python
class ZUser:
    def __init__(self, InitialAddress: ZAddress) -> None:
        self._Address = InitialAddress

    def SetAddress(self, NewAddress: ZAddress) -> None:
        self._Address = NewAddress
```

不推荐为了避免冲突使用尾随下划线：

```python
def SetAddress(self, Address_: ZAddress) -> None:
    self._Address = Address_
```

---

### 避免布尔参数陷阱

布尔参数在调用点含义不清晰时，应改用枚举。

不好的做法：

```python
Window.Open(True, False)
```

好的做法：

```python
class EOpenMode(Enum):
    ReadOnly = auto()
    ReadWrite = auto()

class ECreatePolicy(Enum):
    MustExist = auto()
    CreateIfMissing = auto()

Window.Open(EOpenMode.ReadWrite, ECreatePolicy.CreateIfMissing)
```

如果必须使用布尔参数，调用时应使用关键字参数。

```python
Window.SetVisible(bVisible=True)
```

---

### 输出参数推荐使用 Out 前缀

Python 中应尽量避免输出参数。优先返回值、元组、数据类或 `TResult[T]`。

不推荐：

```python
def ParseColor(Text: String, OutColor: SColor) -> bool:
    ...
```

推荐：

```python
def ParseColor(Text: String) -> TResult[SColor, SError]:
    ...
```

必须填充外部可变对象时，使用 `Out` 前缀，并在文档中说明副作用。

```python
def CollectErrors(ManifestText: String, OutErrors: list[SError]) -> None:
    ...
```

---

### 函数长度

函数应保持短小。超过约 50 行的函数应考虑拆分。超过约 100 行的函数通常需要重构。

```python
def Open(self, ManifestPath: Path) -> TResult[None, SError]:
    TextResult = self._FileSystem.ReadTextFile(ManifestPath)

    if TextResult.IsErr():
        return TResult.Err(TextResult.Failure())

    ManifestResult = ParseManifest(TextResult.Value(), ManifestPath)

    if ManifestResult.IsErr():
        return TResult.Err(ManifestResult.Failure())

    self._ApplyManifest(ManifestPath, ManifestResult.Value())
    return TResult.Ok(None)
```

---

## 变量与成员

### 局部变量

局部变量使用 `PascalCase`，并尽量靠近首次使用处声明。

```python
Record = self._Manifest.FindAsset(Name)

if Record is None:
    return TResult.Err(MakeError(EErrorCode.AssetNotFound, "asset not found"))
```

---

### 初始化

构造函数中应完整初始化对象状态。数据类字段应提供合理默认值。

```python
@dataclass(slots=True)
class STextureInfo:
    Width: int = 0
    Height: int = 0
    Format: ETextureFormat = ETextureFormat.Unknown
    bHasMipmaps: bool = False
```

可变默认值必须使用 `default_factory`。

```python
@dataclass(slots=True)
class SAssetManifestData:
    Records: list[SAssetRecord] = field(default_factory=list)
```

不推荐：

```python
@dataclass
class SAssetManifestData:
    Records: list[SAssetRecord] = []
```

---

### None

使用 `is None` / `is not None`，不要用真值判断代替空值判断。

```python
if Record is None:
    return TResult.Err(MakeError(EErrorCode.AssetNotFound, "asset not found"))
```

不推荐：

```python
if not Record:
    ...
```

除非类型本身明确以真值语义表达状态。

---

### 推导式

简单映射和过滤可以使用推导式；复杂逻辑应拆成循环。

好的做法：

```python
Names = [Record.Name for Record in Records]
```

不好的做法：

```python
Names = [Normalize(Record.Name) for Record in Records if Record.bEnabled and IsValid(Record) and not IsHidden(Record)]
```

复杂条件应展开：

```python
Names: list[String] = []

for Record in Records:
    if not Record.bEnabled:
        continue

    if IsHidden(Record):
        continue

    if not IsValid(Record):
        continue

    Names.append(Normalize(Record.Name))
```

---

## 错误处理

### 可预期错误使用 TResult

项目代码中，可预期的业务错误推荐通过 `TResult[TValue, TError]` 返回，而不是隐藏在异常控制流中。

```python
def ReadTextFile(self, FilePath: Path) -> TResult[String, SError]:
    ...
```

适合使用 `TResult` 的场景：

- 文件不存在、配置无效、资源未找到。
- 用户输入非法。
- 远程请求失败且调用方需要处理。
- 解析失败且需要错误码、上下文路径、行列号等信息。

---

### 异常使用边界

异常适合以下场景：

- 编程错误，例如非法状态、断言失败、违反不变量。
- 第三方库边界，先捕获再转换成 `TResult`。
- 程序启动阶段的致命错误。
- Python 协议要求的异常，例如迭代器结束使用 `StopIteration`。

不要把异常作为普通业务分支的主要表达方式。

---

### TResult 约定

`TResult[TValue, TError]` 表达“成功值或错误值”。默认错误类型推荐为 `SError`。

```python
@dataclass(frozen=True, slots=True)
class SError:
    Code: EErrorCode
    Message: String
    ContextPath: Path | None = None
```

调用方必须处理成功和失败：

```python
ManifestResult = ParseManifest(Text, ManifestPath)

if ManifestResult.IsErr():
    return TResult.Err(ManifestResult.Failure())

Manifest = ManifestResult.Value()
```

不要无理由丢弃 `TResult`。

```python
SaveResult = Config.Save()

if SaveResult.IsErr():
    LogError(FormatError(SaveResult.Failure()))
    return SaveResult
```

如果确实要忽略，应显式说明理由：

```python
_ = Cache.TryWarmUp()  # Best-effort optimization. Failure is non-fatal.
```

---

### TOptional 表达“没有”

当失败只有“有或没有”，且不需要错误原因时，使用 `TOptional[T]` 或 `T | None`。

```python
def FindAsset(self, Name: String) -> SAssetRecord | None:
    return self._RecordsByName.get(Name)
```

如果找不到不是错误，只是查询结果为空，`None` 更合适。如果调用方需要知道为什么失败，使用 `TResult`。

---

### 错误信息

错误类型应包含机器可读的错误码和人类可读的信息。

```python
class EErrorCode(Enum):
    Unknown = auto()
    FileNotFound = auto()
    InvalidManifest = auto()
    DuplicateAsset = auto()
    AssetNotFound = auto()
    NotOpen = auto()

@dataclass(frozen=True, slots=True)
class SError:
    Code: EErrorCode
    Message: String
    ContextPath: Path | None = None
```

---

## 资源管理与上下文管理器

### 使用 with 管理资源

需要成对打开和关闭的资源应支持上下文管理器。

```python
class ZFileHandle:
    def __init__(self, FilePath: Path) -> None:
        self._FilePath = FilePath
        self._Handle: TextIO | None = None

    def __enter__(self) -> "ZFileHandle":
        self.Open()
        return self

    def __exit__(self, ExcType, ExcValue, Traceback) -> None:
        self.Close()
```

使用时：

```python
with ZFileHandle(ConfigPath) as FileHandle:
    Text = FileHandle.ReadText()
```

---

### 所有权与生命周期

Python 没有 C++ 式所有权系统，但仍应在 API 中表达生命周期和引用关系。

```python
class ZRenderer:
    def __init__(self, FileSystem: IFileSystem) -> None:
        self._FileSystem = FileSystem  # Non-owning dependency.
```

如果对象保存外部可变对象引用，应在文档中说明。必要时复制输入，避免外部修改破坏不变量。

```python
class ZAssetManifest:
    def __init__(self, Records: Iterable[SAssetRecord]) -> None:
        self._RecordsByName = {Record.Name: Record for Record in Records}
```

---

### 避免全局可变状态

全局可变状态会降低可测试性和可推理性。优先使用依赖注入。

```python
class ZAssetDatabase:
    def __init__(self, FileSystem: IFileSystem) -> None:
        self._FileSystem = FileSystem
```

不推荐：

```python
GlobalFileSystem = ZNativeFileSystem()

class ZAssetDatabase:
    def Open(self, ManifestPath: Path) -> TResult[None, SError]:
        return GlobalFileSystem.ReadTextFile(ManifestPath)
```

---

## 泛型、协议与类型标注

### 公共 API 必须标注类型

公共函数、公共方法、数据类字段必须写类型标注。

```python
def ResolveAssetPath(self, Name: String) -> TResult[Path, SError]:
    ...
```

内部局部变量在类型明显时可以省略标注；类型不明显时应显式标注。

```python
RecordsByName: dict[String, SAssetRecord] = {}
```

---

### TypeVar 命名

类型变量使用 `T` 前缀，并带语义名。

好的做法：

```python
TValue = TypeVar("TValue")
TError = TypeVar("TError")
TKey = TypeVar("TKey")
```

不好的做法：

```python
T = TypeVar("T")
K = TypeVar("K")
V = TypeVar("V")
```

`TValue`、`TError`、`TKey` 比单字母更清楚。

---

### Protocol 与 ABC

纯接口优先使用 `Protocol`，适合结构化类型检查。

```python
class IFileSystem(Protocol):
    def ReadTextFile(self, FilePath: Path) -> TResult[String, SError]:
        ...
```

需要运行时注册、共享部分实现或强制继承时，可以使用 `ABC`。

```python
class IPlugin(ABC):
    @abstractmethod
    def Load(self) -> TResult[None, SError]:
        ...
```

---

### C 前缀约束协议

`C` 前缀用于表达泛型约束或能力约束，不表示业务接口。

```python
class CSerializable(Protocol):
    def ToJson(self) -> dict[str, object]:
        ...

TSerializable = TypeVar("TSerializable", bound=CSerializable)
```

`IFileSystem` 是依赖边界；`CSerializable` 是能力约束。二者语义不同。

---

### 类型别名

基础值类型别名不加 `Z` / `S` / `T`。

```python
String: TypeAlias = str
Bytes: TypeAlias = bytes
JsonObject: TypeAlias = dict[str, object]
```

泛型别名使用 `T` 前缀。

```python
TValue = TypeVar("TValue")
TKey = TypeVar("TKey")

TList: TypeAlias = list[TValue]
THashMap: TypeAlias = dict[TKey, TValue]
```

Python 3.12+ 可以写成：

```python
type TList[TValue] = list[TValue]
type THashMap[TKey, TValue] = dict[TKey, TValue]
```

---

## 注释与文档

### 注释解释原因，不重复代码

好的做法：

```python
# Keep the old manifest active until parsing succeeds so Reload() is atomic.
NewManifest = ManifestResult.Value()
```

不好的做法：

```python
# Set bOpen to True.
self._bOpen = True
```

注释应解释“为什么”，而不是复述“做了什么”。

---

### 公共 API 文档

复杂公共 API 应说明失败条件、副作用、所有权、线程安全和返回值语义。

```python
def Open(self, ManifestPath: Path) -> TResult[None, SError]:
    """Open and parse an asset manifest.

    Returns:
        Ok(None) on success.
        Err(FileNotFound) if the file system cannot read ManifestPath.
        Err(InvalidManifest) if the manifest syntax is invalid.

    On failure, the previous open manifest remains unchanged.
    """
```

文档字符串内容可以使用英文或中文，但同一项目应保持一致。若项目计划公开，推荐英文。

---

### TODO 注释

`TODO` 应带上下文，不要留下无法行动的短句。

```python
# TODO(Zero): Cache parsed manifests after hot-reload behavior is finalized.
```

不推荐：

```python
# TODO: fix
```

---

## 测试代码风格

测试代码可以比生产代码更直接，但仍应遵守命名、错误处理和格式化规则。

### pytest

如果使用 `pytest` 且希望测试名也保持 PascalCase，建议配置收集规则。

```toml
[tool.pytest.ini_options]
python_files = ["Test*.py"]
python_functions = ["Test*"]
python_classes = ["Z*Test"]
```

测试示例：

```python
class ZAssetDatabaseTest:
    def TestOpenReturnsInvalidManifestForMalformedLine(self) -> None:
        FileSystem = ZMemoryFileSystem()
        FileSystem.AddTextFile(Path("Assets.manifest"), "BrokenLine")

        Database = ZAssetDatabase(FileSystem)
        OpenResult = Database.Open(Path("Assets.manifest"))

        assert OpenResult.IsErr()
        assert OpenResult.Failure().Code == EErrorCode.InvalidManifest
```

如果不配置 pytest，必须使用其默认 `test_` 命名才能自动收集。这属于工具边界例外。

---

### 测试名

测试名应描述行为和预期。

好的做法：

```python
def TestResolveAssetPathReturnsNotOpenWhenDatabaseIsClosed() -> None:
    ...
```

不好的做法：

```python
def TestBad() -> None:
    ...
```

---

## 工具链建议

### 格式化

推荐使用 Black 或 Ruff Formatter，行宽设为 100。格式化工具不会改变标识符命名，因此可以与 PascalCase 风格共存。

```toml
[tool.black]
line-length = 100
target-version = ["py311"]
```

或：

```toml
[tool.ruff]
line-length = 100
target-version = "py311"

[tool.ruff.format]
quote-style = "double"
indent-style = "space"
```

---

### Lint

推荐使用 Ruff。由于本规范有意使用 PascalCase 函数和变量，不建议启用 `pep8-naming`，或需要忽略相关命名规则。

```toml
[tool.ruff]
line-length = 100
target-version = "py311"

[tool.ruff.lint]
select = [
    "E",    # pycodestyle errors
    "F",    # pyflakes
    "I",    # import sorting
    "B",    # bugbear
    "UP",   # pyupgrade
    "SIM",  # simplify
    "C4",   # comprehensions
    "RUF",  # ruff-specific rules
]
ignore = [
    "E501",  # line length handled by formatter
]
```

如果启用 `pep8-naming`，需要忽略与 PascalCase 冲突的规则：

```toml
[tool.ruff.lint]
extend-select = ["N"]
extend-ignore = [
    "N802",  # function name should be lowercase
    "N803",  # argument name should be lowercase
    "N806",  # variable in function should be lowercase
    "N815",  # mixedCase variable in class scope
]
```

---

### 类型检查

推荐使用 Pyright 或 Mypy。公共 API 应尽量通过严格类型检查。

Pyright 示例：

```json
{
    "typeCheckingMode": "strict",
    "pythonVersion": "3.11",
    "reportMissingTypeStubs": "warning"
}
```

Mypy 示例：

```toml
[tool.mypy]
python_version = "3.11"
strict = true
warn_unused_ignores = true
warn_return_any = true
disallow_untyped_defs = true
```

---

## 完整示例

下面的示例展示本风格的核心约定：

- `Z` class 前缀。
- `S` 数据类前缀。
- `T` 泛型类型、类型变量和类型别名前缀。
- `I` 接口前缀。
- `E` 枚举前缀。
- `C` 约束协议前缀。
- `b` 布尔前缀。
- `_` 非公开成员前缀。
- `TResult[T]` 显式错误处理。
- 标准库类型只做 alias，不做无语义封装。

```python
from __future__ import annotations

from dataclasses import dataclass
from enum import Enum, auto
from pathlib import Path
from typing import Generic, Protocol, TypeAlias, TypeVar

String: TypeAlias = str

TValue = TypeVar("TValue")
TError = TypeVar("TError")


class EErrorCode(Enum):
    Unknown = auto()
    FileNotFound = auto()
    InvalidManifest = auto()
    DuplicateAsset = auto()
    AssetNotFound = auto()
    NotOpen = auto()


@dataclass(frozen=True, slots=True)
class SError:
    Code: EErrorCode
    Message: String
    ContextPath: Path | None = None


class TResult(Generic[TValue, TError]):
    def __init__(
        self,
        Value: TValue | None = None,
        Error: TError | None = None,
        bOk: bool = True,
    ) -> None:
        self._Value = Value
        self._Error = Error
        self._bOk = bOk

    @staticmethod
    def Ok(Value: TValue) -> "TResult[TValue, TError]":
        return TResult(Value=Value, bOk=True)

    @staticmethod
    def Err(Error: TError) -> "TResult[TValue, TError]":
        return TResult(Error=Error, bOk=False)

    def IsOk(self) -> bool:
        return self._bOk

    def IsErr(self) -> bool:
        return not self._bOk

    def Value(self) -> TValue:
        if not self._bOk:
            raise RuntimeError("cannot read Value from Err result")
        return self._Value  # type: ignore[return-value]

    def Failure(self) -> TError:
        if self._bOk:
            raise RuntimeError("cannot read Failure from Ok result")
        return self._Error  # type: ignore[return-value]


def MakeError(
    Code: EErrorCode,
    Message: String,
    ContextPath: Path | None = None,
) -> SError:
    return SError(Code=Code, Message=Message, ContextPath=ContextPath)


@dataclass(slots=True)
class SAssetRecord:
    Name: String
    RelativePath: Path
    bPreload: bool = False


class IFileSystem(Protocol):
    def ReadTextFile(self, FilePath: Path) -> TResult[String, SError]:
        ...


class ZAssetManifest:
    def __init__(self) -> None:
        self._RecordsByName: dict[String, SAssetRecord] = {}

    def ContainsAsset(self, Name: String) -> bool:
        return Name in self._RecordsByName

    def FindAsset(self, Name: String) -> SAssetRecord | None:
        return self._RecordsByName.get(Name)

    def AddRecord(self, Record: SAssetRecord) -> TResult[None, SError]:
        if not Record.Name:
            return TResult.Err(
                MakeError(
                    EErrorCode.InvalidManifest,
                    "asset name must not be empty",
                )
            )

        if Record.Name in self._RecordsByName:
            return TResult.Err(
                MakeError(
                    EErrorCode.DuplicateAsset,
                    f"duplicate asset name: {Record.Name}",
                )
            )

        self._RecordsByName[Record.Name] = Record
        return TResult.Ok(None)


class ZAssetDatabase:
    def __init__(self, FileSystem: IFileSystem) -> None:
        self._FileSystem = FileSystem
        self._CurrentManifestPath: Path | None = None
        self._Manifest = ZAssetManifest()
        self._bOpen = False

    def IsOpen(self) -> bool:
        return self._bOpen

    def Open(self, ManifestPath: Path) -> TResult[None, SError]:
        TextResult = self._FileSystem.ReadTextFile(ManifestPath)

        if TextResult.IsErr():
            return TResult.Err(TextResult.Failure())

        ManifestResult = ParseManifest(TextResult.Value(), ManifestPath)

        if ManifestResult.IsErr():
            return TResult.Err(ManifestResult.Failure())

        self._CurrentManifestPath = ManifestPath
        self._Manifest = ManifestResult.Value()
        self._bOpen = True
        return TResult.Ok(None)

    def ResolveAssetPath(self, Name: String) -> TResult[Path, SError]:
        if not self._bOpen or self._CurrentManifestPath is None:
            return TResult.Err(MakeError(EErrorCode.NotOpen, "database is not open"))

        Record = self._Manifest.FindAsset(Name)

        if Record is None:
            return TResult.Err(
                MakeError(
                    EErrorCode.AssetNotFound,
                    f"asset not found: {Name}",
                    self._CurrentManifestPath,
                )
            )

        return TResult.Ok(self._CurrentManifestPath.parent / Record.RelativePath)


def ParseManifest(Text: String, ManifestPath: Path) -> TResult[ZAssetManifest, SError]:
    Manifest = ZAssetManifest()

    for LineNumber, RawLine in enumerate(Text.splitlines(), start=1):
        Line = RawLine.strip()

        if not Line or Line.startswith("#"):
            continue

        Parts = Line.split("=", maxsplit=1)

        if len(Parts) != 2:
            return TResult.Err(
                MakeError(
                    EErrorCode.InvalidManifest,
                    f"invalid manifest line {LineNumber}: {RawLine}",
                    ManifestPath,
                )
            )

        Name = Parts[0].strip()
        RelativePath = Path(Parts[1].strip())
        AddResult = Manifest.AddRecord(SAssetRecord(Name=Name, RelativePath=RelativePath))

        if AddResult.IsErr():
            return TResult.Err(AddResult.Failure())

    return TResult.Ok(Manifest)
```

---

## 总结

本风格可以概括为：

```text
PascalCase Python
Zero-style semantic prefixes
Python ecosystem boundaries respected
Typed public APIs
Explicit Result for expected failures
Dataclass for plain data
Protocol for dependency boundaries
No meaningless standard-library wrappers
```

最终目标不是“把 Python 写成 C++”，而是在个人项目中获得这些效果：

- 类型、函数、变量在视觉上清晰。
- `class`、数据结构、泛型、接口、枚举、约束协议能从名字直接区分。
- 布尔值、输入输出方向、错误处理有明确提示。
- API 从调用点就能读懂。
- Python 标准库和生态能力照用，不重新发明基础设施。
- 依赖关系清晰，模块容易测试和维护。
