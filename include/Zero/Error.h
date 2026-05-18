#pragma once

// =============================================================================
// Zero/Error.h — 结构化错误类型
// =============================================================================
//
// 提供 SUnit、EErrorCode、SError 和 MakeError。
// 与 TResult.h / Result.h 配合使用。

#include <filesystem>
#include <string>
#include <utility>

#include "Macros.h"
#include "Types.h"

namespace Zero {

// =============================================================================
// SUnit — 表示“无值成功”，用于 TVoidResult
// =============================================================================

struct SUnit
{
    constexpr bool operator==(const SUnit&) const noexcept { return true; }
};

// =============================================================================
// EErrorCode — 枚举所有已知错误类别
// =============================================================================

enum class EErrorCode
{
    Unknown,
    // 文件系统
    FileNotFound,
    FileReadFailed,
    FileWriteFailed,
    // 数据 / 格式
    InvalidArgument,
    InvalidManifest,
    ParseFailed,
    // 状态
    NotOpen,
    AlreadyOpen,
    // 资产
    AssetNotFound,
    DuplicateAsset,
};

// =============================================================================
// SError — 携带 Code、Message、ContextPath 的错误值
// =============================================================================

struct SError
{
    EErrorCode Code = EErrorCode::Unknown;
    String     Message;
    Path       ContextPath;
};

ZERO_NODISCARD inline SError MakeError(
    EErrorCode Code,
    String     Message,
    Path       ContextPath = {})
{
    return SError{
        .Code        = Code,
        .Message     = std::move(Message),
        .ContextPath = std::move(ContextPath),
    };
}

}  // namespace Zero
