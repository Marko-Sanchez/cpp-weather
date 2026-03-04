#include "geonetwork.h"

#include <print>
#include <httplib.h>

#include "cppweather/cppweather.h"
#include "utility/weatherparser.h"

namespace network
{
GeoNetwork::GeoNetwork(const std::string city, const std::string state):
m_state(state),
m_geoclient(WEB_ADDRESS)
{
    m_geoparams.emplace("name", city);
    m_geoparams.emplace("count", "5");
    m_geoparams.emplace("language", "en");
    m_geoparams.emplace("format", "json");
    m_geoparams.emplace("countryCode", "US");

    m_headers.emplace("User-Agent", std::format("cpp-weather/{}", CPPWEATHER_VERSION));
    m_headers.emplace("Accept", "application/json");
}

std::expected<Location, std::string> GeoNetwork::GetGeographicCordinates()
{
    auto result = m_geoclient.Get(API_ENDPOINT, m_geoparams, m_headers);
    if (result && result->status == httplib::StatusCode::OK_200)
    {
        auto type = result->get_header_value("Content-type");
        if (type.find("application/json") == std::string::npos)
        {
            return std::unexpected("No Json value returned.");
        }

        auto pair = utility::GeoParseContents(m_state, result->body);
        return Location{pair->first, pair->second};
    }

    return std::unexpected("Unable to retrieve latitude/longitude.");
}

std::optional<Location> GeoNetwork::GetCoordinates()
{
    auto cords = this->GetGeographicCordinates();
    if (cords)
    {
        return cords.value();
    }

    std::println("{}", cords.error());
    return std::nullopt;
}
}// namespace network
