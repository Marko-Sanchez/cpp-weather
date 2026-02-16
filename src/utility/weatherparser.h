#ifndef WEATHERPARSER_H
#define WEATHERPARSER_H

#include <format>
#include <nlohmann/json.hpp>

namespace utility
{
    std::string ParseContents(const std::string& contents);
}
#endif
