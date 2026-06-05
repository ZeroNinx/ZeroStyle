# Examples

## Basic Console

`examples/basic-console` 展示默认接入方式：

- 项目公共头 `ProjectCore.h`。
- 选择性转发 `Zero` 类型。
- `ZERO_NODISCARD TResult<SConfig>` 风格的函数声明。
- `ZERO_NODISCARD_TYPE` 类型标记。
- `TResult` 错误返回。

构建：

```bash
cmake --preset dev
cmake --build --preset dev
```

运行 Windows/MSVC Release 示例：

```powershell
.\build\dev\examples\basic-console\Release\ZeroStyle_BasicConsole.exe
```

## Package Consumer

`examples/package-consumer` 展示已安装 ZeroStyle 后，通过 `find_package` 消费：

```bash
cmake --install build/dev --prefix build/install
cmake -S examples/package-consumer -B build/package-consumer -DCMAKE_PREFIX_PATH=build/install
cmake --build build/package-consumer --config Release
```

## String Utilities

`SplitView` 返回的 `StdStringView` 指向输入文本，调用方必须保证输入文本在结果使用期间仍然有效：

```cpp
StdString Text = "a,b,c";
TVector<StdStringView> Parts = SplitView(Text, ',');
```

如果需要安全持有拆分结果，使用 owning 版本 `Split`：

```cpp
TVector<StdString> Parts = Split(StdString("a,b,c"), ',');
```
