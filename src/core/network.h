#ifndef NETWORK_H
#define NETWORK_H

#include <atomic>
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
    std::chrono::steady_clock::time_point m_nextUpdate = std::chrono::steady_clock::now();

    std::mutex m_weatherMutex;
    std::atomic<bool> m_isRunning;

    httplib::Client m_client;
    httplib::Params m_weatherParams;
    httplib::Headers m_headers;

    WeatherResults m_weatherResults;

    std::jthread m_thread;

    std::string ParseContents(std::string content);
    std::string GetWeather();

    void ThreadLoop();

public:
    Network(const std::string& cli, const httplib::Params& params = {});
    ~Network();

    WeatherResults GetLatestWeather();
    void CloseConnection();
};
}
#endif
