#pragma once

// =============================================================================
// Zero/Result.h — TResult<TValue, TError>
// =============================================================================
//
// TResult 提供稳定的 PascalCase API。C++23 且 std::expected 可用时，内部使用
// std::expected 存储；否则使用 std::variant 存储。公开行为不依赖后端。

#include "Config.h"
#include "Error.h"
#include "Macros.h"

#include <functional>
#include <type_traits>
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

    template <typename TDefault>
    ZERO_NODISCARD TValue ValueOr(TDefault&& DefaultValue) const&
    {
        if (IsOk())
        {
            return Value();
        }

        return static_cast<TValue>(std::forward<TDefault>(DefaultValue));
    }

    template <typename TDefault>
    ZERO_NODISCARD TValue ValueOr(TDefault&& DefaultValue) &&
    {
        if (IsOk())
        {
            return std::move(*this).TakeValue();
        }

        return static_cast<TValue>(std::forward<TDefault>(DefaultValue));
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

    ZERO_NODISCARD TError TakeFailure() &&
    {
        ZERO_ASSERT(IsErr());

#if ZERO_HAS_EXPECTED
        return std::move(Storage.error());
#else
        return std::move(std::get<1>(Storage));
#endif
    }

    template <typename TFunc>
    ZERO_NODISCARD auto Map(TFunc&& Func) const&
    {
        using TMappedValue = std::invoke_result_t<TFunc, const TValue&>;

        if constexpr (std::is_void_v<TMappedValue>)
        {
            using TResultType = TResult<void, TError>;

            if (IsOk())
            {
                std::invoke(std::forward<TFunc>(Func), Value());
                return TResultType::Ok();
            }

            return TResultType::Err(Failure());
        }
        else
        {
            using TResultType = TResult<std::remove_cvref_t<TMappedValue>, TError>;

            if (IsOk())
            {
                return TResultType::Ok(std::invoke(std::forward<TFunc>(Func), Value()));
            }

            return TResultType::Err(Failure());
        }
    }

    template <typename TFunc>
    ZERO_NODISCARD auto Map(TFunc&& Func) &&
    {
        using TMappedValue = std::invoke_result_t<TFunc, TValue>;

        if constexpr (std::is_void_v<TMappedValue>)
        {
            using TResultType = TResult<void, TError>;

            if (IsOk())
            {
                std::invoke(std::forward<TFunc>(Func), std::move(*this).TakeValue());
                return TResultType::Ok();
            }

            return TResultType::Err(std::move(*this).TakeFailure());
        }
        else
        {
            using TResultType = TResult<std::remove_cvref_t<TMappedValue>, TError>;

            if (IsOk())
            {
                return TResultType::Ok(
                    std::invoke(std::forward<TFunc>(Func), std::move(*this).TakeValue()));
            }

            return TResultType::Err(std::move(*this).TakeFailure());
        }
    }

    template <typename TFunc>
    ZERO_NODISCARD auto AndThen(TFunc&& Func) const&
    {
        using TResultType = std::invoke_result_t<TFunc, const TValue&>;

        if (IsOk())
        {
            return std::invoke(std::forward<TFunc>(Func), Value());
        }

        return TResultType::Err(Failure());
    }

    template <typename TFunc>
    ZERO_NODISCARD auto AndThen(TFunc&& Func) &&
    {
        using TResultType = std::invoke_result_t<TFunc, TValue>;

        if (IsOk())
        {
            return std::invoke(std::forward<TFunc>(Func), std::move(*this).TakeValue());
        }

        return TResultType::Err(std::move(*this).TakeFailure());
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
// TResult<void, TError> — 无返回值成功结果的偏特化
// =============================================================================

template <typename TError>
class TResult<void, TError> final
{
public:
    ZERO_NODISCARD static TResult Ok()
    {
        return TResult(SOkTag{});
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

    void Value() const&
    {
        ZERO_ASSERT(IsOk());
    }

    void TakeValue() &&
    {
        ZERO_ASSERT(IsOk());
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

    ZERO_NODISCARD TError TakeFailure() &&
    {
        ZERO_ASSERT(IsErr());

#if ZERO_HAS_EXPECTED
        return std::move(Storage.error());
#else
        return std::move(std::get<1>(Storage));
#endif
    }

    template <typename TFunc>
    ZERO_NODISCARD auto Map(TFunc&& Func) const&
    {
        using TMappedValue = std::invoke_result_t<TFunc>;

        if constexpr (std::is_void_v<TMappedValue>)
        {
            using TResultType = TResult<void, TError>;

            if (IsOk())
            {
                std::invoke(std::forward<TFunc>(Func));
                return TResultType::Ok();
            }

            return TResultType::Err(Failure());
        }
        else
        {
            using TResultType = TResult<std::remove_cvref_t<TMappedValue>, TError>;

            if (IsOk())
            {
                return TResultType::Ok(std::invoke(std::forward<TFunc>(Func)));
            }

            return TResultType::Err(Failure());
        }
    }

    template <typename TFunc>
    ZERO_NODISCARD auto Map(TFunc&& Func) &&
    {
        using TMappedValue = std::invoke_result_t<TFunc>;

        if constexpr (std::is_void_v<TMappedValue>)
        {
            using TResultType = TResult<void, TError>;

            if (IsOk())
            {
                std::invoke(std::forward<TFunc>(Func));
                return TResultType::Ok();
            }

            return TResultType::Err(std::move(*this).TakeFailure());
        }
        else
        {
            using TResultType = TResult<std::remove_cvref_t<TMappedValue>, TError>;

            if (IsOk())
            {
                return TResultType::Ok(std::invoke(std::forward<TFunc>(Func)));
            }

            return TResultType::Err(std::move(*this).TakeFailure());
        }
    }

    template <typename TFunc>
    ZERO_NODISCARD auto AndThen(TFunc&& Func) const&
    {
        using TResultType = std::invoke_result_t<TFunc>;

        if (IsOk())
        {
            return std::invoke(std::forward<TFunc>(Func));
        }

        return TResultType::Err(Failure());
    }

    template <typename TFunc>
    ZERO_NODISCARD auto AndThen(TFunc&& Func) &&
    {
        using TResultType = std::invoke_result_t<TFunc>;

        if (IsOk())
        {
            return std::invoke(std::forward<TFunc>(Func));
        }

        return TResultType::Err(std::move(*this).TakeFailure());
    }

private:
    struct SOkTag {};
    struct SErrTag {};

    explicit TResult(SOkTag)
#if ZERO_HAS_EXPECTED
        : Storage()
#else
        : Storage(std::in_place_index<0>)
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
    std::expected<void, TError> Storage;
#else
    std::variant<std::monostate, TError> Storage;
#endif
};

}  // namespace Zero
