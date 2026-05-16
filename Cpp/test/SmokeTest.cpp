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

struct MoveOnly final
{
    explicit MoveOnly(int32 InValue)
        : Value(InValue)
    {
    }

    MoveOnly(const MoveOnly&)            = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&&) noexcept            = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;

    int32 Value = 0;
};

int main()
{
    // Ok 路径
    auto OkResult = Result<int32>::Ok(42);
    assert(OkResult.IsOk());
    assert(!OkResult.IsErr());
    assert(OkResult.Value() == 42);

#if ZERO_HAS_EXPECTED
    assert(OkResult.has_value());
    assert(*OkResult == 42);
#endif

    // Err 路径
    auto ErrResult = Result<int32>::Err(
        MakeError(ErrorCode::InvalidArgument, "bad input", "config.json"));
    assert(ErrResult.IsErr());
    assert(!ErrResult.IsOk());
    assert(ErrResult.Failure().Code == ErrorCode::InvalidArgument);
    assert(ErrResult.Failure().ContextPath == Path("config.json"));

#if ZERO_HAS_EXPECTED
    assert(!ErrResult.has_value());
    assert(ErrResult.error().Code == ErrorCode::InvalidArgument);
#endif

    // VoidResult
    VoidResult<> OkVoid = VoidResult<>::Ok(Unit{});
    assert(OkVoid.IsOk());

    // Move-only 值
    auto MoveResult = Result<MoveOnly>::Ok(MoveOnly{7});
    assert(MoveResult.IsOk());
    MoveOnly MovedValue = std::move(MoveResult).TakeValue();
    assert(MovedValue.Value == 7);

    // 容器别名实例化
    Vector<int32>          Vec = {1, 2, 3};
    HashMap<String, int32> Map;
    Map["hello"] = 1;

    assert(Vec.size() == 3);
    assert(Map["hello"] == 1);

    return EXIT_SUCCESS;
}
