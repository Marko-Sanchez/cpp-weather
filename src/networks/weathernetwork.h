#ifndef WEATHERNETWORK_H
#define WEATHERNETWORK_H

#include <string>
#include <httplib.h>

#include "networks/geonetwork.h"

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

public:
    WeatherNetwork(std::optional<Location> coordinates = {});

    std::expected<std::string, std::string> GetWeather();
    httplib::Client& GetClient();
};
}// network
#endif
