# Examples

## Basic Console

`examples/basic-console` 展示默认接入方式：

- 项目公共头 `ProjectCore.h`。
- 选择性转发 `Zero` 类型。
- `NODISCARD TResult<SConfig>` 风格的函数声明。
- `NODISCARD_TYPE` 类型标记。
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
