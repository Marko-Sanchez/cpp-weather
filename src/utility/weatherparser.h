#ifndef WEATHERPARSER_H
#define WEATHERPARSER_H

#include <expected>
#include <nlohmann/json.hpp>

#include "weatherdata.h"

namespace utility
{
    std::expected<WeatherData, std::string> WeatherParseContents(const std::string& contents, const utility::Location& location);
    std::expected<utility::Location, std::string> GeoParseContents(const std::string& state, const std::string& city, const std::string& contents);
}
#endif
