#include "ZeroStyle.h"

#include <cstdlib>

using namespace Zero;

int main()
{
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

    const Path ConfigPath = "Config/Default.json";
    ZERO_CHECK(ToGenericString(ConfigPath) == "Config/Default.json");
    ZERO_CHECK(HasExtension(ConfigPath, ".json"));
    ZERO_CHECK(HasExtension(ConfigPath, "json"));
    ZERO_CHECK(WithExtension(ConfigPath, "txt").extension() == ".txt");
    ZERO_CHECK(ToGenericString(NormalizeLexically("Config/../Config/Default.json")) ==
           "Config/Default.json");

    return EXIT_SUCCESS;
}
