#include <chrono>
#include <core/network.h>

namespace Core
{
Network::Network(const std::string& cli, const httplib::Params& params):
m_client(cli),
m_weatherResults(nullptr)
{
    if (!params.empty())
    {
        m_weatherParams = params;
    }
    else
    {
        m_weatherParams.emplace("latitude", "47.6446751");
        m_weatherParams.emplace("longitude", "-122.133615");

        m_weatherParams.emplace("current", "temperature_2m");
        m_weatherParams.emplace("temperature_unit", "fahrenheit");
        m_weatherParams.emplace("timezone", "America/Los_Angeles");
    }

    // Identify who we are and only allow for json response.
    m_headers.emplace("User-Agent", std::format("cpp-weather/{}", CPPWEATHER_VERSION));
    m_headers.emplace("Accept", "application/json");

    m_thread = std::jthread([this](std::stop_token st)
    {
        this->ThreadLoop(st);
    });
}

std::string Network::ParseContents(std::string content)
{
    auto json = nlohmann::json::parse(content);

    std::string tempUnits = json.at("current_units").at("temperature_2m").get<std::string>();
    double temperature = json.at("current").at("temperature_2m").get<double>();

    return std::format("temperature: {} {}", temperature, tempUnits);
}

std::string Network::GetWeather()
{
    auto result = m_client.Get(API_ENDPOINT, m_weatherParams, m_headers);
    if (result && result->status == httplib::StatusCode::OK_200)
    {
        auto type = result->get_header_value("Content-type");
        if (type.find("application/json") == std::string::npos)
        {
            return "No Json value returned.";
        }


        return this->ParseContents(result->body);
    }

    return result->body;
}

/*
* Return a shared pointer containing const weather results. Constness helps avoid
* data race condition; readers will have a reference to pointer. While writer atomically
* updates the pointer itself. Once weatherResults is updated readers will still have old shared
* pointer, eventually being destroyed.
*/
std::shared_ptr<const WeatherResults> Network::GetLatestWeather() const
{
    return m_weatherResults.load();
}

void Network::ForceRefresh()
{
    m_cv.notify_all();
}

void Network::ThreadLoop(std::stop_token stoptoken)
{
    auto untilNextUpdate{std::chrono::steady_clock::now()};
    while (!stoptoken.stop_requested())
    {
        untilNextUpdate += UPDATE_INTERVAL;

        // Process request.
        const auto wr = GetWeather();
        if (!wr.empty())
        {
            auto snapshot = std::make_shared<WeatherResults>(WeatherResults{
                .weather = wr,
                .timestamp = std::chrono::steady_clock::now()
                });

            m_weatherResults.store(snapshot);
        }

        // wait until next scheduled time or if thread is called to stop.
        std::unique_lock lck(m_cvMutex);
        m_cv.wait_until(lck, stoptoken, untilNextUpdate, [] {return false;});
    }
}
}// namespace Core
