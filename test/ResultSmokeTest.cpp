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

    auto CopiedOkResult = OkResult;
    ZERO_CHECK(CopiedOkResult.IsOk());
    ZERO_CHECK(CopiedOkResult.Value() == 42);

    auto ErrResult = TResult<int32>::Err(
        MakeError(EErrorCode::InvalidArgument, "bad input", "config.json"));
    ZERO_CHECK(ErrResult.IsErr());
    ZERO_CHECK(!ErrResult.IsOk());
    ZERO_CHECK(ErrResult.Failure().Code == EErrorCode::InvalidArgument);
    ZERO_CHECK(ErrResult.Failure().ContextPath == Path("config.json"));

    auto CopiedErrResult = ErrResult;
    ZERO_CHECK(CopiedErrResult.IsErr());
    ZERO_CHECK(CopiedErrResult.Failure().Code == EErrorCode::InvalidArgument);

    TVoidResult<> VoidResult = OkVoid();
    ZERO_CHECK(VoidResult.IsOk());

    auto MoveResult = TResult<SMoveOnly>::Ok(SMoveOnly{7});
    ZERO_CHECK(MoveResult.IsOk());
    SMoveOnly MovedValue = std::move(MoveResult).TakeValue();
    ZERO_CHECK(MovedValue.Value == 7);

    const SError FormattedError = MakeError(
        EErrorCode::ParseFailed,
        "bad config",
        "config.json");
    ZERO_CHECK(ToString(EErrorCode::ParseFailed) == "ParseFailed");
    ZERO_CHECK(FormatError(FormattedError) == "ParseFailed: bad config (config.json)");

    return EXIT_SUCCESS;
}
