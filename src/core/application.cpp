#include "application.h"

#include <chrono>
#include <print>
#include <thread>

#include <raylib.h>

#include "layers/forecastlayer.h"
#include "layers/titlelayer.h"

namespace Core
{
Application::Application(const std::string windowname, const std::string version, std::optional<std::pair<std::string, std::string>> stringlocation):
m_windowname(windowname),
m_applicationversion(version),
m_weatherSlot(std::make_shared<utility::ThreadSafeSlot>()),
m_network(m_weatherSlot, stringlocation)
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
    auto results = m_weatherSlot->TryConsume();
    while (results == std::nullopt)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        results = m_weatherSlot->TryConsume();
    }

    std::println("Current temperature {}", results.value().currentTemperature);
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
 * Search for forecast layer if it exist, consume weather data.
*/
void Application::ProcessWeatherUpdate()
{
    for (const auto& layer: m_layerstack)
    {
        if (auto weatherlayer = dynamic_cast<Layers::ForecastLayer*>(layer.get()))
        {
            // note: transitioning back to layer will consume data.
            if (auto data = m_weatherSlot->TryConsume())
            {
                weatherlayer->OnWeatherUpdate(data.value());
            }
        }
    }
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
