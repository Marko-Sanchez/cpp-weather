#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <mutex>
#include <optional>
#include "utility/weatherdata.h"

namespace utility
{
class ThreadSafeSlot
{
private:

    std::mutex  m_mutex;

    WeatherData m_data;
    bool        m_dirty{false};

public:

    void Set(WeatherData data)
    {
        std::lock_guard lock(m_mutex);
        m_data = std::move(data);
        m_dirty = true;
    }

    std::optional<WeatherData> TryConsume()
    {
        std::lock_guard lock(m_mutex);

        if (!m_dirty)
        {
            return std::nullopt;
        }
        m_dirty = false;

        return m_data;
    }
};
}// namespace utility
#endif
