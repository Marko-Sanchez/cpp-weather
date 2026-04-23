#include "forecastlayer.h"

#include <algorithm>
#include <string>

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
    constexpr int k_FontSizeHourly{20};
    constexpr int k_FontSizeSmall{12};
    constexpr int k_FontSpacing{2};

    constexpr float k_Margin{1.0f / 8.0f};
    constexpr float k_TitleY{1.0f / 8.0f};
    constexpr float k_HourlyY{1.0f / 4.0f};
    constexpr float k_HourlyHeight{2.0f / 10.0f};
    constexpr float k_WeeklyY{1.0f / 2.0f};
    constexpr float k_WeeklyHeight{1.0f / 4.0f};

    constexpr float k_PanelRoundness{0.2f};
    constexpr int k_PanelSegments{0}; // 0 let raylib decide.

    constexpr std::string k_hourlyTitle{"Hourly forecast"};

    Color GetTemperatureColor(int temp)
    {
        if (temp >= 85) return Color{255, 69, 0, 255};   // Hot red-orange.
        if (temp >= 75) return Color{255, 140, 0, 255};  // Warm orange.
        if (temp >= 65) return Color{255, 215, 0, 255};  // Mild yellow.
        if (temp >= 55) return Color{255, 206, 235, 255};// Cool sky blue.

        return BLUE;                                     // Cold blue.
    }
}

ForecastLayer::ForecastLayer():
m_screenWidth(512),
m_screenHeight(1024),
m_layerScrollOffset(0.0f),
m_hourlyScrollOffset(0.0f),
m_weeklyScrollOffset(0.0f),
m_isDraggingHourly(false),
m_isDraggingWeekly(false),
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

void ForecastLayer::DrawHourlyForecast()
{
    // Panel background.
    const Rectangle rect{m_screenWidth * k_Margin, m_screenHeight * k_HourlyY, (m_screenWidth * 6.0f) * k_Margin, m_screenHeight * k_HourlyHeight};
    DrawRectangleRounded(rect, k_PanelRoundness, k_PanelSegments, Fade(SKYBLUE, 0.3f));

    const float xPadding{10.0f};

    // Title.
    Vector2 titlelength{MeasureTextEx(m_font, k_hourlyTitle.c_str(), k_FontSizeHourly, k_FontSpacing)};
    Vector2 titleV{rect.x + xPadding, rect.y + titlelength.y};
    DrawTextEx(m_font, k_hourlyTitle.c_str(), titleV, k_FontSizeHourly, k_FontSpacing, WHITE);

    const float columnWidth{65.0f};                                          // width of temp/hour/icon.
    const auto  totalWidth {columnWidth * m_weatherData.hourlyForecast.size()};
    const auto  maxScroll  {std::max(totalWidth - rect.width, 0.0f)};

    this->HandleScrolling(rect, m_isDraggingHourly);
    m_hourlyScrollOffset = std::clamp(m_hourlyScrollOffset, 0.0f, maxScroll);

    const float yTemp     {rect.y + rect.height * 0.25f};
    const float yCondition{rect.y + rect.height * 0.50f};
    const float yHour     {rect.y + rect.height * 0.75f};

    const float xStart {xPadding + rect.x - m_hourlyScrollOffset};

    // Draw hourly forecast.
    BeginScissorMode(rect.x, rect.y, rect.width, rect.height);

        std::string buffer;
        for (size_t i{0}; i < m_weatherData.hourlyForecast.size(); ++i)
        {
            const auto& hour   = m_weatherData.hourlyForecast[i];
            const float xCol   = xStart + (i * columnWidth);

            // TODO: if data is not loaded in time default '---' is used and throws error on stoi.
            buffer = hour.temperature;
            DrawTextEx(m_font, buffer.c_str(), Vector2{xCol, yTemp}, k_FontSizeHourly, k_FontSpacing, GetTemperatureColor(std::stoi(hour.temperature)));

            m_iconAtlas.DrawWeatherIcon(hour.condition, Vector2{xCol, yCondition}, 1.0f);

            buffer = hour.hour;
            DrawTextEx(m_font, buffer.c_str(), Vector2{xCol, yHour}, k_FontSizeHourly, k_FontSpacing, WHITE);
        }

    EndScissorMode();

    // Scroll progress.
    if (maxScroll > 0.0f)
    {
        const float scrollPercent{m_hourlyScrollOffset / maxScroll};
        const float indicatorWidth{(rect.width / totalWidth) * rect.width};
        const float indicatorX{rect.x + (scrollPercent * (rect.width - indicatorWidth))};

        const float yPadding{8.0f};
        const float indicatorY{rect.y + rect.height - yPadding};
        const float edgePadding{10.0f};

        const float clampedX{std::clamp(indicatorX, rect.x + edgePadding, rect.x + rect.width - indicatorWidth - edgePadding)};
        DrawRectangleRounded(Rectangle{clampedX, indicatorY, indicatorWidth, 2.0f}, k_PanelRoundness, k_PanelSegments, Fade(WHITE, 0.5f));
    }
}

void ForecastLayer::DrawWeeklyForecast()
{
    // Panel background.
    const Rectangle rect{m_screenWidth * k_Margin, m_screenHeight * k_WeeklyY, (m_screenWidth * 6.0f) * k_Margin, m_screenHeight * k_WeeklyHeight};
    DrawRectangleRounded(rect, k_PanelRoundness, k_PanelSegments, Fade(SKYBLUE, 0.6f));

    Vector2 weeklyV{rect.x + 10, rect.y - m_weeklyScrollOffset};
    Vector2 textSize{MeasureTextEx(m_font, "XX", k_FontSizeTemp, k_FontSpacing)};

    // Todays temperature.
    const std::string tday{"Today"};
    DrawTextEx(m_font, tday.c_str(), weeklyV, k_FontSizeTemp, k_FontSpacing, WHITE);
    DrawTextEx(m_font, m_weatherData.high.c_str(), Vector2{rect.x + rect.width - textSize.x, weeklyV.y}, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(std::stoi(m_weatherData.high)));
    DrawTextEx(m_font, m_weatherData.low.c_str(), Vector2{rect.x + rect.width * 0.50f, weeklyV.y}, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(std::stoi(m_weatherData.low)));


    weeklyV.y += k_FontSizeTemp;

    // DAY WEATHER_CODE LOW HIGH
    for (const auto& day: m_weatherData.weeklyForecast)
    {
        DrawTextEx(m_font, day.day.c_str(), weeklyV, k_FontSizeTemp, k_FontSpacing, WHITE);
        DrawTextEx(m_font, day.high.c_str(), Vector2{rect.x + rect.width - textSize.x, weeklyV.y}, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(std::stoi(day.high)));
        DrawTextEx(m_font, day.low.c_str(), Vector2{rect.x + rect.width * 0.50f, weeklyV.y}, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(std::stoi(day.low)));

        weeklyV.y += k_FontSizeTemp;
    }

}

void ForecastLayer::HandleScrolling(const Rectangle& rect, bool& isdragging)
{
    Vector2 currMousePos = GetMousePosition();

    if (CheckBounds(currMousePos, rect))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            isdragging = true;
            m_lastMousePos = currMousePos;
        }
    }

    if (isdragging)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            float deltaX{currMousePos.x - m_lastMousePos.x};
            m_hourlyScrollOffset -= deltaX;
            m_lastMousePos = currMousePos;
        }
        else
        {
            isdragging = false;
        }
    }
}
}// namespace Layers
