#ifndef APPSINGLETON_H
#define APPSINGLETON_H

#include "weatherdata.h"
#include "threadsafeslot.h"

namespace utility
{
/*
 * Ensures class has only one instance, by making the class itself
 * responsible for keeping track of the instance. This is done via
 * GetInstance() which creates the instance once and making the constructor
 * private so it can only be accessed by GetInstance(), ensuring one copy.
 *
 * Singleton Design pattern.
 */
class AppSingleton
{
    public:

        // Thread-safe
        static AppSingleton& GetInstance()
        {
            static AppSingleton instance;
            return instance;
        }

        ThreadSafeSlot weatherslot;
        WeatherData    currentweather = utility::MakeDefaultWeatherData();

        AppSingleton(const AppSingleton&)            = delete;
        AppSingleton(AppSingleton&&)                 = delete;
        AppSingleton& operator=(const AppSingleton&) = delete;
        AppSingleton& operator=(AppSingleton&&)      = delete;

    private:

        // Only GetInstance() can call the constructor.
        AppSingleton() = default;
};
}// namespace utility
#endif
