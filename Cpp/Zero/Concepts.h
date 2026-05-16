#pragma once

// =============================================================================
// Zero/Concepts.h — Zero 项目常用 Concepts（C++20）
// =============================================================================

#include <concepts>
#include <filesystem>
#include <functional>
#include <string_view>
#include <type_traits>

#include "Types.h"

namespace Zero {

/// 可隐式转换为 std::string_view 的类型（std::string、const char*、string_view 等）
template <typename T>
concept StringLike = std::convertible_to<T, StringView>;

/// 可构造 std::filesystem::path 的类型
template <typename T>
concept PathLike = std::convertible_to<T, Path>;

/// 可用作无序容器键（需要 std::hash 特化和相等运算符）
template <typename T>
concept Hashable =
    requires(T Val)
    {
        { std::hash<T>{}(Val) } -> std::convertible_to<std::size_t>;
        { Val == Val }          -> std::convertible_to<bool>;
    };

/// 满足 Zero 错误风格的类型（拥有 Code 与 Message 成员）
template <typename T>
concept ErrorType =
    requires(T Err)
    {
        Err.Code;
        { Err.Message } -> StringLike;
    };

}  // namespace Zero
