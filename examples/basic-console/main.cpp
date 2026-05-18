#include "ProjectCore.h"

#include <cstdlib>
#include <iostream>
#include <utility>

namespace Example {

struct NODISCARD_TYPE SConfig final
{
    String Name;
};

NODISCARD TResult<SConfig> ParseConfig(String Text)
{
    constexpr StringView Prefix = "Name=";

    if (!Text.starts_with(Prefix))
    {
        return TResult<SConfig>::Err(MakeError(
            EErrorCode::ParseFailed,
            "config text must start with Name="));
    }

    Text.erase(0, Prefix.size());

    if (Text.empty())
    {
        return TResult<SConfig>::Err(MakeError(
            EErrorCode::InvalidArgument,
            "config name must not be empty"));
    }

    return TResult<SConfig>::Ok(SConfig{.Name = std::move(Text)});
}

}  // namespace Example

int main()
{
    auto ConfigResult = Example::ParseConfig("Name=ZeroStyle");

    if (ConfigResult.IsErr())
    {
        std::cerr << ConfigResult.Failure().Message << '\n';
        return EXIT_FAILURE;
    }

    const Example::SConfig Config = std::move(ConfigResult).TakeValue();
    std::cout << Config.Name << '\n';

    return EXIT_SUCCESS;
}
