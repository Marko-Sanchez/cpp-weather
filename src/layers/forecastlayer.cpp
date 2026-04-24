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
    const Rectangle panel{m_screenWidth * k_Margin, m_screenHeight * k_HourlyY, (m_screenWidth * 6.0f) * k_Margin, m_screenHeight * k_HourlyHeight};
    DrawRectangleRounded(panel, k_PanelRoundness, k_PanelSegments, Fade(SKYBLUE, 0.3f));

    const auto xPadding{10.0f};
    const auto yPadding{10.0f};

    // Title.
    Vector2 titleSize     {MeasureTextEx(m_font, k_hourlyTitle.c_str(), k_FontSizeHourly, k_FontSpacing)};
    Vector2 titlePosition {panel.x + xPadding, panel.y + titleSize.y};
    DrawTextEx(m_font, k_hourlyTitle.c_str(), titlePosition, k_FontSizeHourly, k_FontSpacing, WHITE);

    // length of texts.
    const auto columnWidth{m_iconAtlas.GetIconSize()};
    const auto totalWidth {columnWidth * m_weatherData.hourlyForecast.size()};
    const auto maxScroll  {std::max(totalWidth - panel.width, 0.0f)};

    this->HandleScrolling(panel, m_isDraggingHourly);
    m_hourlyScrollOffset = std::clamp(m_hourlyScrollOffset, 0.0f, maxScroll);


    // Space where hour forecast will be displayed.
    auto areaTop      {titlePosition.y + yPadding};
    auto areaBottom   {panel.y + panel.height - yPadding};
    auto workingHeight{areaBottom - areaTop};

    auto section     {workingHeight/ 3.0f};
    auto centerTop   {areaTop + (section * 0.5f)};
    auto centerMiddle{areaTop + (section * 1.5f)};
    auto centerBottom{areaTop + (section * 2.5f)};

    const auto yTemp     {centerTop - (k_FontSizeHourly * 0.5f)};
    const auto yCondition{centerMiddle - (m_iconAtlas.GetIconSize() * 0.5f)};
    const auto yHour     {centerBottom - (k_FontSizeHourly * 0.5f)};

    const auto xStart {panel.x - m_hourlyScrollOffset};

    // Draw hourly forecast.
    BeginScissorMode(panel.x, panel.y, panel.width, panel.height);

        std::string buffer;
        for (size_t i{0}; i < m_weatherData.hourlyForecast.size(); ++i)
        {
            const auto& hour    = m_weatherData.hourlyForecast[i];
            const auto xCenter = xStart + (i * columnWidth) + (columnWidth / 2.0f);

            buffer = hour.temperature;
            Vector2 tempSize{MeasureTextEx(m_font, buffer.c_str(), k_FontSizeHourly, k_FontSpacing)};
            Vector2 tempPos{xCenter - (tempSize.x / 2.0f), yTemp};

            int tempVal = (buffer == "---") ? 0: std::stoi(buffer);
            DrawTextEx(m_font, buffer.c_str(), tempPos, k_FontSizeHourly, k_FontSpacing, GetTemperatureColor(tempVal));

            auto iconScale{1.0f};
            auto iconWidth{m_iconAtlas.GetIconSize() * iconScale};
            Vector2 iconPos{xCenter - (iconWidth / 2.0f), yCondition};
            m_iconAtlas.DrawWeatherIcon(hour.condition, iconPos, iconScale);

            buffer = hour.hour;
            Vector2 hourSize{MeasureTextEx(m_font, buffer.c_str(), k_FontSizeHourly, k_FontSpacing)};
            Vector2 hourPos{xCenter - (hourSize.x / 2.0f), yHour};
            DrawTextEx(m_font, buffer.c_str(), hourPos, k_FontSizeHourly, k_FontSpacing, WHITE);
        }

    EndScissorMode();

    // Scroll progress.
    if (maxScroll > 0.0f)
    {
        this->DrawHourScrollIndicator(panel, maxScroll, totalWidth);
    }
}

void ForecastLayer::DrawHourScrollIndicator(const Rectangle panel, float maxScroll, float totalWidth) const
{
    const float yPadding{8.0f};
    const float xPadding{10.0f};

    const float scrollPercent{m_hourlyScrollOffset / maxScroll};

    const float indicatorHeight{2.0f};
    const float indicatorWidth{(panel.width / totalWidth) * panel.width};
    const float indicatorX{panel.x + (scrollPercent * (panel.width - indicatorWidth))};
    const float indicatorY{panel.y + panel.height - yPadding};

    const float clampedX{std::clamp(indicatorX, panel.x + xPadding, panel.x + panel.width - indicatorWidth - xPadding)};
    const Rectangle bar{clampedX, indicatorY, indicatorWidth, indicatorHeight};

    DrawRectangleRounded(bar, k_PanelRoundness, k_PanelSegments, Fade(WHITE, 0.5f));
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
