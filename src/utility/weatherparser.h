#ifndef WEATHERPARSER_H
#define WEATHERPARSER_H

#include <expected>
#include <chrono>
#include <nlohmann/json.hpp>

#include "weatherdata.h"

namespace utility
{
    struct WeatherTime
    {
        std::chrono::year_month_weekday ymw;
        std::chrono::hh_mm_ss<std::chrono::duration<long>>  hms;

        std::string day;
        std::string hour;
    };

    std::expected<WeatherTime, std::string> TimeZoneParse(const std::string& time, const std::string& timezone);

    std::expected<WeatherData, std::string> WeatherParseContents(const std::string& contents, const utility::Location& location);

    std::expected<Location, std::string> GeoParseContents(const std::string& state, const std::string& city, const std::string& contents);
}
#endif
