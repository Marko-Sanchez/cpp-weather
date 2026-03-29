#include "core/network.h"

#include <chrono>
#include <memory>

#include <httplib.h>

#include "networks/networklogging.h"
#include "utility/weatherdata.h"

namespace Core
{
Network::Network(std::shared_ptr<utility::ThreadSafeSlot> queue, std::optional<std::pair<std::string, std::string>> citystate):
m_queue(queue)
{
    // if citystate is defined call geonetwork, else continue and call weathernetwork.
    if (citystate)
    {
        m_geonetwork = std::make_unique<network::GeoNetwork>(citystate->first, citystate->second);
        if (const auto location = m_geonetwork->GetCoordinates(); location)
        {
            m_weathernetwork = std::make_unique<network::WeatherNetwork>(location.value());
        }
    }

    if (!m_weathernetwork)
    {
        m_weathernetwork = std::make_unique<network::WeatherNetwork>();
    }

#if ENABLE_LOGGING
    m_logging = std::make_shared<utility::LogFile>("resources/logging/log.txt");
#endif

    // note: the above geo call is made before logging is performed.
    if (m_geonetwork)
    {
        network::AttachLoggers(m_geonetwork->GetClient(), m_logging, "GEO");
    }
    network::AttachLoggers(m_weathernetwork->GetClient(), m_logging, "WEATHER");

    m_thread = std::jthread([this](std::stop_token st)
    {
        this->ThreadLoop(st);
    });
}

void Network::ThreadLoop(std::stop_token stoptoken)
{
    auto untilNextUpdate{std::chrono::steady_clock::now()};
    while (!stoptoken.stop_requested())
    {
        untilNextUpdate += UPDATE_INTERVAL;

        // Process request.
        if (const auto wr = m_weathernetwork->GetWeather(); wr)
        {
            m_queue->Set(wr.value());
        }

        // wait until next scheduled time or if thread is called to stop.
        std::unique_lock lck(m_cvMutex);
        m_cv.wait_until(lck, stoptoken, untilNextUpdate, [] {return false;});
    }
}

void Network::ForceRefresh()
{
    m_cv.notify_all();
}
}// namespace Core
