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
//   Zero/Macros.h    — 属性宏（NODISCARD、GUARDED_BY 等线程安全注解）
//   Zero/Types.h     — 整数别名、字符串/路径别名、容器与智能指针别名
//   Zero/Error.h     — Unit、ErrorCode、Error、MakeError
//   Zero/Result.h    — Result<T,E>、VoidResult
//   Zero/Concepts.h  — StringLike、PathLike、Hashable、ErrorType
//
// =============================================================================

#include "Zero/Config.h"
#include "Zero/Macros.h"
#include "Zero/Types.h"
#include "Zero/Error.h"
#include "Zero/Result.h"
#include "Zero/Concepts.h"
