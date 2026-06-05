#pragma once

// Example project public header.
//
// A real project can rename this file to ProjectCore.h, GameCore.h, AppCore.h,
// or any project-specific public entry. Keep the using declarations explicit so
// public headers do not leak the whole Zero namespace.

#include "ZeroStyle.h"

namespace Example {

using Zero::EErrorCode;
using Zero::MakeError;
using Zero::StdPath;
using Zero::SError;
using Zero::StdString;
using Zero::StdStringView;
using Zero::THashMap;
using Zero::TOptional;
using Zero::TResult;
using Zero::TVector;
using Zero::int32;
using Zero::uint32;

}  // namespace Example
