#pragma once

// =============================================================================
// Zero/Result.h — Result<TValue, TError> 与 VoidResult
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
// Result<TValue, TError = Error>
// =============================================================================

#if ZERO_HAS_EXPECTED

template <typename TValue, typename TError = Error>
class Result final : public std::expected<TValue, TError>
{
    using Base = std::expected<TValue, TError>;

public:
    using Base::Base;  // 继承全部 std::expected 构造函数

    // --- 工厂方法 ---

    NODISCARD static Result Ok(TValue InValue)
    {
        return Result(std::move(InValue));
    }

    NODISCARD static Result Err(TError InError)
    {
        return Result(std::unexpected(std::move(InError)));
    }

    // --- PascalCase 兼容 API ---

    NODISCARD bool IsOk()  const noexcept { return Base::has_value(); }
    NODISCARD bool IsErr() const noexcept { return !Base::has_value(); }

    NODISCARD const TValue& Value() const&
    {
        return Base::value();  // 未持值时抛出 std::bad_expected_access
    }

    NODISCARD TValue TakeValue() &&
    {
        return std::move(*this).value();
    }

    NODISCARD const TError& Failure() const&
    {
        return Base::error();  // 未持错误时行为未定义
    }
};

#else  // std::expected 不可用时的回退实现

template <typename TValue, typename TError = Error>
class Result final
{
public:
    // --- 工厂方法 ---

    NODISCARD static Result Ok(TValue InValue)
    {
        return Result(OkTag{}, std::move(InValue));
    }

    NODISCARD static Result Err(TError InError)
    {
        return Result(ErrTag{}, std::move(InError));
    }

    // --- 查询 ---

    NODISCARD bool IsOk()  const noexcept { return Storage.index() == 0; }
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
    struct OkTag  {};
    struct ErrTag {};

    explicit Result(OkTag, TValue InValue)
        : Storage(std::in_place_index<0>, std::move(InValue))
    {
    }

    explicit Result(ErrTag, TError InError)
        : Storage(std::in_place_index<1>, std::move(InError))
    {
    }

    std::variant<TValue, TError> Storage;
};

#endif  // ZERO_HAS_EXPECTED

// =============================================================================
// VoidResult — Result<Unit, TError> 的别名
// =============================================================================

template <typename TError = Error>
using VoidResult = Result<Unit, TError>;

}  // namespace Zero
