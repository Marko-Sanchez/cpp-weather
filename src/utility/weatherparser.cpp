#include "weatherparser.h"

#include <string>

#include "utility/weatherdata.h"

namespace utility
{
std::expected<WeatherData, std::string> WeatherParseContents(const std::string& contents, const utility::Location& location)
{
    try
    {
        auto json = nlohmann::json::parse(contents);
        if (json.contains("error"))
        {
            return std::unexpected(json["reason"].get<std::string>());
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

        data.currentTemperature = std::format("{}{}", currentTemp, tempUnits);
        data.high               = std::format("{}{}", maxtemp[0].get<float>(), tempUnits);
        data.low                = std::format("{}{}", mintemp[0].get<float>(), tempUnits);
        data.condition          = TranslateWeatherCode(currentWC);

        // note: relies on url parameter forecast_days >= 8.
        // start at 1, skip today.
        for (size_t i{1}; i <= data.weeklyForecast.size(); ++i)
        {
            data.weeklyForecast[i - 1] =
            {
                .day  = time[i].get<std::string>(),
                .mean = std::format("{}{}", meantemp[i].get<float>(), tempUnits),
                .high = std::format("{}{}", maxtemp[i].get<float>(), tempUnits),
                .low = std::format("{}{}", mintemp[i].get<float>(), tempUnits),
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
            return std::unexpected("Could not find current time in hourly forecast.");
        }

        // get the next 24 hours of forecast.
        size_t hourslater{i + data.hourlyForecast.size()};
        for (size_t k{0}; i < hourslater; ++i, ++k)
        {
            data.hourlyForecast[k] =
            {
                .hour        = hourlyTime[i].get<std::string>(),
                .temperature = std::format("{}{}", hourlyTemperature[i].get<float>(), tempUnits),
                .condition   = TranslateWeatherCode(hourlyWC[i].get<int>())
            };
        }

        data.lastUpdated = current.at("time").get<std::string>();

        return data;
    }
    catch(const nlohmann::json::exception& e)
    {
        // TODO: figure out how to handle error.
        return std::unexpected(std::format("JSON parse failed: {}", e.what()));
    }
}

std::expected<utility::Location, std::string> GeoParseContents(const std::string& state, const std::string& city, const std::string& contents)
{
    try
    {
        auto json = nlohmann::json::parse(contents);
        if (!json.contains("results"))
        {
            if (json.contains("reason"))
            {
                return std::unexpected(json["reason"].get<std::string>());
            }

            return std::unexpected("Unkown error: missing 'results' field.");
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
        return std::unexpected(std::format("JSON parse failed: {}", e.what()));
    }

    return std::unexpected{std::format("Could not find state: {}", state)};
}
}// namespace utility
