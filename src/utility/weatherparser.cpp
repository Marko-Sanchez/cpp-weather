#include "weatherparser.h"
#include <string>

namespace utility
{
std::string ParseContents(const std::string& contents)
{
    auto json = nlohmann::json::parse(contents);
    if (json.contains("error"))
    {
        return json.at("reasons").get<std::string>();
    }

    double temperature = json.at("current").at("temperature_2m").get<double>();
    std::string tempUnits = json.at("current_units").at("temperature_2m").get<std::string>();

    const auto time = json.at("daily").at("time");
    const auto temp = json.at("daily").at("temperature_2m_mean");

    std::string comb{std::format("temperature: {} {}\n\n", temperature, tempUnits)};
    for (size_t i{1}; i < time.size(); ++i)
    {
        comb += std::format("temperature from {} at {}{}\n", time[i].get<std::string>(), temp[i].get<int>(), tempUnits);
    }

    return comb;
};

std::expected<std::pair<std::string, std::string>, std::string> GeoParseContents(const std::string& state, const std::string& contents)
{
    auto json = nlohmann::json::parse(contents);
    if (json.contains("error"))
    {
        return std::unexpected{"Json parsing error."};
    }

    for (const auto& result: json.at("results"))
    {
        if (result.at("admin1").get<std::string>() == state)
        {
            auto latI = result.at("latitude").get<int>();
            auto lonI = result.at("longitude").get<int>();

            return std::make_pair(std::to_string(latI), std::to_string(lonI));
        }
    }

    return std::unexpected{"Could not find correct state."};
}
}// namespace utility
