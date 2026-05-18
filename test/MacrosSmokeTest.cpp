#include "ZeroStyle.h"

#include <cstdlib>

#ifdef NODISCARD
#    error "Short macro NODISCARD must not be defined by default."
#endif

#ifdef REQUIRES
#    error "Short macro REQUIRES must not be defined by default."
#endif

using namespace Zero;

struct SEmpty
{
};

struct SAttributeSmoke
{
    ZERO_NO_UNIQUE_ADDRESS SEmpty Empty;
    int32                    Value = 0;
};

int main()
{
    bool bEnsureSmoke = true;
    ZERO_CHECK(ZERO_ENSURE(bEnsureSmoke));

    ZERO_MAYBE_UNUSED const int32 UnusedValue = 0;

    SAttributeSmoke AttributeSmoke;
    AttributeSmoke.Value = 1;
    ZERO_CHECK(AttributeSmoke.Value == 1);

    int32 FallthroughValue = 0;
    switch (1)
    {
    case 0:
        FallthroughValue += 1;
        ZERO_FALLTHROUGH;
    case 1:
        FallthroughValue += 2;
        break;
    default:
        break;
    }
    ZERO_CHECK(FallthroughValue == 2);

    return EXIT_SUCCESS;
}
