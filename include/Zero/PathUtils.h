#pragma once

// =============================================================================
// Zero/PathUtils.h — 轻量路径工具
// =============================================================================

#include "Macros.h"
#include "Types.h"

namespace Zero {

ZERO_NODISCARD inline StdString ToGenericString(const StdPath& FilePath)
{
    return FilePath.generic_string();
}

ZERO_NODISCARD inline StdPath NormalizeLexically(const StdPath& FilePath)
{
    return FilePath.lexically_normal();
}

ZERO_NODISCARD inline bool HasExtension(const StdPath& FilePath, StdStringView Extension)
{
    const StdString ActualExtension = FilePath.extension().string();

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
           StdStringView(ActualExtension).substr(1) == Extension;
}

ZERO_NODISCARD inline StdPath WithExtension(StdPath FilePath, StdStringView Extension)
{
    if (Extension.empty() || Extension.front() == '.')
    {
        FilePath.replace_extension(Extension);
        return FilePath;
    }

    StdString DottedExtension = ".";
    DottedExtension += Extension;
    FilePath.replace_extension(DottedExtension);
    return FilePath;
}

}  // namespace Zero
