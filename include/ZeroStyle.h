#pragma once

// =============================================================================
// ZeroStyle.h — Zero 项目基础工具类型（入口头文件）
// =============================================================================
//
// 单一入口：#include "ZeroStyle.h" 即可获得所有 Zero 工具类型。
// 推荐用法：using namespace Zero;  （仅限 .cpp 文件）
//
// 各子头文件可单独引入：
//   Zero/Config.h    — 编译期配置（ZERO_HAS_CXX23、ZERO_HAS_EXPECTED、ZERO_ASSERT）
//   Zero/Macros.h    — 属性宏（ZERO_NODISCARD、ZERO_NORETURN、ZERO_GUARDED_BY 等）
//   Zero/Types.h     — 基础值类型别名、T 前缀容器与智能指针别名
//   Zero/Error.h     — EErrorCode、SError、MakeError
//   Zero/Result.h    — TResult<T,E>
//   Zero/Concepts.h  — CStringLike、CPathLike、CHashable、CErrorType
//   Zero/Diagnostics.h — ZERO_CHECK、ZERO_ENSURE、ZERO_UNREACHABLE
//   Zero/Scope.h     — TScopeExit、MakeScopeExit
//   Zero/Flags.h     — enum class flags opt-in helpers
//   Zero/StringUtils.h — Trim、StartsWith、SplitView 等字符串工具
//   Zero/PathUtils.h — ToGenericString、HasExtension 等路径工具
//   Zero/Format.h    — ToString(EErrorCode)、FormatError
//
// =============================================================================

#include "Zero/Config.h"
#include "Zero/Macros.h"
#include "Zero/Types.h"
#include "Zero/Error.h"
#include "Zero/Result.h"
#include "Zero/Concepts.h"
#include "Zero/Diagnostics.h"
#include "Zero/Scope.h"
#include "Zero/Flags.h"
#include "Zero/StringUtils.h"
#include "Zero/PathUtils.h"
#include "Zero/Format.h"
