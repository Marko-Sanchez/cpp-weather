#ifndef GEONETWORK_H
#define GEONETWORK_H

#include <expected>
#include <string>

#include <httplib.h>

namespace network
{
struct Location
{
    std::string latitude;
    std::string longitude;
};

class GeoNetwork
{
private:
    const std::string WEB_ADDRESS{"http://geocoding-api.open-meteo.com"};
    const std::string API_ENDPOINT{"/v1/search"};

    const std::string m_state;

    httplib::Client m_geoclient;
    httplib::Params m_geoparams;
    httplib::Headers m_headers;

    std::expected<Location, std::string> GetGeographicCordinates();

public:
    GeoNetwork(const std::string city, const std::string state);

    std::optional<Location> GetCoordinates();
};
}// namespace network
#endif
