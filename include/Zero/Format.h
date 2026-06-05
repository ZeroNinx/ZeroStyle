#pragma once

// =============================================================================
// Zero/Format.h — 轻量格式化辅助
// =============================================================================

#include "Error.h"
#include "Macros.h"

namespace Zero {

ZERO_NODISCARD inline StdString ToString(EErrorCode Code)
{
    switch (Code)
    {
    case EErrorCode::Unknown:
        return "Unknown";
    case EErrorCode::FileNotFound:
        return "FileNotFound";
    case EErrorCode::FileReadFailed:
        return "FileReadFailed";
    case EErrorCode::FileWriteFailed:
        return "FileWriteFailed";
    case EErrorCode::InvalidArgument:
        return "InvalidArgument";
    case EErrorCode::InvalidManifest:
        return "InvalidManifest";
    case EErrorCode::ParseFailed:
        return "ParseFailed";
    case EErrorCode::NotOpen:
        return "NotOpen";
    case EErrorCode::AlreadyOpen:
        return "AlreadyOpen";
    case EErrorCode::AssetNotFound:
        return "AssetNotFound";
    case EErrorCode::DuplicateAsset:
        return "DuplicateAsset";
    }

    return "Unknown";
}

ZERO_NODISCARD inline StdString FormatError(const SError& Error)
{
    StdString Text = ToString(Error.Code);

    if (!Error.Message.empty())
    {
        Text += ": ";
        Text += Error.Message;
    }

    if (!Error.ContextPath.empty())
    {
        Text += " (";
        Text += Error.ContextPath.string();
        Text += ")";
    }

    return Text;
}

}  // namespace Zero
