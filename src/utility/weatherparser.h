#ifndef WEATHERPARSER_H
#define WEATHERPARSER_H

#include <expected>
#include <chrono>
#include <nlohmann/json.hpp>

#include "weatherdata.h"
#include "errorhandling.h"

// Option: error output to terminal on Debug otherwise error output to a file. This is differnent then the network logging.
namespace utility
{
    struct WeatherTime
    {
        std::chrono::year_month_weekday ymw;
        std::chrono::hh_mm_ss<std::chrono::duration<long>>  hms;

        std::string day;
        std::string hour;
    };

    std::expected<WeatherTime, ParsingError> TimeZoneParse(const std::string& time, const std::string& timezone);

    std::expected<WeatherData, ParsingError> WeatherParseContents(const std::string& contents, const utility::Location& location);

    std::expected<Location, ParsingError> GeoParseContents(const std::string& state, const std::string& city, const std::string& contents);
}
#endif
