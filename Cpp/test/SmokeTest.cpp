#include "ZeroStyle.h"

#include <cassert>
#include <cstdlib>
#include <utility>

using namespace Zero;

// --- 类型大小静态断言 ---
static_assert(sizeof(int8)   == 1);
static_assert(sizeof(int16)  == 2);
static_assert(sizeof(int32)  == 4);
static_assert(sizeof(int64)  == 8);
static_assert(sizeof(uint8)  == 1);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(uint64) == 8);

struct SMoveOnly final
{
    explicit SMoveOnly(int32 InitialValue)
        : Value(InitialValue)
    {
    }

    SMoveOnly(const SMoveOnly&)            = delete;
    SMoveOnly& operator=(const SMoveOnly&) = delete;

    SMoveOnly(SMoveOnly&&) noexcept            = default;
    SMoveOnly& operator=(SMoveOnly&&) noexcept = default;

    int32 Value = 0;
};

int main()
{
    // Ok 路径
    auto OkResult = TResult<int32>::Ok(42);
    assert(OkResult.IsOk());
    assert(!OkResult.IsErr());
    assert(OkResult.Value() == 42);

#if ZERO_HAS_EXPECTED
    assert(OkResult.has_value());
    assert(*OkResult == 42);
#endif

    // Err 路径
    auto ErrResult = TResult<int32>::Err(
        MakeError(EErrorCode::InvalidArgument, "bad input", "config.json"));
    assert(ErrResult.IsErr());
    assert(!ErrResult.IsOk());
    assert(ErrResult.Failure().Code == EErrorCode::InvalidArgument);
    assert(ErrResult.Failure().ContextPath == Path("config.json"));

#if ZERO_HAS_EXPECTED
    assert(!ErrResult.has_value());
    assert(ErrResult.error().Code == EErrorCode::InvalidArgument);
#endif

    // TVoidResult
    TVoidResult<> OkVoid = TVoidResult<>::Ok(SUnit{});
    assert(OkVoid.IsOk());

    // Move-only 值
    auto MoveResult = TResult<SMoveOnly>::Ok(SMoveOnly{7});
    assert(MoveResult.IsOk());
    SMoveOnly MovedValue = std::move(MoveResult).TakeValue();
    assert(MovedValue.Value == 7);

    // 容器别名实例化
    TVector<int32>          Vec = {1, 2, 3};
    THashMap<String, int32> Map;
    Map["hello"] = 1;

    assert(Vec.size() == 3);
    assert(Map["hello"] == 1);

    return EXIT_SUCCESS;
}
