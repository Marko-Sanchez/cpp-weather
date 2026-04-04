#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <string>
#include <array>

namespace utility
{

enum class WeatherCondition
{
    Clear,
    PartlyCloudy,
    Cloudy,
    Foggy,
    Drizzle,
    Rainy,
    Snowy,
    RainShowers,
    ThunderStorm,
    Unknown
};

struct Location
{
    std::string latitude;
    std::string longitude;

    std::string state;
    std::string city;
};

struct HourlyForecast
{
    std::string hour;      // "11am", "11pm"
    std::string temperature;

    WeatherCondition condition;
};

struct DailyForecast
{
    std::string day; // "Mon", "Tue"

    std::string mean;
    std::string high;
    std::string low;

    WeatherCondition condition;
};

struct WeatherData
{
    // location.
    Location location;

    // current condition.
    std::string currentTemperature;
    std::string high;
    std::string low;

    WeatherCondition condition;

    // forecasts'.
    std::array<HourlyForecast, 24> hourlyForecast;
    std::array<DailyForecast, 7> weeklyForecast;

    std::string lastUpdated; // date:time of last update.
    bool isStale{true};
};

/*
 * @note:
 *
 * https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
 */
inline WeatherCondition TranslateWeatherCode(int code)
{
    if (code == 0)                return WeatherCondition::Clear;
    if (code <= 2)                return WeatherCondition::PartlyCloudy;
    if (code == 3)                return WeatherCondition::Cloudy;
    if (code >= 40 && code <= 49) return WeatherCondition::Foggy;
    if (code >= 50 && code <= 59) return WeatherCondition::Drizzle;
    if (code >= 60 && code <= 69) return WeatherCondition::Rainy;
    if (code >= 70 && code <= 75) return WeatherCondition::Snowy;
    if (code >= 80 && code <= 84) return WeatherCondition::RainShowers;
    if (code >= 95 && code <= 99) return WeatherCondition::ThunderStorm;

    return WeatherCondition::Unknown;
}

inline std::string_view WeatherConditionToString(WeatherCondition condition)
{
    switch(condition)
    {
        case WeatherCondition::Clear:        return "Clear";
        case WeatherCondition::PartlyCloudy: return "Partly Cloudy";
        case WeatherCondition::Cloudy:       return "Cloudy";
        case WeatherCondition::Foggy:        return "Foggy";
        case WeatherCondition::Drizzle:      return "Drizzle";
        case WeatherCondition::Rainy:        return "Rainy";
        case WeatherCondition::Snowy:        return "Snowy";
        case WeatherCondition::RainShowers:  return "Rain Showers";
        case WeatherCondition::ThunderStorm: return "Thunderstorm";

        default:                             return "Unknown";
    }
}

inline Location GetDefaultLocation()
{
    return Location
    {
        .latitude  = "47.6446751",
        .longitude = "-122.133615",
        .state     = "Washington, USA",
        .city      = "Redmond"
    };
}

// Place holder or used when first starting forecast layer.
inline WeatherData MakeDefaultWeatherData()
{
    WeatherData data =
    {
        .location = GetDefaultLocation(),

        .currentTemperature = "---",
        .high               = "---",
        .low                = "---",
        .condition          = WeatherCondition::Cloudy,
        .isStale            = true
    };

    for (int i{0}; i < 24; ++i)
    {
        data.hourlyForecast[i] =
        {
            .hour        = std::to_string(i) + ":00",
            .temperature = "---",
            .condition   = WeatherCondition::Unknown
        };
    }

    for (int i{0}; i < 7; ++i)
    {
        data.weeklyForecast[i] =
        {
            .day      = "---",
            .mean     = "---",
            .high     = "---",
            .low      = "---",
            .condition = WeatherCondition::Unknown
        };
    }

    return data;
}
}// namespace utility
#endif
