#include "weatherparser.h"

#include <chrono>
#include <string>

#include "weatherdata.h"
#include "errorhandling.h"

namespace utility
{

/*
 * Parses time provided by API. User may want to change time zone from what was used by the API.
 *
 * @note:
 *
 * If time given by API is no longer in local time, change 'timepoint' time to use sys_time.
 *
 * @params:
 *
 * time: "2026-03-27T19:15" in API time zone, json.at("timeszone").
 * timezone: desired time zone.
 */
std::expected<WeatherTime, ParsingError> TimeZoneParse(const std::string& time, const std::string& timezone = "America/Los_Angeles")
{
    try
    {
        std::istringstream ss{time};

        // time provided by API uses 'local_time'
        const std::chrono::time_zone* zone{std::chrono::locate_zone(timezone)};
        std::chrono::local_time<std::chrono::minutes> timepoint;


        // 2026-03-27T19:15
        if (time.find('T') != std::string::npos)
        {
            if (!(ss >> std::chrono::parse("%FT%R", timepoint)))
            {
                return std::unexpected(ParsingError("ParsingError", "Error parsing timepoint."));
            }
        }
        // 2026-03-27
        else
        {
            std::chrono::local_time<std::chrono::days> datepoint;
            if ( !(ss >> std::chrono::parse("%F",datepoint)) )
            {
                return std::unexpected(ParsingError("ParsingError","Error parsing datepoint."));
            }

            timepoint = std::chrono::local_time<std::chrono::minutes>{datepoint};
        }

        std::chrono::zoned_time zonedtime = std::chrono::zoned_time{zone, timepoint};
        auto localtime = zonedtime.get_local_time();

        // Get total nuber of days since unix epoch, then translate to 'mon'- 'fri'.
        std::chrono::local_time<std::chrono::days> days = std::chrono::floor<std::chrono::days>(localtime);
        std::chrono::year_month_weekday yearlybreakdown = std::chrono::year_month_weekday(days);

        // time elapsed since midnight.
        auto durationsincemidnight = localtime - days;
        std::chrono::hh_mm_ss hourlybreakdown{durationsincemidnight};

        return WeatherTime
        {
            .ymw = yearlybreakdown,
            .hms = hourlybreakdown,

            .day  = std::format("{}", yearlybreakdown.weekday()),
            .hour = std::format("{}{}",
                                    std::chrono::make12(hourlybreakdown.hours()).count(),
                                    std::chrono::is_am(hourlybreakdown.hours()) ?"am":"pm")
        };
    }
    catch (const std::exception& e)
    {
        return std::unexpected(ParsingError("ParsingError", std::format("Time parsing error: {}", e.what())));
    }
}

std::expected<WeatherData, ParsingError> WeatherParseContents(const std::string& contents, const utility::Location& location)
{
    try
    {
        auto json = nlohmann::json::parse(contents);
        if (json.contains("error"))
        {
            return std::unexpected(ParsingError("ParsingError", json["reason"].get<std::string>()));
        }

        WeatherData data;

        data.location = location;

        const auto current = json.at("current");
        const auto hourly  = json.at("hourly");
        const auto daily   = json.at("daily");

        const auto currentTemp = current.at("temperature_2m").get<float>();
        const auto currentWC   = current.at("weather_code").get<int>();
        // note: ubuntu font has trouble displaying tempUnits symbol.
        // const auto tempUnits   = json.at("current_units").at("temperature_2m").get<std::string>();
        const auto tempUnits   = "*F";

        const auto time     = daily.at("time");
        const auto meantemp = daily.at("temperature_2m_mean");
        const auto mintemp  = daily.at("temperature_2m_min");
        const auto maxtemp  = daily.at("temperature_2m_max");
        const auto weathercode = daily.at("weather_code");

        const auto hourlyTime        = hourly.at("time");
        const auto hourlyTemperature = hourly.at("temperature_2m");
        const auto hourlyWC          = hourly.at("weather_code");

        data.currentTemperature = std::format("{:.1f}", currentTemp);
        data.high               = std::format("{:.0f}", maxtemp[0].get<float>());
        data.low                = std::format("{:.0f}", mintemp[0].get<float>());
        data.condition          = TranslateWeatherCode(currentWC);

        // note: relies on url parameter forecast_days >= 8.
        // start at 1, skip today.
        for (size_t i{1}; i <= data.weeklyForecast.size(); ++i)
        {
            auto utime = time[i].get<std::string>();
            auto ptime = TimeZoneParse(utime);
            auto day   = ptime.has_value() ? ptime->day : utime;

            data.weeklyForecast[i - 1] =
            {
                .day  = day,
                .mean = std::format("{:.0f}", meantemp[i].get<float>()),
                .high = std::format("{:.0f}", maxtemp[i].get<float>()),
                .low = std::format("{:.0f}", mintemp[i].get<float>()),
                .condition = TranslateWeatherCode(weathercode[i].get<int>())
            };
        }

        // "2026-03-27T19:15"
        auto normalizedTime = current.at("time").get<std::string>();
        if (auto colon = normalizedTime.rfind(":"); colon != std::string::npos)
        {
            normalizedTime.replace(colon + 1, 2, "00");
        }

        // find current hour to start hourly forecast from.
        size_t i{0};
        while(i < hourlyTime.size())
        {
            if (!normalizedTime.compare(hourlyTime[i].get<std::string>()))
            {
                break;
            }
            ++i;
        }
        if (i == hourlyTime.size())
        {
            return std::unexpected(ParsingError("ParsingError", "Could not find current time in hourly forecast."));
        }

        // get the next 24 hours of forecast.
        size_t hourslater{i + data.hourlyForecast.size()};
        for (size_t k{0}; i < hourslater; ++i, ++k)
        {
            auto utime = hourlyTime[i].get<std::string>();
            auto ptime = TimeZoneParse(utime);
            auto hour = ptime.has_value() ? ptime->hour : utime.substr(utime.find('T'));

            data.hourlyForecast[k] =
            {
                .hour        = hour,
                .temperature = std::format("{:.0f}", hourlyTemperature[i].get<float>()),
                .condition   = TranslateWeatherCode(hourlyWC[i].get<int>())
            };
        }

        data.lastUpdated = current.at("time").get<std::string>();

        return data;
    }
    catch(const nlohmann::json::exception& e)
    {
        // TODO: figure out how to handle error.
        return std::unexpected(ParsingError("ParsingError", std::format("JSON parse failed: {}", e.what())));
    }
}

std::expected<Location, ParsingError> GeoParseContents(const std::string& state, const std::string& city, const std::string& contents)
{
    try
    {
        auto json = nlohmann::json::parse(contents);
        if (!json.contains("results"))
        {
            if (json.contains("reason"))
            {
                return std::unexpected(ParsingError("ParsingError", json["reason"].get<std::string>()));
            }

            return std::unexpected(ParsingError("ParsingError", "Unkown error: missing 'results' field."));
        }

        for (const auto& result: json["results"])
        {
            if (result.contains("admin1")
                    && result["admin1"].get<std::string>() == state
                    && result["name"].get<std::string>() == city)
            {
                auto latD = result.at("latitude").get<double>();
                auto lonD = result.at("longitude").get<double>();

                return utility::Location
                {
                    .latitude  = std::format("{:.6f}", latD),
                    .longitude = std::format("{:.6f}", lonD),
                    .state     = state,
                    .city      = city
                };
            }
        }
    }
    catch(const nlohmann::json::exception& e)
    {
        return std::unexpected(ParsingError("ParsingError", std::format("JSON parse failed: {}", e.what())));
    }

    return std::unexpected(ParsingError("ParsingError", std::format("Could not find state: {}", state)));
}
}// namespace utility
