#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <regex>
#include <string_view>

namespace utility
{
struct CommandLineArgs
{
    bool showversion{false};
    bool nogui{false};
    std::optional<std::pair<std::string, std::string>> location;
};


std::optional<std::pair<std::string, std::string>> ParseLocation(std::string_view location);

CommandLineArgs ParseArgs(int argc, char* argv[]);

}// namespace utility
#endif
