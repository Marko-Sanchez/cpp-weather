#include "forecastlayer.h"

#include <raylib.h>

#include "layers/aboutlayer.h"
#include "utility/weatherdata.h"
#include "utility/appstate.h"

namespace Layers
{
namespace
{
    constexpr int k_FontSizeTitle{42};
    constexpr int k_FontSizeTemp{32};
    constexpr int k_FontSizeHighLow{16};
    constexpr int k_FontSpacing{2};

    constexpr float k_Margin{1.0f / 8.0f};
    constexpr float k_TitleY{1.0f / 8.0f};
    constexpr float k_HourlyY{1.0f / 4.0f};
    constexpr float k_HourlyHeight{1.0f / 8.0f};
    constexpr float k_WeeklyY{1.0f / 2.0f};
    constexpr float k_WeeklyHeight{1.0f / 4.0f};

    constexpr float k_PanelRoundness{0.2f};
    constexpr int k_PanelSegments{0}; // 0 let raylib decide.
}

ForecastLayer::ForecastLayer():
m_screenWidth(512),
m_screenHeight(1024),
m_weatherData(utility::AppState::Get().currentweather)
{
    m_font = LoadFont("resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf");
}

ForecastLayer::~ForecastLayer()
{
    if (m_font.texture.id > 0 && m_font.texture.id != GetFontDefault().texture.id)
    {
        UnloadFont(m_font);
    }
}

void ForecastLayer::OnEvent()
{
    if (IsKeyPressed(KEY_ONE))
    {
        TransitionTo<Layers::AboutLayer>();
    }

    // TODO: handle scrolling through hourly / daily forecast.
}

void ForecastLayer::OnUpdate(float deltatime)
{
    m_weatherData = utility::AppState::Get().currentweather;
}

void ForecastLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        this->DrawBackground();
        this->DrawTitle();
        this->DrawHourlyForecast();
        this->DrawWeeklyForecast();
    EndDrawing();
}

void ForecastLayer::DrawBackground() const
{
    DrawRectangleGradientV(0, 0, m_screenWidth, m_screenHeight, BLUE, SKYBLUE);
}

void ForecastLayer::DrawTitle() const
{
    const std::string highlow{std::format("H: {} L: {}", m_weatherData.high, m_weatherData.low)};

    // Location.
    const Vector2 titlesize{MeasureTextEx(m_font, m_weatherData.location.city.c_str(), k_FontSizeTitle, k_FontSpacing)};
    const Vector2 titlepos{CenterX(titlesize.x), m_screenHeight * k_TitleY};

    // Current temperature.
    const Vector2 tempsize{MeasureTextEx(m_font, m_weatherData.currentTemperature.c_str(), k_FontSizeTemp, k_FontSpacing)};
    const Vector2 hlsize{MeasureTextEx(m_font, highlow.c_str(), k_FontSizeHighLow, k_FontSpacing)};

    const Vector2 temppos{CenterX(tempsize.x), titlepos.y + titlesize.y};
    const Vector2 hlpos{CenterX(hlsize.x), temppos.y + tempsize.y};

    DrawTextEx(m_font, m_weatherData.location.city.c_str(), titlepos, k_FontSizeTitle, k_FontSpacing, WHITE);
    DrawTextEx(m_font, m_weatherData.currentTemperature.c_str(), temppos, k_FontSizeTemp, k_FontSpacing, WHITE);
    DrawTextEx(m_font, highlow.c_str(), hlpos, k_FontSizeHighLow, k_FontSpacing, WHITE);
}

void ForecastLayer::DrawHourlyForecast() const
{
    const Rectangle rect{m_screenWidth * k_Margin, m_screenHeight * k_HourlyY, (m_screenWidth * 6.0f) * k_Margin, m_screenHeight * k_HourlyHeight};
    DrawRectangleRounded(rect, k_PanelRoundness, k_PanelSegments, Fade(SKYBLUE, 0.3f));
}

void ForecastLayer::DrawWeeklyForecast() const
{
    const Rectangle rect{m_screenWidth * k_Margin, m_screenHeight * k_WeeklyY, (m_screenWidth * 6.0f) * k_Margin, m_screenHeight * k_WeeklyHeight};
    DrawRectangleRounded(rect, k_PanelRoundness, k_PanelSegments, Fade(SKYBLUE, 0.6f));
}
}// namespace Layers
