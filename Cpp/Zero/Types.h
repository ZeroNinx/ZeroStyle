#pragma once

// =============================================================================
// Zero/Types.h — 基础类型别名
// =============================================================================
//
// 整数类型、浮点别名、字符串/路径别名、容器与智能指针别名。
// 均在 Zero:: 命名空间内定义。

#include <array>
#include <cstdint>
#include <deque>
#include <filesystem>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Zero {

// --- 整数 ---

using int8    = std::int8_t;
using int16   = std::int16_t;
using int32   = std::int32_t;
using int64   = std::int64_t;

using uint8   = std::uint8_t;
using uint16  = std::uint16_t;
using uint32  = std::uint32_t;
using uint64  = std::uint64_t;

using float32 = float;
using float64 = double;

// --- 字符串 / 路径 ---

using String      = std::string;
using StringView  = std::string_view;
using WString     = std::wstring;
using WStringView = std::wstring_view;
using Path        = std::filesystem::path;

// --- 智能指针 / Optional ---

template <typename T>
using Optional  = std::optional<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using WeakPtr   = std::weak_ptr<T>;

// --- 序列容器 ---

template <typename T>
using Vector = std::vector<T>;

template <typename T, std::size_t N>
using Array  = std::array<T, N>;

template <typename T>
using Deque  = std::deque<T>;

template <typename T>
using List   = std::list<T>;

template <typename T>
using Queue  = std::queue<T>;

template <typename T>
using Stack  = std::stack<T>;

template <typename TFirst, typename TSecond>
using Pair   = std::pair<TFirst, TSecond>;

// --- 关联容器 ---

template <typename TKey, typename TValue,
          typename TCompare = std::less<TKey>>
using Map = std::map<TKey, TValue, TCompare>;

template <typename TKey, typename TValue,
          typename THash  = std::hash<TKey>,
          typename TEqual = std::equal_to<TKey>>
using HashMap = std::unordered_map<TKey, TValue, THash, TEqual>;

template <typename T,
          typename TCompare = std::less<T>>
using Set = std::set<T, TCompare>;

template <typename T,
          typename THash  = std::hash<T>,
          typename TEqual = std::equal_to<T>>
using HashSet = std::unordered_set<T, THash, TEqual>;

}  // namespace Zero
