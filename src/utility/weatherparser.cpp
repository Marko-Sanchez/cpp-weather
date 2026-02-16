#include "weatherparser.h"

namespace utility
{
std::string ParseContents(const std::string& contents)
{
    auto json = nlohmann::json::parse(contents);
    if (json.contains("error"))
    {
        return json.at("reasons").get<std::string>();
    }

    std::string tempUnits = json.at("current_units").at("temperature_2m").get<std::string>();
    double temperature = json.at("current").at("temperature_2m").get<double>();

    return std::format("temperature: {} {}", temperature, tempUnits);
};
}
