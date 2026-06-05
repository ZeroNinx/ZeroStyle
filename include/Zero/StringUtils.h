#pragma once

// =============================================================================
// Zero/StringUtils.h — 轻量字符串工具
// =============================================================================

#include "Macros.h"
#include "Types.h"

namespace Zero {

ZERO_NODISCARD constexpr bool IsAsciiWhitespace(char Char) noexcept
{
    return Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r' ||
           Char == '\f' || Char == '\v';
}

ZERO_NODISCARD constexpr StdStringView TrimLeft(StdStringView Text) noexcept
{
    while (!Text.empty() && IsAsciiWhitespace(Text.front()))
    {
        Text.remove_prefix(1);
    }

    return Text;
}

ZERO_NODISCARD constexpr StdStringView TrimRight(StdStringView Text) noexcept
{
    while (!Text.empty() && IsAsciiWhitespace(Text.back()))
    {
        Text.remove_suffix(1);
    }

    return Text;
}

ZERO_NODISCARD constexpr StdStringView Trim(StdStringView Text) noexcept
{
    return TrimRight(TrimLeft(Text));
}

ZERO_NODISCARD constexpr bool StartsWith(StdStringView Text, StdStringView Prefix) noexcept
{
    return Text.starts_with(Prefix);
}

ZERO_NODISCARD constexpr bool EndsWith(StdStringView Text, StdStringView Suffix) noexcept
{
    return Text.ends_with(Suffix);
}

// 返回的 StdStringView 指向输入 Text。
// 调用方必须保证 Text 指向的数据在返回结果使用期间仍然有效。
ZERO_NODISCARD inline TVector<StdStringView> SplitView(
    StdStringView Text,
    char          Delimiter,
    bool          bKeepEmpty = false)
{
    TVector<StdStringView> Parts;

    while (true)
    {
        const std::size_t  Position = Text.find(Delimiter);
        const StdStringView Part     = Text.substr(0, Position);

        if (bKeepEmpty || !Part.empty())
        {
            Parts.push_back(Part);
        }

        if (Position == StdStringView::npos)
        {
            break;
        }

        Text.remove_prefix(Position + 1);
    }

    return Parts;
}

ZERO_NODISCARD inline TVector<StdString> Split(
    StdStringView Text,
    char          Delimiter,
    bool          bKeepEmpty = false)
{
    TVector<StdString> Parts;

    while (true)
    {
        const std::size_t  Position = Text.find(Delimiter);
        const StdStringView Part     = Text.substr(0, Position);

        if (bKeepEmpty || !Part.empty())
        {
            Parts.emplace_back(Part);
        }

        if (Position == StdStringView::npos)
        {
            break;
        }

        Text.remove_prefix(Position + 1);
    }

    return Parts;
}

}  // namespace Zero
