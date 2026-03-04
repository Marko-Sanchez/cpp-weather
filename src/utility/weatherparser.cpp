#include "weatherparser.h"
#include <string>

namespace utility
{
std::expected<std::string, std::string> WeatherParseContents(const std::string& contents)
{
    try
    {
        auto json = nlohmann::json::parse(contents);
        if (json.contains("error"))
        {
            return std::unexpected(json["reason"].get<std::string>());
        }

        double temperature = json.at("current").at("temperature_2m").get<double>();
        std::string tempUnits = json.at("current_units").at("temperature_2m").get<std::string>();

        const auto time = json.at("daily").at("time");
        const auto temp = json.at("daily").at("temperature_2m_mean");

        std::string comb{std::format("temperature: {} {}\n\n", temperature, tempUnits)};

        // 0th-index is current day, skip.
        for (size_t i{1}; i < time.size(); ++i)
        {
            comb += std::format("temperature from {} at {}{}\n", time[i].get<std::string>(), temp[i].get<int>(), tempUnits);
        }

        return comb;
    }
    catch(const nlohmann::json::exception& e)
    {
        return std::unexpected(std::format("JSON parse failed: {}", e.what()));
    }
}

std::expected<PSS, std::string> GeoParseContents(const std::string& state, const std::string& contents)
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
            if (result.contains("admin1") && result["admin1"].get<std::string>() == state)
            {
                auto latD = result.at("latitude").get<double>();
                auto lonD = result.at("longitude").get<double>();

                return std::make_pair(std::format("{:.6f}", latD), std::format("{:.6f}", lonD));
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
