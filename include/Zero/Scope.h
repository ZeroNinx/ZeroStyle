#pragma once

// =============================================================================
// Zero/Scope.h — 局部作用域 RAII 工具
// =============================================================================

#include "Macros.h"

#include <type_traits>
#include <utility>

namespace Zero {

template <typename TFunction>
class TScopeExit final
{
public:
    explicit TScopeExit(TFunction Function) noexcept(
        std::is_nothrow_move_constructible_v<TFunction>)
        : Function(std::move(Function))
    {
    }

    TScopeExit(const TScopeExit&)            = delete;
    TScopeExit& operator=(const TScopeExit&) = delete;

    TScopeExit(TScopeExit&& Other) noexcept(
        std::is_nothrow_move_constructible_v<TFunction>)
        : Function(std::move(Other.Function))
        , bActive(Other.bActive)
    {
        Other.Release();
    }

    TScopeExit& operator=(TScopeExit&&) = delete;

    ~TScopeExit() noexcept(noexcept(std::declval<TFunction&>()()))
    {
        if (bActive)
        {
            Function();
        }
    }

    void Release() noexcept
    {
        bActive = false;
    }

    NODISCARD bool IsActive() const noexcept
    {
        return bActive;
    }

private:
    TFunction Function;
    bool      bActive = true;
};

template <typename TFunction>
NODISCARD auto MakeScopeExit(TFunction&& Function)
{
    return TScopeExit<std::decay_t<TFunction>>(std::forward<TFunction>(Function));
}

}  // namespace Zero
