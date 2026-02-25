#ifndef APPLICATION_H
#define APPLICATION_H

#include <chrono>
#include <string>
#include "core/network.h"

namespace Core
{
class Application
{
private:
    const std::string m_windowname;
    const std::string m_applicationversion;

    Core::Network m_weatherclient;
    std::chrono::steady_clock::time_point m_lastrequesttime;

public:
    Application(const std::string windowname, const std::string version);

    void GetWebContents();
    void Run();
};
}// namespace Core
#endif
