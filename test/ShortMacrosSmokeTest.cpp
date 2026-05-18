#define ZERO_ENABLE_SHORT_MACROS
#include "ZeroStyle.h"

#include <cstdlib>

using namespace Zero;

NODISCARD int32 ReturnValue()
{
    return 42;
}

struct NODISCARD_TYPE SShortMacroValue
{
    int32 Value = 0;
};

int main()
{
    MAYBE_UNUSED const int32 UnusedValue = ReturnValue();
    SShortMacroValue Value{42};

    ZERO_CHECK(Value.Value == 42);
    ZERO_CHECK(ReturnValue() == 42);

    return EXIT_SUCCESS;
}
