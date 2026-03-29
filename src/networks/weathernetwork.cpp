#include "weathernetwork.h"

#include "cppweather/cppweather.h"
#include "utility/weatherparser.h"

namespace network
{
WeatherNetwork::WeatherNetwork(std::optional<Location> coordinates):
m_weatherclient(WEB_ADDRESS)
{
    if (coordinates)
    {
        m_weatherparams.emplace("latitude", coordinates->latitude);
        m_weatherparams.emplace("longitude", coordinates->longitude);
    }
    else // default to microsoft headquaters: redmond, wa.
    {
        m_weatherparams.emplace("latitude", "47.6446751");
        m_weatherparams.emplace("longitude", "-122.133615");
    }

    m_weatherparams.emplace("current", "temperature_2m");
    m_weatherparams.emplace("current", "weather_code");

    m_weatherparams.emplace("hourly", "temperature_2m");

    m_weatherparams.emplace("daily", "temperature_2m_mean");
    m_weatherparams.emplace("daily", "temperature_2m_min");
    m_weatherparams.emplace("daily", "temperature_2m_max");
    m_weatherparams.emplace("daily", "weather_code");
    m_weatherparams.emplace("forecast_days", "8");

    m_weatherparams.emplace("temperature_unit", "fahrenheit");
    m_weatherparams.emplace("timezone", "America/Los_Angeles");

    m_headers.emplace("User-Agent", std::format("cpp-weather/{}", CPPWEATHER_VERSION));
    m_headers.emplace("Accept", "application/json");
}

std::expected<utility::WeatherData, std::string> WeatherNetwork::GetWeather()
{
    auto result = m_weatherclient.Get(API_ENDPOINT, m_weatherparams, m_headers);
    if (result && result->status == httplib::StatusCode::OK_200)
    {
        auto type = result->get_header_value("Content-type");
        if (type.empty() || type.find("application/json") == std::string::npos)
        {
            return std::unexpected(std::format("Unexpected Content-type returned: {}", type));
        }

        return utility::WeatherParseContents(result->body);
    }

    return std::unexpected(std::format("HTTP error: {}", httplib::to_string(result.error())));
}

httplib::Client& WeatherNetwork::GetClient()
{
    return m_weatherclient;
}
}// namespace network
