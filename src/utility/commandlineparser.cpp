#include "commandlineparser.h"

#include <print>

namespace utility
{
std::optional<std::pair<std::string, std::string>> ParseLocation(std::string_view location)
{
    const std::regex locationPattern{R"((\w+(?:\s+\w+)*)\s*,\s*(\w+(?:\s+\w+)*))"};
    std::smatch match;

    std::string locationString{location};
    if (std::regex_match(locationString, match, locationPattern))
    {
        return std::make_pair(match[1].str(), match[2].str());
    }

    return std::nullopt;
}

CommandLineArgs ParseArgs(int argc, char* argv[])
{
    CommandLineArgs args;

    for (int i{1}; i < argc; ++i)
    {
        std::string_view arg{argv[i]};

        if (arg == "--version")
        {
            args.showversion = true;
        }
        else if (arg == "--no-gui")
        {
            args.nogui = true;
        }
        else if (arg == "--location")
        {
            if (i + 1 < argc)
            {
                auto location = ParseLocation(argv[i + 1]);
                if (location)
                {
                    args.location = location;
                    ++i;
                }
                else
                {
                    std::println(stderr, "Error: Invalid format. Expected: city, state");
                    std::println(stderr, "Example: --location \"Los Angeles, California\"");
                    return args;
                }
            }
            else
            {
                std::println(stderr, "Error: --location requires an argument.");
                return args;
            }
        }
    }

    return args;
}
}// namespace utility
