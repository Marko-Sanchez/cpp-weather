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
    Rainy,
    Stormy,
    Snowy,
    Foggy,
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

inline WeatherCondition TranslateWeatherCode(int code)
{
    return WeatherCondition::Unknown;
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
