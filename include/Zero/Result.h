#pragma once

// =============================================================================
// Zero/Result.h — TResult<TValue, TError> 与 TVoidResult
// =============================================================================
//
// std::expected 可用时：继承 std::expected<TValue, TError>。
//   — 原生方法（has_value()、value()、error()、operator bool、operator*）可用。
//   — PascalCase 方法（IsOk()、Value()、Failure()、TakeValue()）作为兼容层。
//
// 回退实现：基于 std::variant，提供相同的 PascalCase API。

#include "Config.h"
#include "Error.h"
#include "Macros.h"

#include <variant>

namespace Zero {

// =============================================================================
// TResult<TValue, TError = SError>
// =============================================================================

#if ZERO_HAS_EXPECTED

template <typename TValue, typename TError = SError>
class TResult final : public std::expected<TValue, TError>
{
    using TBase = std::expected<TValue, TError>;

public:
    using TBase::TBase;  // 继承全部 std::expected 构造函数

    // --- 工厂方法 ---

    NODISCARD static TResult Ok(TValue Value)
    {
        return TResult(std::move(Value));
    }

    NODISCARD static TResult Err(TError Error)
    {
        return TResult(std::unexpected(std::move(Error)));
    }

    // --- PascalCase 兼容 API ---

    NODISCARD bool IsOk() const noexcept { return TBase::has_value(); }
    NODISCARD bool IsErr() const noexcept { return !TBase::has_value(); }

    NODISCARD const TValue& Value() const&
    {
        return TBase::value();  // 未持值时抛出 std::bad_expected_access
    }

    NODISCARD TValue TakeValue() &&
    {
        return std::move(*this).value();
    }

    NODISCARD const TError& Failure() const&
    {
        return TBase::error();  // 未持错误时行为未定义
    }
};

#else  // std::expected 不可用时的回退实现

template <typename TValue, typename TError = SError>
class TResult final
{
public:
    // --- 工厂方法 ---

    NODISCARD static TResult Ok(TValue Value)
    {
        return TResult(SOkTag{}, std::move(Value));
    }

    NODISCARD static TResult Err(TError Error)
    {
        return TResult(SErrTag{}, std::move(Error));
    }

    // --- 查询 ---

    NODISCARD bool IsOk() const noexcept { return Storage.index() == 0; }
    NODISCARD bool IsErr() const noexcept { return Storage.index() == 1; }

    NODISCARD explicit operator bool() const noexcept { return IsOk(); }

    // --- 值访问 ---

    NODISCARD const TValue& Value() const&
    {
        ZERO_ASSERT(IsOk());
        return std::get<0>(Storage);
    }

    NODISCARD TValue TakeValue() &&
    {
        ZERO_ASSERT(IsOk());
        return std::move(std::get<0>(Storage));
    }

    NODISCARD const TError& Failure() const&
    {
        ZERO_ASSERT(IsErr());
        return std::get<1>(Storage);
    }

private:
    struct SOkTag {};
    struct SErrTag {};

    explicit TResult(SOkTag, TValue Value)
        : Storage(std::in_place_index<0>, std::move(Value))
    {
    }

    explicit TResult(SErrTag, TError Error)
        : Storage(std::in_place_index<1>, std::move(Error))
    {
    }

    std::variant<TValue, TError> Storage;
};

#endif  // ZERO_HAS_EXPECTED

// =============================================================================
// TVoidResult — TResult<SUnit, TError> 的别名
// =============================================================================

template <typename TError = SError>
using TVoidResult = TResult<SUnit, TError>;

}  // namespace Zero
