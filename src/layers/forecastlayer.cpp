#include "forecastlayer.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>

#include <raylib.h>

#include "layers/aboutlayer.h"
#include "render/cullingdecorator.h"
#include "render/dynamictextelement.h"
#include "render/hourlystripelement.h"
#include "render/paneldecorator.h"
#include "render/textelement.h"
#include "utility/weatherdata.h"
#include "utility/appstate.h"

namespace Layers
{
namespace
{
    constexpr int k_FontSizeTitle   {42};
    constexpr int k_FontSizeTemp    {32};
    constexpr int k_FontSizeHighLow {16};
    constexpr int k_FontSizeHourly  {20};
    constexpr int k_FontSizeSmall   {12};
    constexpr int k_FontSpacing     {2};

    constexpr float k_Margin       {1.0f / 8.0f};
    constexpr float k_TitleY       {1.0f / 20.0f};
    constexpr float k_HourlyY      {1.0f / 6.0f};
    constexpr float k_HourlyHeight {2.0f / 10.0f};
    constexpr float k_WeeklyY      {4.0f / 10.0f};
    constexpr float k_WeeklyHeight {1.0f / 4.0f};

    constexpr float k_PanelRoundness {0.2f};
    constexpr int k_PanelSegments    {0}; // 0 let raylib decide.

    constexpr float k_wheelMultiplier {60.0f};
    constexpr float k_scrollSpeed     {120.0f};
    constexpr float k_scrollSmooth    {12.0f};

    constexpr std::string_view k_hourlyTitle {"24-hour forecast"};

    constexpr std::string_view k_fontPath {"resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf"};

    Color GetTemperatureColor(int temp)
    {
        if (temp >= 85) return Color {255, 69, 0, 255};  // Hot red-orange.
        if (temp >= 75) return Color {255, 140, 0, 255}; // Warm orange.
        if (temp >= 65) return Color {255, 215, 0, 255}; // Mild yellow.
        if (temp >= 55) return Color {99, 153, 34, 255}; // Green.

        return BLUE;                                     // Cold blue.
    }
}

ForecastLayer::ForecastLayer():
m_screenWidth(GetScreenWidth()),
m_screenHeight(GetScreenHeight()),
m_contentHeight(1500),
m_targetScrollOffset(0.0f),
m_layerScrollOffset(0.0f),
m_hourlyScrollOffset(0.0f),
m_weeklyScrollOffset(0.0f),
m_isDraggingHourly(false),
m_weatherData(utility::AppState::Get().currentweather)
{
    m_font = LoadFont(k_fontPath.data());

    this->BuildTitle();
    this->BuildHourlyForecast();
}

ForecastLayer::~ForecastLayer()
{
    m_compositor.clear();

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

    if (float wheel {GetMouseWheelMove()}; wheel != 0)
    {
        m_targetScrollOffset -= wheel * k_wheelMultiplier;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        m_targetScrollOffset += k_scrollSpeed * GetFrameTime();
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        m_targetScrollOffset -= k_scrollSpeed * GetFrameTime();
    }

    m_targetScrollOffset = std::clamp(m_targetScrollOffset, 0.0f, static_cast<float>(m_contentHeight));
    for (auto& comp: m_compositor)
    {
        comp->OnEvent();
    }
}

void ForecastLayer::OnUpdate(float deltatime)
{
    // isStale gets set in Application::ProcessWeatherUpdate().
    if (!utility::AppState::Get().currentweather.isStale)
    {
        m_weatherData = utility::AppState::Get().currentweather;
        m_signal.Broadcast();

        utility::AppState::Get().currentweather.isStale = true;
    }

    m_layerScrollOffset = std::lerp(m_layerScrollOffset, m_targetScrollOffset, k_scrollSmooth * deltatime);
    for (auto& comp: m_compositor)
    {
        comp->OnUpdate(m_layerScrollOffset);
    }
}

void ForecastLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        this->DrawBackground();
        this->DrawWeeklyForecast();

        for (auto& comp: m_compositor)
        {
            comp->OnRender();
        }
    EndDrawing();
}

void ForecastLayer::DrawBackground() const
{
    DrawRectangleGradientV(0, 0, m_screenWidth, m_screenHeight, BLUE, SKYBLUE);
}

void ForecastLayer::BuildTitle()
{
    const auto m_highlow = std::format("H: {} L: {}", m_weatherData.high, m_weatherData.low);

    // Current temperature.
    const Vector2 tempSize {MeasureTextEx(m_font, m_weatherData.currentTemperature.c_str(), k_FontSizeTemp, k_FontSpacing)};
    const Vector2 hlSize   {MeasureTextEx(m_font, m_highlow.c_str(), k_FontSizeHighLow, k_FontSpacing)};
    const Vector2 titleSize{MeasureTextEx(m_font, m_weatherData.location.city.c_str(), k_FontSizeTitle, k_FontSpacing)};

    // Anchor point for title.
    const Vector2 titlePos{CenterX(titleSize.x), m_screenHeight * k_TitleY};
    const Vector2 tempPos {CenterX(tempSize.x), titlePos.y + titleSize.y};
    const Vector2 hlPos   {CenterX(hlSize.x), tempPos.y + tempSize.y};

    auto cityFunc = [&city = m_weatherData.location.city]                 (void) {return city;};
    auto tempFunc = [&temperature = m_weatherData.currentTemperature]     (void) {return temperature;};
    auto hlFunc   = [&high = m_weatherData.high, &low = m_weatherData.low](void) {return std::format("H: {} L: {}", high, low);};

    m_compositor.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::DynamicTextElement>
         (
          cityFunc, m_signal,
          &m_font, m_weatherData.location.city, titlePos.x, titlePos.y, m_screenWidth, k_FontSizeTitle, k_FontSpacing, WHITE
         ),
         m_screenWidth, m_screenHeight
     )
    );

    m_compositor.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::DynamicTextElement>
         (
          tempFunc, m_signal,
          &m_font, m_weatherData.currentTemperature, tempPos.x, tempPos.y, m_screenWidth, k_FontSizeTemp, k_FontSpacing, WHITE
         ),
         m_screenWidth, m_screenHeight
     )
    );

    m_compositor.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::DynamicTextElement>
         (
          hlFunc, m_signal,
          &m_font, m_highlow, hlPos.x, hlPos.y, m_screenWidth, k_FontSizeHighLow, k_FontSpacing, WHITE
         ),
         m_screenWidth, m_screenHeight
     )
    );
}

void ForecastLayer::BuildHourlyForecast()
{
    const Rectangle panel
    {
        m_screenWidth  * k_Margin,
        m_screenHeight * k_HourlyY,
        m_screenWidth  * 6.0f * k_Margin,
        m_screenHeight * k_HourlyHeight
    };

    const Vector2 titleSize     {MeasureTextEx(m_font, k_hourlyTitle.data(), k_FontSizeHourly, k_FontSpacing)};
    const Vector2 titlePosition {panel.x, panel.y + titleSize.y};

    m_compositor.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
          std::make_unique<render::PanelDecorator>
          (
             std::make_unique<render::TextElement>
             (
              &m_font, k_hourlyTitle, titlePosition.x, titlePosition.y, panel.width, k_FontSizeHourly, k_FontSpacing, WHITE
             ),
             Fade(GRAY, 0.75f), true
          ),
         m_screenWidth, m_screenHeight
     )
    );

    auto titlePanel = m_compositor.back()->GetBounds();

    auto hourFunc = [&hourlyForecast = m_weatherData.hourlyForecast](void) {return hourlyForecast;};
    auto hourHeight {panel.height - titlePanel.height};

    m_compositor.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::PanelDecorator>
         (
             std::make_unique<render::HourlyStripElement>
             (
              m_weatherData.hourlyForecast, hourFunc, m_signal, &m_iconAtlas,
              &m_font, panel.x, titlePanel.y + titlePanel.height, panel.width, hourHeight, k_FontSizeHourly, k_FontSpacing, WHITE
             ),
             Fade(BLACK, 0.75f), true
         ),
         m_screenWidth, m_screenHeight
     )
    );
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
    // Anchor point for weekly forecast.
    auto xStart{m_screenWidth * k_Margin};
    auto yStart{m_screenHeight * k_WeeklyY - m_layerScrollOffset};

    auto cardWidth{m_screenWidth * (6.0f / 8.0f)};
    auto cardHeight{60.0f};
    auto cardSpacing{12.0f};

    Rectangle card
    {
        xStart,
        yStart,
        cardWidth,
        cardHeight
    };
    this->DrawDayCard(card, "Today", m_weatherData.high, m_weatherData.low, true);
    card.y += card.height + cardSpacing;

    for (const auto& day: m_weatherData.weeklyForecast)
    {
        this->DrawDayCard(card, day.day, day.high, day.low, false);
        card.y += card.height + cardSpacing;
    }
}

void ForecastLayer::DrawDayCard(const Rectangle card, const std::string& day, const std::string& high, const std::string& low, bool isToday)
{
    int hightemp {isdigit(high.front()) ? std::stoi(high) : 0};
    int lowtemp  {isdigit(low.front()) ? std::stoi(low) : 0};
    float padding{12.0f};
    float tempColumnWidth{64.0f};

    // card background.
    Color cardBg{Fade(BLACK, 0.75f)};
    DrawRectangleRounded(card, k_PanelRoundness, k_PanelSegments, cardBg);

    auto dayTextSize{MeasureTextEx(m_font, day.c_str(), k_FontSizeTemp, k_FontSpacing)};
    Vector2 dayPos
    {
        card.x + padding,
        card.y + (card.height - dayTextSize.y) * 0.5f
    };
    DrawTextEx(m_font, day.c_str(), dayPos, k_FontSizeTemp, k_FontSpacing, WHITE);

    if (isToday)
    {
        const float badgeWidth{40.0f};
        const float badgeHeight{16.0f};
        const float badgeFontSize{12.0f};

        Rectangle badge
        {
            card.x + dayTextSize.x + padding,
            card.y + card.height * 0.5f,
            40,
            16
        };
        DrawRectangleRounded(badge, k_PanelRoundness, k_PanelSegments, Fade(BLUE, 0.3f));

        auto badgeTextSize{MeasureTextEx(m_font, "NOW", badgeFontSize, k_FontSpacing)};
        Vector2 badgeTextPos
        {
            badge.x + (badgeWidth - badgeTextSize.x) * 0.5f,
            badge.y + (badgeHeight - badgeTextSize.y) * 0.5f
        };
        DrawTextEx(m_font, "NOW", badgeTextPos, badgeFontSize, k_FontSpacing, BLUE);
    }

    // Temperature.
    const auto labelFontSize{12.0f};
    const auto highTextSize {MeasureTextEx(m_font, high.c_str(), k_FontSizeTemp, k_FontSpacing)};
    const auto lowTextSize  {MeasureTextEx(m_font, low.c_str(), k_FontSizeTemp, k_FontSpacing)};
    const auto highLabelSize{MeasureTextEx(m_font, "HIGH", labelFontSize, k_FontSpacing)};
    const auto lowLabelSize {MeasureTextEx(m_font, "LOW", labelFontSize, k_FontSpacing)};

    float highColumnX{card.x + card.width - (tempColumnWidth * 2.0f) - padding};
    Vector2 highLabelPos
    {
        highColumnX + (tempColumnWidth - highLabelSize.x) * 0.5f,
        card.y + 8.0f
    };
    Vector2 highTempPos
    {
        highColumnX + (tempColumnWidth - highTextSize.x) * 0.5f,
        card.y + card.height - highTextSize.y - 8.0f
    };
    DrawTextEx(m_font, "HIGH", highLabelPos, labelFontSize, k_FontSpacing, WHITE);
    DrawTextEx(m_font, high.c_str(), highTempPos, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(hightemp));

    float lowColumnX{card.x + card.width - tempColumnWidth - padding};
    Vector2 lowLabelPos
    {
        lowColumnX + (tempColumnWidth - lowLabelSize.x) * 0.5f,
        card.y + 8.0f
    };
    Vector2 lowTempPos
    {
        lowColumnX + (tempColumnWidth - lowTextSize.x) * 0.5f,
        card.y + card.height - lowTextSize.y - 8.0f
    };
    DrawTextEx(m_font, "LOW", lowLabelPos, labelFontSize, k_FontSpacing, WHITE);
    DrawTextEx(m_font, low.c_str(), lowTempPos, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(lowtemp));
}

void ForecastLayer::HandleScrolling(const Rectangle& rect, bool& isDragging)
{
    Vector2 currMousePos = GetMousePosition();

    if (CheckBounds(currMousePos, rect))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            isDragging = true;
            m_lastMousePos = currMousePos;
        }
    }

    if (isDragging)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            float deltaX{currMousePos.x - m_lastMousePos.x};
            m_hourlyScrollOffset -= deltaX;
            m_lastMousePos = currMousePos;
        }
        else
        {
            isDragging = false;
        }
    }
}
}// namespace Layers
