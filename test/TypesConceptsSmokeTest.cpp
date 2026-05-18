#include "ZeroStyle.h"

#include <cstdlib>
#include <functional>
#include <memory>
#include <type_traits>

using namespace Zero;

static_assert(sizeof(int8)   == 1);
static_assert(sizeof(int16)  == 2);
static_assert(sizeof(int32)  == 4);
static_assert(sizeof(int64)  == 8);
static_assert(sizeof(uint8)  == 1);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(uint64) == 8);
static_assert(std::is_same_v<TVector<int32>, std::vector<int32>>);
static_assert(std::is_same_v<TUniquePtr<int32>, std::unique_ptr<int32>>);
static_assert(std::is_same_v<TUniquePtr<int32, std::default_delete<int32>>, std::unique_ptr<int32>>);

static_assert(CStringLike<String>);
static_assert(CStringLike<StringView>);
static_assert(CStringLike<const char*>);
static_assert(!CStringLike<int32>);
static_assert(CPathLike<Path>);
static_assert(CHashable<String>);
static_assert(CHashable<int32>);
static_assert(CErrorType<SError>);

struct SCountingDelete
{
    void operator()(int32* Value) const
    {
        delete Value;
    }
};

int main()
{
    TVector<int32>          Vec = {1, 2, 3};
    THashMap<String, int32> Map;
    Map["hello"] = 1;

    ZERO_CHECK(Vec.size() == 3);
    ZERO_CHECK(Map["hello"] == 1);

    TVector<int32, std::allocator<int32>> CustomAllocatedVec = {4, 5};
    TMap<int32, String, std::greater<int32>> CustomCompareMap;
    THashMap<String, int32, std::hash<String>, std::equal_to<String>> CustomHashMap;
    TUniquePtr<int32, SCountingDelete> CustomDeletePtr(new int32(9));

    CustomCompareMap.emplace(1, "one");
    CustomHashMap.emplace("nine", *CustomDeletePtr);

    ZERO_CHECK(CustomAllocatedVec.size() == 2);
    ZERO_CHECK(CustomCompareMap.begin()->first == 1);
    ZERO_CHECK(CustomHashMap["nine"] == 9);

    return EXIT_SUCCESS;
}
