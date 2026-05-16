#pragma once

// =============================================================================
// Zero/Error.h — 结构化错误类型
// =============================================================================
//
// 提供 Unit、ErrorCode、Error 和 MakeError。
// 与 Result.h 配合使用。

#include <filesystem>
#include <string>

#include "Macros.h"
#include "Types.h"

namespace Zero {

// =============================================================================
// Unit — 表示"无值成功"，用于 VoidResult
// =============================================================================

struct Unit final
{
    constexpr bool operator==(const Unit&) const noexcept { return true; }
};

// =============================================================================
// ErrorCode — 枚举所有已知错误类别
// =============================================================================

enum class ErrorCode
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
// Error — 携带 Code、Message、Path 的错误值
// =============================================================================

struct Error final
{
    ErrorCode  Code    = ErrorCode::Unknown;
    String     Message;
    Path       Path;
};

NODISCARD inline Error MakeError(
    ErrorCode  InCode,
    String     InMessage,
    Path       InPath = {})
{
    return Error{
        .Code    = InCode,
        .Message = std::move(InMessage),
        .Path    = std::move(InPath),
    };
}

}  // namespace Zero
