#pragma once

// =============================================================================
// Zero/Result.h — TResult<TValue, TError> 与 TVoidResult
// =============================================================================
//
// TResult 提供稳定的 PascalCase API。C++23 且 std::expected 可用时，内部使用
// std::expected 存储；否则使用 std::variant 存储。公开行为不依赖后端。

#include "Config.h"
#include "Error.h"
#include "Macros.h"

#include <utility>
#include <variant>

namespace Zero {

// =============================================================================
// TResult<TValue, TError = SError>
// =============================================================================

template <typename TValue, typename TError = SError>
class TResult final
{
public:
    ZERO_NODISCARD static TResult Ok(TValue Value)
    {
        return TResult(SOkTag{}, std::move(Value));
    }

    ZERO_NODISCARD static TResult Err(TError Error)
    {
        return TResult(SErrTag{}, std::move(Error));
    }

    ZERO_NODISCARD bool IsOk() const noexcept
    {
#if ZERO_HAS_EXPECTED
        return Storage.has_value();
#else
        return Storage.index() == 0;
#endif
    }

    ZERO_NODISCARD bool IsErr() const noexcept
    {
        return !IsOk();
    }

    ZERO_NODISCARD explicit operator bool() const noexcept
    {
        return IsOk();
    }

    ZERO_NODISCARD const TValue& Value() const&
    {
        ZERO_ASSERT(IsOk());

#if ZERO_HAS_EXPECTED
        return *Storage;
#else
        return std::get<0>(Storage);
#endif
    }

    ZERO_NODISCARD TValue TakeValue() &&
    {
        ZERO_ASSERT(IsOk());

#if ZERO_HAS_EXPECTED
        return std::move(*Storage);
#else
        return std::move(std::get<0>(Storage));
#endif
    }

    ZERO_NODISCARD const TError& Failure() const&
    {
        ZERO_ASSERT(IsErr());

#if ZERO_HAS_EXPECTED
        return Storage.error();
#else
        return std::get<1>(Storage);
#endif
    }

private:
    struct SOkTag {};
    struct SErrTag {};

    explicit TResult(SOkTag, TValue Value)
#if ZERO_HAS_EXPECTED
        : Storage(std::move(Value))
#else
        : Storage(std::in_place_index<0>, std::move(Value))
#endif
    {
    }

    explicit TResult(SErrTag, TError Error)
#if ZERO_HAS_EXPECTED
        : Storage(std::unexpected(std::move(Error)))
#else
        : Storage(std::in_place_index<1>, std::move(Error))
#endif
    {
    }

#if ZERO_HAS_EXPECTED
    std::expected<TValue, TError> Storage;
#else
    std::variant<TValue, TError> Storage;
#endif
};

// =============================================================================
// TVoidResult — TResult<SUnit, TError> 的别名
// =============================================================================

template <typename TError = SError>
using TVoidResult = TResult<SUnit, TError>;

template <typename TError = SError>
ZERO_NODISCARD TVoidResult<TError> OkVoid()
{
    return TVoidResult<TError>::Ok(SUnit{});
}

}  // namespace Zero
