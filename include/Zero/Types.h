#pragma once

// =============================================================================
// Zero/Types.h — 基础类型别名
// =============================================================================
//
// 仅提供标准库类型的 PascalCase / T 前缀别名，不做无语义封装。
// 基础值类型别名不加 Z/S/T；模板类型别名使用 T 前缀。

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

// --- 整数 / 浮点 ---

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

template <typename TValue>
using TOptional = std::optional<TValue>;

template <typename TValue, typename TDeleter = std::default_delete<TValue>>
using TUniquePtr = std::unique_ptr<TValue, TDeleter>;

template <typename TValue>
using TSharedPtr = std::shared_ptr<TValue>;

template <typename TValue>
using TWeakPtr = std::weak_ptr<TValue>;

// --- 序列容器 ---

template <typename TValue, typename TAllocator = std::allocator<TValue>>
using TVector = std::vector<TValue, TAllocator>;

template <typename TValue, std::size_t Size>
using TArray = std::array<TValue, Size>;

template <typename TValue, typename TAllocator = std::allocator<TValue>>
using TDeque = std::deque<TValue, TAllocator>;

template <typename TValue, typename TAllocator = std::allocator<TValue>>
using TList = std::list<TValue, TAllocator>;

template <typename TValue, typename TContainer = std::deque<TValue>>
using TQueue = std::queue<TValue, TContainer>;

template <typename TValue, typename TContainer = std::deque<TValue>>
using TStack = std::stack<TValue, TContainer>;

template <typename TFirst, typename TSecond>
using TPair = std::pair<TFirst, TSecond>;

// --- 关联容器 ---

template <
    typename TKey,
    typename TValue,
    typename TCompare   = std::less<TKey>,
    typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
using TMap = std::map<TKey, TValue, TCompare, TAllocator>;

template <
    typename TKey,
    typename TValue,
    typename THash      = std::hash<TKey>,
    typename TEqual     = std::equal_to<TKey>,
    typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
using THashMap = std::unordered_map<TKey, TValue, THash, TEqual, TAllocator>;

template <
    typename TValue,
    typename TCompare   = std::less<TValue>,
    typename TAllocator = std::allocator<TValue>>
using TSet = std::set<TValue, TCompare, TAllocator>;

template <
    typename TValue,
    typename THash      = std::hash<TValue>,
    typename TEqual     = std::equal_to<TValue>,
    typename TAllocator = std::allocator<TValue>>
using THashSet = std::unordered_set<TValue, THash, TEqual, TAllocator>;

}  // namespace Zero
