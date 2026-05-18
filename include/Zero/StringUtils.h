#pragma once

// =============================================================================
// Zero/StringUtils.h — 轻量字符串工具
// =============================================================================

#include "Macros.h"
#include "Types.h"

namespace Zero {

NODISCARD constexpr bool IsAsciiWhitespace(char Char) noexcept
{
    return Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r' ||
           Char == '\f' || Char == '\v';
}

NODISCARD constexpr StringView TrimLeft(StringView Text) noexcept
{
    while (!Text.empty() && IsAsciiWhitespace(Text.front()))
    {
        Text.remove_prefix(1);
    }

    return Text;
}

NODISCARD constexpr StringView TrimRight(StringView Text) noexcept
{
    while (!Text.empty() && IsAsciiWhitespace(Text.back()))
    {
        Text.remove_suffix(1);
    }

    return Text;
}

NODISCARD constexpr StringView Trim(StringView Text) noexcept
{
    return TrimRight(TrimLeft(Text));
}

NODISCARD constexpr bool StartsWith(StringView Text, StringView Prefix) noexcept
{
    return Text.starts_with(Prefix);
}

NODISCARD constexpr bool EndsWith(StringView Text, StringView Suffix) noexcept
{
    return Text.ends_with(Suffix);
}

NODISCARD inline TVector<StringView> SplitView(
    StringView Text,
    char       Delimiter,
    bool       bKeepEmpty = false)
{
    TVector<StringView> Parts;

    while (true)
    {
        const std::size_t Position = Text.find(Delimiter);
        const StringView  Part     = Text.substr(0, Position);

        if (bKeepEmpty || !Part.empty())
        {
            Parts.push_back(Part);
        }

        if (Position == StringView::npos)
        {
            break;
        }

        Text.remove_prefix(Position + 1);
    }

    return Parts;
}

}  // namespace Zero
