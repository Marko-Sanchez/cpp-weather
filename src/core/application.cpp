#include "application.h"

#include <chrono>
#include <print>
#include <thread>

namespace Core
{
Application::Application(const std::string& host, const int port):
m_weatherClient(host)
{}

void Application::GetWebContents()
{
    // Temoprary: to fast for thread to process data.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto ptr = m_weatherClient.GetLatestWeather();
    if (ptr)
    {
        std::println("{}", ptr->weather);
    }
}

void Application::Run()
{
    // bool isHandled{false};
    // while (!isHandled) // If a certain key is pressed: Exit.
    // {
        // Get Network Results...
        // Output every minute, returns a bool if still in
        // timer.
        // this->GetWebContents();

        // If isHeadless not true draw gui.
        // isHandled = true;
    // }
}
}// namespace Core
