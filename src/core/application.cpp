#include "application.h"

#include <raylib.h>
#include <print>

#include "layers/TitleLayer.h"

namespace Core
{
Application::Application(const std::string windowname, const std::string version):
m_windowname(windowname),
m_applicationversion(version)
{}

Application::~Application()
{
    m_layerstack.clear();
    CloseWindow();
}

/*
 * Invoked once when '--no-gui' is passed as an argument.
 */
void Application::GetWebContents()
{
    if (auto ptr{m_weatherclient.GetLatestWeather()}; ptr)
    {
        std::println("{}", ptr->weather);
    }
}

void Application::Run()
{
    const int screenWidth{512};
    const int screenHeight{1024};

    std::string titlename{std::format("{} (v{})", m_windowname, m_applicationversion)};
    InitWindow(screenWidth, screenHeight, titlename.c_str());
    this->PushLayer<Layers::TitleLayer>();

    SetTargetFPS(45);
    while (!WindowShouldClose())
    {
        this->RenderLayers();
    }
}

void Application::RenderLayers()
{
    for (const auto& layer: m_layerstack)
    {
        layer->OnRender();
    }
}
}// namespace Core
