#ifndef NETWORK_H
#define NETWORK_H

#include <thread>
#include <cpp-httplib/httplib.h>
#include <json/include/nlohmann/json.hpp>

#include "cppweather/cppweather.h"

namespace Core
{
struct WeatherResults
{
    std::string weather;
    std::chrono::steady_clock::time_point timestamp;
};

/*
* Handles connection to weather api; pulls weather information every minute.
* Http/Get is blocking, hence ran on a thread.
*/
class Network
{
private:
    const std::string API_ENDPOINT{"/v1/forecast"};
    const std::chrono::seconds UPDATE_INTERVAL{60};

    std::mutex m_cvMutex;
    std::condition_variable_any m_cv;

    httplib::Client m_client;
    httplib::Params m_weatherParams;
    httplib::Headers m_headers;

    std::atomic<std::shared_ptr<const WeatherResults>> m_weatherResults;

    std::jthread m_thread;

    std::string ParseContents(std::string content);
    std::string GetWeather();

    void ThreadLoop(std::stop_token st);

public:
    Network(const std::string& cli, const httplib::Params& params = {});
    ~Network() = default;

    std::shared_ptr<const WeatherResults> GetLatestWeather() const;
    void ForceRefresh();
};
}
#endif
