#ifndef WEATHERPARSER_H
#define WEATHERPARSER_H

#include <expected>
#include <nlohmann/json.hpp>

#include "weatherdata.h"

namespace utility
{
    using PSS = std::pair<std::string, std::string>;

    std::expected<WeatherData, std::string> WeatherParseContents(const std::string& contents);
    std::expected<PSS, std::string> GeoParseContents(const std::string& state, const std::string& contents);
}
#endif
