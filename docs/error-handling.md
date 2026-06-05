# Error Handling

ZeroStyle 推荐用显式返回值表达可恢复错误。`TResult` 的公开 API 在 C++20 fallback 与 C++23 `std::expected` 后端下保持一致；C++23 只影响内部存储实现。

## TResult

当失败需要错误原因时，使用 `TResult<T>`：

```cpp
ZERO_NODISCARD TResult<SConfig> ParseConfig(StdString Text);
```

调用方必须检查结果：

```cpp
auto ConfigResult = ParseConfig(Text);

if (ConfigResult.IsErr())
{
    return ConfigResult.Failure();
}

SConfig Config = std::move(ConfigResult).TakeValue();
```

简单默认值可以使用 `ValueOr`。需要转换成功值时使用 `Map`；需要串联另一个返回 `TResult` 的操作时使用 `AndThen`：

```cpp
int32 Port = ParsePort(Text).ValueOr(8080);

auto NormalizedConfig = ParseConfig(Text).Map([](SConfig Config) {
    Config.Name = Trim(Config.Name);
    return Config;
});

auto LoadedConfig = ReadTextFile(ConfigPath).AndThen(ParseConfig);
```

`AndThen` 的回调应返回使用同一错误类型的 `TResult`，错误路径只负责透传，不做错误类型转换。

## TResult<void>

当成功不需要返回值，但失败需要错误原因时，使用 `TResult<void>`：

```cpp
ZERO_NODISCARD TResult<void> SaveConfig(const SConfig& Config);
```

成功返回使用 `TResult<void>::Ok()`：

```cpp
TResult<void> SaveConfig(const SConfig& Config)
{
    return TResult<void>::Ok();
}
```

## TOptional

当结果只有“有或没有”，且没有错误原因时，使用 `TOptional<T>`：

```cpp
ZERO_NODISCARD TOptional<SAssetRecord> FindAsset(StdStringView Name) const;
```

找不到不一定是错误。需要错误原因时再使用 `TResult<T>`。

## Exception Policy

ZeroStyle core 不禁止异常，但不把异常作为主要错误通道。推荐：

- 可恢复业务错误：`TResult<T>`。
- 查询为空：`TOptional<T>`。
- 违反前置条件或不可恢复错误：断言、check 或异常，由项目自行决定。
