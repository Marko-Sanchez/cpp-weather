#ifndef GEONETWORK_H
#define GEONETWORK_H

#include <expected>
#include <string>

#include <httplib.h>

#include "utility/weatherdata.h"
#include "utility/errorhandling.h"

namespace network
{
class GeoNetwork
{
private:

    using Error = std::variant<utility::NetworkError, utility::ParsingError>;

    const std::string WEB_ADDRESS{"http://geocoding-api.open-meteo.com"};
    const std::string API_ENDPOINT{"/v1/search"};

    const std::string m_state;
    const std::string m_city;

    httplib::Client m_geoclient;
    httplib::Params m_geoparams;
    httplib::Headers m_headers;

    std::expected<utility::Location, Error> GetGeographicCoordinates();

public:

    GeoNetwork(const std::string& city, const std::string& state);

    std::expected<utility::Location, Error> GetCoordinates();
    httplib::Client& GetClient();
};
}// namespace network
#endif
