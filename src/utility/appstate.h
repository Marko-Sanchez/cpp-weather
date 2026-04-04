#ifndef APPSTATE_H
#define APPSTATE_H

#include "weatherdata.h"
#include "threadsafeslot.h"

namespace utility
{
struct AppSate
{
    static AppSate& Get()
    {
        static AppSate instance;
        return instance;
    }

    ThreadSafeSlot weatherslot;
    WeatherData    currentweather = utility::MakeDefaultWeatherData();
};
}// namespace utility
#endif
