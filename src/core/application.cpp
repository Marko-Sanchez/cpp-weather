#include "application.h"
#include "raylib.h"

#include <print>

namespace Core
{
Application::Application(const std::string windowname, const std::string version):
m_windowname(windowname),
m_applicationversion(version)
{}

void Application::GetWebContents()
{
    if (auto ptr{m_weatherclient.GetLatestWeather()}; ptr->timestamp != m_lastrequesttime)
    {
        std::println("{}", ptr->weather);
        m_lastrequesttime = ptr->timestamp;
    }
}

void Application::Run()
{
    const int screenWidth{512};
    const int screenHeight{1024};

    std::string titlename{std::format("{} (v{})", m_windowname, m_applicationversion)};
    InitWindow(screenWidth, screenHeight, titlename.c_str());

    Texture2D texture = LoadTexture("resources/images/squiggle2.png");

    int textureWidth{MeasureText("cppweather", 42)};
    int textX{(screenWidth - textureWidth) / 2};
    int textY{screenHeight / 2};

    SetTargetFPS(45);
    while (!WindowShouldClose())
    {

        // this->GetWebContents();

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(texture, screenWidth / 2 - texture.width / 2, 0, WHITE);
            DrawText("cppweather", textX, textY, 42, BLACK);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
}
}// namespace Core
