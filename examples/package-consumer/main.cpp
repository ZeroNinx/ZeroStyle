#include "ZeroStyle.h"

#include <cstdlib>

int main()
{
    Zero::TVector<Zero::int32> Values = {1, 2, 3};
    return Values.size() == 3 ? EXIT_SUCCESS : EXIT_FAILURE;
}
