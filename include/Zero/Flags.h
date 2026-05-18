#pragma once

// =============================================================================
// Zero/Flags.h — enum class 位运算辅助
// =============================================================================
//
// 使用方式：
//
//   namespace Project {
//   enum class EFlags : uint32 { A = 1 << 0, B = 1 << 1 };
//   ZERO_ENABLE_ENUM_FLAGS(EFlags);
//   }
//
// 只有显式 opt-in 的 enum 才会启用位运算符。

#include "Macros.h"

#include <concepts>
#include <type_traits>

namespace Zero {

template <typename TEnum>
concept CEnumFlags =
    std::is_enum_v<TEnum> &&
    requires(TEnum Value)
    {
        { ZeroEnableEnumFlags(Value) } -> std::same_as<std::true_type>;
    };

template <typename TEnum>
    requires std::is_enum_v<TEnum>
NODISCARD constexpr auto ToUnderlying(TEnum Value) noexcept
{
    return static_cast<std::underlying_type_t<TEnum>>(Value);
}

template <CEnumFlags TEnum>
NODISCARD constexpr bool HasAnyFlags(TEnum Value, TEnum Flags) noexcept
{
    return (ToUnderlying(Value) & ToUnderlying(Flags)) != 0;
}

template <CEnumFlags TEnum>
NODISCARD constexpr bool HasAllFlags(TEnum Value, TEnum Flags) noexcept
{
    return (ToUnderlying(Value) & ToUnderlying(Flags)) == ToUnderlying(Flags);
}

}  // namespace Zero

#define ZERO_ENABLE_ENUM_FLAGS(EnumType)                      \
    NODISCARD constexpr std::true_type ZeroEnableEnumFlags(   \
        EnumType) noexcept                                    \
    {                                                         \
        return {};                                            \
    }

template <Zero::CEnumFlags TEnum>
NODISCARD constexpr TEnum operator|(TEnum Lhs, TEnum Rhs) noexcept
{
    return static_cast<TEnum>(Zero::ToUnderlying(Lhs) | Zero::ToUnderlying(Rhs));
}

template <Zero::CEnumFlags TEnum>
NODISCARD constexpr TEnum operator&(TEnum Lhs, TEnum Rhs) noexcept
{
    return static_cast<TEnum>(Zero::ToUnderlying(Lhs) & Zero::ToUnderlying(Rhs));
}

template <Zero::CEnumFlags TEnum>
NODISCARD constexpr TEnum operator^(TEnum Lhs, TEnum Rhs) noexcept
{
    return static_cast<TEnum>(Zero::ToUnderlying(Lhs) ^ Zero::ToUnderlying(Rhs));
}

template <Zero::CEnumFlags TEnum>
NODISCARD constexpr TEnum operator~(TEnum Value) noexcept
{
    return static_cast<TEnum>(~Zero::ToUnderlying(Value));
}

template <Zero::CEnumFlags TEnum>
constexpr TEnum& operator|=(TEnum& Lhs, TEnum Rhs) noexcept
{
    Lhs = Lhs | Rhs;
    return Lhs;
}

template <Zero::CEnumFlags TEnum>
constexpr TEnum& operator&=(TEnum& Lhs, TEnum Rhs) noexcept
{
    Lhs = Lhs & Rhs;
    return Lhs;
}

template <Zero::CEnumFlags TEnum>
constexpr TEnum& operator^=(TEnum& Lhs, TEnum Rhs) noexcept
{
    Lhs = Lhs ^ Rhs;
    return Lhs;
}
