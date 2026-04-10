#ifndef APPSTATE_H
#define APPSTATE_H

#include "weatherdata.h"
#include "threadsafeslot.h"

namespace utility
{
struct AppState
{
    static AppState& Get()
    {
        static AppState instance;
        return instance;
    }

    ThreadSafeSlot weatherslot;
    WeatherData    currentweather = utility::MakeDefaultWeatherData();
};
}// namespace utility
#endif
