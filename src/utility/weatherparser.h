#ifndef WEATHERPARSER_H
#define WEATHERPARSER_H

#include <expected>
#include <nlohmann/json.hpp>

namespace utility
{
    std::string ParseContents(const std::string& contents);
    std::expected<std::pair<std::string, std::string>, std::string> GeoParseContents(const std::string& state, const std::string& contents);
}
#endif
