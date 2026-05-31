#include "application.h"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <print>
#include <thread>

#include <raylib.h>

#include "utility/appsingleton.h"
#include "layers/titlelayer.h"

namespace Core
{
namespace
{
constexpr std::array<std::string_view, 8> k_Blocks =
{
"▁","▂","▃","▄","▅","▆","▇","█"
};

std::string UILine(std::span<const utility::HourlyForecast> hours)
{
    std::vector<int> values;
    values.reserve(hours.size());

    for (const auto& hour: hours)
    {
        int v;
        std::from_chars(hour.temperature.data(), hour.temperature.data() + hour.temperature.size(), v);
        values.emplace_back(v);
    }


    const auto [lo, hi] = std::ranges::minmax(values);
    const double range = static_cast<double>(hi - lo);

    std::string bar;
    for (int value: values)
    {
        double normalized {(range > 0) ? (value - lo) / range : 0.5};
        auto index {static_cast<std::size_t>(std::round(normalized * 7))};
        bar += k_Blocks[index];
    }

    return bar;
}
}// anonymous namespace

Application::Application(const std::string windowname, const std::string version, std::optional<std::pair<std::string, std::string>> stringlocation):
m_windowname(windowname),
m_applicationversion(version),
m_network(stringlocation)
{}

Application::~Application()
{
    m_layerstack.clear();
    if (IsWindowReady())
    {
        CloseWindow();
    }
}

/*
 * Invoked once when '--no-gui' is passed as an argument.
 */
void Application::GetWebContents()
{
    auto results = utility::AppSingleton::GetInstance().weatherslot.TryConsume();
    while (results == std::nullopt)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        results = utility::AppSingleton::GetInstance().weatherslot.TryConsume();
    }
    auto& weatherdata = results.value();

    std::println("┌─ {} ────────────────────────────────", weatherdata.location.city);
    std::println("│ Now: {:<10} High: {:<8} Low: {:<8}", weatherdata.currentTemperature, weatherdata.high, weatherdata.low);
    std::println("└────────────────────────────────────────────────┘");

    std::println("\n24h {} Hi {} -> Lo {}", UILine(weatherdata.hourlyForecast), weatherdata.hourlyForecast.front().temperature, weatherdata.hourlyForecast.back().temperature);
    std::println("{:>7}{:>11}{:>10}", weatherdata.hourlyForecast[0].hour, weatherdata.hourlyForecast[11].hour, weatherdata.hourlyForecast[23].hour);
}

void Application::Run()
{
    const int screenWidth{512};
    const int screenHeight{1024};

    std::string titlename{std::format("{} (v{})", m_windowname, m_applicationversion)};
    InitWindow(screenWidth, screenHeight, titlename.c_str());

    m_queuedtransition = std::make_shared<std::optional<Layers::TransitionLayer>>(std::nullopt);
    this->PushLayer<Layers::TitleLayer>();

    SetTargetFPS(45);

    float deltaTime{};
    while (!WindowShouldClose())
    {
        deltaTime = GetFrameTime();

        this->OnEvent();
        this->OnUpdate(deltaTime);
        this->RenderLayers();
    }
}

/*
 * Queue layer transition.
 */
auto Application::TransitionLayerLambda(std::list<uLayer>::iterator iter) -> std::function<void(uLayer)>
{
    return [queue = m_queuedtransition, currentIter = iter](std::unique_ptr<Layers::Layer> newlayer) mutable
    {
        queue->emplace(Layers::TransitionLayer {.pendinglayer = std::move(newlayer), .it = currentIter});
    };
}

void Application::ProcessTransition()
{
    if (!m_queuedtransition || !m_queuedtransition->has_value())
    {
        return;
    }

    auto& [newlayer, currentIter] = m_queuedtransition->value();

    // swap current layer with new one, by updating current layers position in list.
    *currentIter = std::move(newlayer);
    m_queuedtransition->reset();

    // set new layers callback.
    (*currentIter)->SetTransitionCallback(this->TransitionLayerLambda(currentIter));
}

/*
 * Retrieve data from thread safe queue.
*/
void Application::ProcessWeatherUpdate()
{
    auto data = utility::AppSingleton::GetInstance().weatherslot.TryConsume();
    if (!data)
    {
        return;
    }

    utility::AppSingleton::GetInstance().currentweather         = std::move(*data);
    utility::AppSingleton::GetInstance().currentweather.isStale = false;
}

/*
 * Calls layers event handlers.
 */
void Application::OnEvent()
{
    for (const auto& layer: m_layerstack)
    {
        layer->OnEvent();
    }
}

/*
 * Process layer transition and reset optional.
 */
void Application::OnUpdate(float delta)
{
    this->ProcessTransition();
    this->ProcessWeatherUpdate();

    for (const auto& layer: m_layerstack)
    {
        layer->OnUpdate(delta);
    }
}

/*
 * Calls layers draw calls.
 */
void Application::RenderLayers()
{
    for (const auto& layer: m_layerstack)
    {
        layer->OnRender();
    }
}
}// namespace Core
