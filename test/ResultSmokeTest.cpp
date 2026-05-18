#include "ZeroStyle.h"

#include <cstdlib>
#include <utility>

using namespace Zero;

struct SMoveOnly
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
    auto OkResult = TResult<int32>::Ok(42);
    ZERO_CHECK(OkResult.IsOk());
    ZERO_CHECK(!OkResult.IsErr());
    ZERO_CHECK(OkResult.Value() == 42);
    ZERO_CHECK(OkResult.ValueOr(7) == 42);

    auto CopiedOkResult = OkResult;
    ZERO_CHECK(CopiedOkResult.IsOk());
    ZERO_CHECK(CopiedOkResult.Value() == 42);

    auto ErrResult = TResult<int32>::Err(
        MakeError(EErrorCode::InvalidArgument, "bad input", "config.json"));
    ZERO_CHECK(ErrResult.IsErr());
    ZERO_CHECK(!ErrResult.IsOk());
    ZERO_CHECK(ErrResult.Failure().Code == EErrorCode::InvalidArgument);
    ZERO_CHECK(ErrResult.Failure().ContextPath == Path("config.json"));
    ZERO_CHECK(ErrResult.ValueOr(7) == 7);

    auto CopiedErrResult = ErrResult;
    ZERO_CHECK(CopiedErrResult.IsErr());
    ZERO_CHECK(CopiedErrResult.Failure().Code == EErrorCode::InvalidArgument);

    TVoidResult<> VoidResult = OkVoid();
    ZERO_CHECK(VoidResult.IsOk());

    TVoidResult<> DirectVoidResult = TVoidResult<>::Ok();
    ZERO_CHECK(DirectVoidResult.IsOk());
    ZERO_CHECK(DirectVoidResult.Value() == SUnit{});
    ZERO_CHECK(DirectVoidResult.ValueOr() == SUnit{});

    auto MoveResult = TResult<SMoveOnly>::Ok(SMoveOnly{7});
    ZERO_CHECK(MoveResult.IsOk());
    SMoveOnly MovedValue = std::move(MoveResult).TakeValue();
    ZERO_CHECK(MovedValue.Value == 7);

    SMoveOnly MoveValueOrOk = TResult<SMoveOnly>::Ok(SMoveOnly{8}).ValueOr(SMoveOnly{9});
    ZERO_CHECK(MoveValueOrOk.Value == 8);

    SMoveOnly MoveValueOrErr =
        TResult<SMoveOnly>::Err(MakeError(EErrorCode::InvalidArgument, "missing value"))
            .ValueOr(SMoveOnly{9});
    ZERO_CHECK(MoveValueOrErr.Value == 9);

    auto MappedOk = TResult<int32>::Ok(21).Map([](int32 Value) {
        return Value * 2;
    });
    ZERO_CHECK(MappedOk.IsOk());
    ZERO_CHECK(MappedOk.Value() == 42);

    auto MappedErr =
        TResult<int32>::Err(MakeError(EErrorCode::ParseFailed, "bad number")).Map([](int32 Value) {
            return Value * 2;
        });
    ZERO_CHECK(MappedErr.IsErr());
    ZERO_CHECK(MappedErr.Failure().Code == EErrorCode::ParseFailed);

    bool bVoidMapRan = false;
    auto VoidMapResult = TResult<int32>::Ok(1).Map([&bVoidMapRan](int32 Value) {
        bVoidMapRan = Value == 1;
    });
    ZERO_CHECK(bVoidMapRan);
    ZERO_CHECK(VoidMapResult.IsOk());

    auto ChainedOk = TResult<int32>::Ok(5).AndThen([](int32 Value) {
        return TResult<int32>::Ok(Value + 6);
    });
    ZERO_CHECK(ChainedOk.IsOk());
    ZERO_CHECK(ChainedOk.Value() == 11);

    bool bSkippedAndThen = true;
    auto ChainedErr =
        TResult<int32>::Err(MakeError(EErrorCode::NotOpen, "database is not open"))
            .AndThen([&bSkippedAndThen](int32 Value) {
                bSkippedAndThen = false;
                return TResult<int32>::Ok(Value + 6);
            });
    ZERO_CHECK(bSkippedAndThen);
    ZERO_CHECK(ChainedErr.IsErr());
    ZERO_CHECK(ChainedErr.Failure().Code == EErrorCode::NotOpen);

    auto UnitMapped = TVoidResult<>::Ok().Map([] {
        return 42;
    });
    ZERO_CHECK(UnitMapped.IsOk());
    ZERO_CHECK(UnitMapped.Value() == 42);

    bool bUnitVoidMapRan = false;
    auto UnitVoidMapped = TVoidResult<>::Ok().Map([&bUnitVoidMapRan] {
        bUnitVoidMapRan = true;
    });
    ZERO_CHECK(bUnitVoidMapRan);
    ZERO_CHECK(UnitVoidMapped.IsOk());

    auto UnitChained = TVoidResult<>::Ok().AndThen([] {
        return TResult<int32>::Ok(13);
    });
    ZERO_CHECK(UnitChained.IsOk());
    ZERO_CHECK(UnitChained.Value() == 13);

    bool bSkippedUnitAndThen = true;
    auto UnitChainedErr =
        TVoidResult<>::Err(MakeError(EErrorCode::FileWriteFailed, "cannot write"))
            .AndThen([&bSkippedUnitAndThen] {
                bSkippedUnitAndThen = false;
                return TResult<int32>::Ok(13);
            });
    ZERO_CHECK(bSkippedUnitAndThen);
    ZERO_CHECK(UnitChainedErr.IsErr());
    ZERO_CHECK(UnitChainedErr.Failure().Code == EErrorCode::FileWriteFailed);

    const SError FormattedError = MakeError(
        EErrorCode::ParseFailed,
        "bad config",
        "config.json");
    ZERO_CHECK(ToString(EErrorCode::ParseFailed) == "ParseFailed");
    ZERO_CHECK(FormatError(FormattedError) == "ParseFailed: bad config (config.json)");

    return EXIT_SUCCESS;
}
