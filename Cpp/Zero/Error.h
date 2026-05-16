#pragma once

// =============================================================================
// Zero/Error.h — 结构化错误类型
// =============================================================================
//
// 提供 Unit、ErrorCode、Error 和 MakeError。
// 与 Result.h 配合使用。

#include <filesystem>
#include <string>
#include <utility>

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
// Error — 携带 Code、Message、ContextPath 的错误值
// =============================================================================

struct Error final
{
    ErrorCode Code = ErrorCode::Unknown;
    String    Message;
    Path      ContextPath;
};

NODISCARD inline Error MakeError(
    ErrorCode InCode,
    String    InMessage,
    Path      InContextPath = {})
{
    return Error{
        .Code        = InCode,
        .Message     = std::move(InMessage),
        .ContextPath = std::move(InContextPath),
    };
}

}  // namespace Zero
