#pragma once

// =============================================================================
// Zero/PathUtils.h — 轻量路径工具
// =============================================================================

#include "Macros.h"
#include "Types.h"

namespace Zero {

NODISCARD inline String ToGenericString(const Path& FilePath)
{
    return FilePath.generic_string();
}

NODISCARD inline Path NormalizeLexically(const Path& FilePath)
{
    return FilePath.lexically_normal();
}

NODISCARD inline bool HasExtension(const Path& FilePath, StringView Extension)
{
    const String ActualExtension = FilePath.extension().string();

    if (Extension.empty())
    {
        return ActualExtension.empty();
    }

    if (Extension.front() == '.')
    {
        return ActualExtension == Extension;
    }

    return ActualExtension.size() == Extension.size() + 1 &&
           ActualExtension.front() == '.' &&
           StringView(ActualExtension).substr(1) == Extension;
}

NODISCARD inline Path WithExtension(Path FilePath, StringView Extension)
{
    if (Extension.empty() || Extension.front() == '.')
    {
        FilePath.replace_extension(Extension);
        return FilePath;
    }

    String DottedExtension = ".";
    DottedExtension += Extension;
    FilePath.replace_extension(DottedExtension);
    return FilePath;
}

}  // namespace Zero
