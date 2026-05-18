# Error Handling

ZeroStyle 推荐用显式返回值表达可恢复错误。

## TResult

当失败需要错误原因时，使用 `TResult<T>`：

```cpp
NODISCARD TResult<SConfig> ParseConfig(String Text);
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

## TVoidResult

当成功不需要返回值，但失败需要错误原因时，使用 `TVoidResult<>`：

```cpp
NODISCARD TVoidResult<> SaveConfig(const SConfig& Config);
```

## TOptional

当结果只有“有或没有”，且没有错误原因时，使用 `TOptional<T>`：

```cpp
NODISCARD TOptional<SAssetRecord> FindAsset(StringView Name) const;
```

找不到不一定是错误。需要错误原因时再使用 `TResult<T>`。

## Exception Policy

ZeroStyle core 不禁止异常，但不把异常作为主要错误通道。推荐：

- 可恢复业务错误：`TResult<T>`。
- 查询为空：`TOptional<T>`。
- 违反前置条件或不可恢复错误：断言、check 或异常，由项目自行决定。
