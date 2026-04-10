#include "core/network.h"

#include <chrono>
#include <memory>

#include <httplib.h>
#include <print>
#include <variant>

#include "networks/networklogging.h"
#include "utility/appstate.h"

namespace Core
{
Network::Network(std::optional<std::pair<std::string, std::string>> citystate)
{

#if ENABLE_LOGGING
    m_logging = std::make_shared<utility::LogFile>("resources/logging/log.txt");
#endif

    // if citystate is defined call geonetwork, else continue and call weathernetwork.
    if (citystate)
    {
        m_geonetwork = std::make_unique<network::GeoNetwork>(citystate->first, citystate->second);
        network::AttachLoggers(m_geonetwork->GetClient(), m_logging, "GEO");

        const auto location = m_geonetwork->GetGeographicCoordinates();
        if (location)
        {
            m_weathernetwork = std::make_unique<network::WeatherNetwork>(location.value());
        }
        else
        {
            std::visit([](const auto& error)
            {
                std::println("{}: {}", error.GetType(), error.GetMessage());
            },
            location.error());
        }
    }

    if (!m_weathernetwork)
    {
        m_weathernetwork = std::make_unique<network::WeatherNetwork>();
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
        const auto wr = m_weathernetwork->GetWeather();
        if (wr)
        {
            utility::AppState::Get().weatherslot.Set(wr.value());
        }
        else
        {
            std::visit([](const auto& error)
            {
                std::println("{}: {}", error.GetType(), error.GetMessage());
            },
            wr.error());
        }

        // wait until next scheduled time or if thread is called to stop.
        std::unique_lock lck(m_cvMutex);
        m_cv.wait_until(lck, stoptoken, untilNextUpdate, [&refresh = m_forceRefresh] {return refresh;});
        m_forceRefresh = false;
    }
}

/*
 * notify_all wakes up the thread, the thread then checks the predicate lambda holding 'm_forceRefresh'
 * if true exits wait and pulls weather data.
 */
void Network::ForceRefresh()
{
    std::lock_guard lck(m_cvMutex);
    m_forceRefresh = true;
    m_cv.notify_all();
}
}// namespace Core
