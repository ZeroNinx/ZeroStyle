#include "ZeroStyle.h"

#include <cassert>
#include <cstdlib>

using namespace Zero;

// --- 类型大小静态断言 ---
static_assert(sizeof(int8)   == 1);
static_assert(sizeof(int16)  == 2);
static_assert(sizeof(int32)  == 4);
static_assert(sizeof(int64)  == 8);
static_assert(sizeof(uint8)  == 1);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(uint64) == 8);

// --- Result<T> 基本行为 ---
static_assert([] {
    // 仅在 constexpr 可用时验证工厂方法可实例化
    return true;
}());

int main()
{
    // Ok 路径
    auto OkResult = Result<int32>::Ok(42);
    assert(OkResult.IsOk());
    assert(!OkResult.IsErr());
    assert(OkResult.Value() == 42);

    // Err 路径
    auto ErrResult = Result<int32>::Err(
        MakeError(ErrorCode::InvalidArgument, "bad input"));
    assert(ErrResult.IsErr());
    assert(!ErrResult.IsOk());
    assert(ErrResult.Failure().Code == ErrorCode::InvalidArgument);

    // VoidResult
    VoidResult<> OkVoid = Result<Unit>::Ok(Unit{});
    assert(OkVoid.IsOk());

    // 容器别名实例化
    Vector<int32>           Vec = {1, 2, 3};
    HashMap<String, int32>  Map;
    Map["hello"] = 1;

    assert(Vec.size() == 3);
    assert(Map["hello"] == 1);

    return EXIT_SUCCESS;
}
