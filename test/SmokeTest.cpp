#include "ZeroStyle.h"

#include <cstdlib>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#ifdef NODISCARD
#    error "Short macro NODISCARD must not be defined by default."
#endif

#ifdef REQUIRES
#    error "Short macro REQUIRES must not be defined by default."
#endif

using namespace Zero;

// --- 类型大小静态断言 ---
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

// --- Concepts 静态断言 ---
static_assert(CStringLike<String>);
static_assert(CStringLike<StringView>);
static_assert(CStringLike<const char*>);
static_assert(!CStringLike<int32>);
static_assert(CPathLike<Path>);
static_assert(CHashable<String>);
static_assert(CHashable<int32>);
static_assert(CErrorType<SError>);

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

struct SEmpty
{
};

struct SAttributeSmoke
{
    ZERO_NO_UNIQUE_ADDRESS SEmpty Empty;
    int32                    Value = 0;
};

struct SCountingDelete
{
    void operator()(int32* Value) const
    {
        delete Value;
    }
};

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
    bool bEnsureSmoke = true;
    ZERO_CHECK(ZERO_ENSURE(bEnsureSmoke));

    ZERO_MAYBE_UNUSED const int32 UnusedValue = 0;
    SAttributeSmoke AttributeSmoke;
    AttributeSmoke.Value = 1;
    ZERO_CHECK(AttributeSmoke.Value == 1);

    // Ok 路径
    auto OkResult = TResult<int32>::Ok(42);
    ZERO_CHECK(OkResult.IsOk());
    ZERO_CHECK(!OkResult.IsErr());
    ZERO_CHECK(OkResult.Value() == 42);

    auto CopiedOkResult = OkResult;
    ZERO_CHECK(CopiedOkResult.IsOk());
    ZERO_CHECK(CopiedOkResult.Value() == 42);

    // Err 路径
    auto ErrResult = TResult<int32>::Err(
        MakeError(EErrorCode::InvalidArgument, "bad input", "config.json"));
    ZERO_CHECK(ErrResult.IsErr());
    ZERO_CHECK(!ErrResult.IsOk());
    ZERO_CHECK(ErrResult.Failure().Code == EErrorCode::InvalidArgument);
    ZERO_CHECK(ErrResult.Failure().ContextPath == Path("config.json"));

    auto CopiedErrResult = ErrResult;
    ZERO_CHECK(CopiedErrResult.IsErr());
    ZERO_CHECK(CopiedErrResult.Failure().Code == EErrorCode::InvalidArgument);

    // TVoidResult
    TVoidResult<> VoidResult = OkVoid();
    ZERO_CHECK(VoidResult.IsOk());

    // Move-only 值
    auto MoveResult = TResult<SMoveOnly>::Ok(SMoveOnly{7});
    ZERO_CHECK(MoveResult.IsOk());
    SMoveOnly MovedValue = std::move(MoveResult).TakeValue();
    ZERO_CHECK(MovedValue.Value == 7);

    // 容器别名实例化
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

    // ScopeExit
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

    // Flags
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

    // Format
    const SError FormattedError = MakeError(
        EErrorCode::ParseFailed,
        "bad config",
        "config.json");
    ZERO_CHECK(ToString(EErrorCode::ParseFailed) == "ParseFailed");
    ZERO_CHECK(FormatError(FormattedError) == "ParseFailed: bad config (config.json)");

    // StringUtils
    ZERO_CHECK(Trim("  hello\t") == "hello");
    ZERO_CHECK(StartsWith("ZeroStyle", "Zero"));
    ZERO_CHECK(EndsWith("ZeroStyle", "Style"));

    const TVector<StringView> Parts = SplitView("a,,b", ',');
    ZERO_CHECK(Parts.size() == 2);
    ZERO_CHECK(Parts[0] == "a");
    ZERO_CHECK(Parts[1] == "b");

    const TVector<StringView> PartsWithEmpty = SplitView("a,,b", ',', true);
    ZERO_CHECK(PartsWithEmpty.size() == 3);
    ZERO_CHECK(PartsWithEmpty[1].empty());

    const TVector<String> OwnedParts = Split(String("a,,b"), ',');
    ZERO_CHECK(OwnedParts.size() == 2);
    ZERO_CHECK(OwnedParts[0] == "a");
    ZERO_CHECK(OwnedParts[1] == "b");

    const TVector<String> OwnedPartsWithEmpty = Split(String("a,,b"), ',', true);
    ZERO_CHECK(OwnedPartsWithEmpty.size() == 3);
    ZERO_CHECK(OwnedPartsWithEmpty[1].empty());

    // PathUtils
    const Path ConfigPath = "Config/Default.json";
    ZERO_CHECK(ToGenericString(ConfigPath) == "Config/Default.json");
    ZERO_CHECK(HasExtension(ConfigPath, ".json"));
    ZERO_CHECK(HasExtension(ConfigPath, "json"));
    ZERO_CHECK(WithExtension(ConfigPath, "txt").extension() == ".txt");
    ZERO_CHECK(ToGenericString(NormalizeLexically("Config/../Config/Default.json")) ==
           "Config/Default.json");

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
