#include "application.h"

#include <print>
#include <thread>
#include <chrono>

namespace Core
{
Application::Application(const std::string& host, const int port):
cli(host)
{}

void Application::Connect()
{
    auto res = cli.Get("API_REQUEST");
    std::println("status {}", res->status);

    std::this_thread::sleep_for(std::chrono::seconds(16));
}
}// namespace Core
