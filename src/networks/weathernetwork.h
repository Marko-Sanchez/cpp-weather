#ifndef WEATHERNETWORK_H
#define WEATHERNETWORK_H

#include <string>
#include <expected>

#include <httplib.h>

#include "utility/weatherdata.h"

namespace network
{
class WeatherNetwork
{
private:

    const std::string WEB_ADDRESS{"http://api.open-meteo.com"};
    const std::string API_ENDPOINT{"/v1/forecast"};

    httplib::Client m_weatherclient;
    httplib::Params m_weatherparams;
    httplib::Headers m_headers;

    utility::Location m_location;

public:

    WeatherNetwork(std::optional<utility::Location> coordinates = {});

    std::expected<utility::WeatherData, std::string> GetWeather();
    httplib::Client& GetClient();
};
}// network
#endif
