#include "ZeroStyle.h"

#include <cstdlib>

using namespace Zero;

enum class ESmokeFlags : uint32
{
    None  = 0,
    Read  = 1 << 0,
    Write = 1 << 1,
};

ZERO_ENABLE_ENUM_FLAGS(ESmokeFlags);

namespace Smoke {

enum class ENamespacedFlags : uint32
{
    None  = 0,
    Alpha = 1 << 0,
    Beta  = 1 << 1,
};

ZERO_ENABLE_ENUM_FLAGS(ENamespacedFlags);

}  // namespace Smoke

int main()
{
    bool bScopeExitRan = false;
    {
        auto ScopeExit = MakeScopeExit([&bScopeExitRan]() {
            bScopeExitRan = true;
        });
        ZERO_CHECK(ScopeExit.IsActive());
    }
    ZERO_CHECK(bScopeExitRan);

    bool bReleasedScopeExitRan = false;
    {
        auto ScopeExit = MakeScopeExit([&bReleasedScopeExitRan]() {
            bReleasedScopeExitRan = true;
        });
        ScopeExit.Release();
    }
    ZERO_CHECK(!bReleasedScopeExitRan);

    ESmokeFlags Flags = ESmokeFlags::Read | ESmokeFlags::Write;
    ZERO_CHECK(HasAnyFlags(Flags, ESmokeFlags::Read));
    ZERO_CHECK(HasAllFlags(Flags, ESmokeFlags::Read | ESmokeFlags::Write));

    Flags &= ESmokeFlags::Read;
    ZERO_CHECK(HasAllFlags(Flags, ESmokeFlags::Read));
    ZERO_CHECK(!HasAnyFlags(Flags, ESmokeFlags::Write));

    Smoke::ENamespacedFlags NamespacedFlags =
        Smoke::ENamespacedFlags::Alpha | Smoke::ENamespacedFlags::Beta;
    ZERO_CHECK(HasAllFlags(NamespacedFlags, Smoke::ENamespacedFlags::Alpha));
    ZERO_CHECK(HasAnyFlags(NamespacedFlags, Smoke::ENamespacedFlags::Beta));

    return EXIT_SUCCESS;
}
