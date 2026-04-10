#ifndef NETWORK_H
#define NETWORK_H

#include <thread>

#include "utility/logfile.h"
#include "networks/geonetwork.h"
#include "networks/weathernetwork.h"

namespace Core
{
/*
* Handles connection to weather api; pulls weather information every minute.
* Http/Get is blocking, hence ran on a thread.
*/
class Network
{
private:

    const std::chrono::seconds UPDATE_INTERVAL{60};

    std::shared_ptr<utility::LogFile>        m_logging;
    std::unique_ptr<network::GeoNetwork>     m_geonetwork;
    std::unique_ptr<network::WeatherNetwork> m_weathernetwork;

    std::mutex                  m_cvMutex;
    std::condition_variable_any m_cv;

    std::jthread m_thread;

    void ThreadLoop(std::stop_token st);

public:

    Network(std::optional<std::pair<std::string, std::string>> citystate = {});
    ~Network() = default;

    void ForceRefresh();
};
}
#endif
