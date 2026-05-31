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
#include "render/weekpanelelement.h"
#include "utility/weatherdata.h"
#include "utility/appsingleton.h"

namespace Layers
{
namespace
{
    constexpr int k_FontSizeTitle   {42};
    constexpr int k_FontSizeTemp    {32};
    constexpr int k_FontSizeHighLow {16};
    constexpr int k_FontSizeHourly  {20};
    constexpr int k_FontSpacing     {2};

    constexpr float k_Margin       {1.0f / 8.0f};
    constexpr float k_TitleY       {1.0f / 20.0f};
    constexpr float k_HourlyY      {1.0f / 6.0f};
    constexpr float k_HourlyHeight {2.0f / 10.0f};
    constexpr float k_WeeklyY      {4.0f / 10.0f};

    constexpr float k_WheelMultiplier {60.0f};
    constexpr float k_ScrollSpeed     {120.0f};
    constexpr float k_ScrollSmooth    {12.0f};

    constexpr Color k_Bluish  {59, 131, 223, 255};
    constexpr Color k_Greyish {255, 255, 255, 200};

    constexpr std::string_view k_HourlyTitle {"24-hour forecast"};
    constexpr std::string_view k_FontPath    {"resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf"};
    constexpr std::string_view k_Footer      {"[1] Next | Scroll Wheel / Arrow keys to navigate"};
}// anonymous namespace

ForecastLayer::ForecastLayer():
m_screenWidth(GetScreenWidth()),
m_screenHeight(GetScreenHeight()),
m_contentHeight(1500),
m_targetScrollOffset(0.0f),
m_layerScrollOffset(0.0f),
m_weatherData(utility::AppSingleton::GetInstance().currentweather)
{
    m_font = LoadFont(k_FontPath.data());

    this->BuildTitle();
    this->BuildHourlyForecast();
    this->BuildWeeklyForecast();

    m_compositor.emplace_back
    (
     std::make_unique<render::TextElement>
     (
      &m_font, k_Footer, 10.0f, m_screenHeight - 28.0f, m_screenWidth, 10.0f, k_FontSpacing, k_Greyish, false
     )
    );
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
        m_targetScrollOffset -= wheel * k_WheelMultiplier;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        m_targetScrollOffset += k_ScrollSpeed * GetFrameTime();
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        m_targetScrollOffset -= k_ScrollSpeed * GetFrameTime();
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
    if (!utility::AppSingleton::GetInstance().currentweather.isStale)
    {
        m_weatherData = utility::AppSingleton::GetInstance().currentweather;
        m_signal.Broadcast();

        utility::AppSingleton::GetInstance().currentweather.isStale = true;
    }

    m_layerScrollOffset = std::lerp(m_layerScrollOffset, m_targetScrollOffset, k_ScrollSmooth * deltatime);
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
    const Vector2 titlePos{(m_screenWidth - titleSize.x) * 0.5f, m_screenHeight * k_TitleY};
    const Vector2 tempPos {(m_screenWidth - tempSize.x) * 0.5f, titlePos.y + titleSize.y};
    const Vector2 hlPos   {(m_screenWidth - hlSize.x) * 0.5f, tempPos.y + tempSize.y};

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
        m_screenWidth  * k_Margin * 6.0f,
        m_screenHeight * k_HourlyHeight
    };

    const Vector2 titleSize     {MeasureTextEx(m_font, k_HourlyTitle.data(), k_FontSizeHourly, k_FontSpacing)};
    const Vector2 titlePosition {panel.x, panel.y + titleSize.y};

    m_compositor.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
          std::make_unique<render::PanelDecorator>
          (
             std::make_unique<render::TextElement>
             (
              &m_font, k_HourlyTitle, titlePosition.x, titlePosition.y, panel.width, k_FontSizeHourly, k_FontSpacing, WHITE
             ),
             Fade(GRAY, 0.70f), true
          ),
         m_screenWidth, m_screenHeight
     )
    );

    auto titlePanel = m_compositor.back()->GetBounds();

    auto hourFunc = [&hourlyForecast = std::as_const(m_weatherData.hourlyForecast)](void) {return hourlyForecast;};
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
             k_Bluish, true
         ),
         m_screenWidth, m_screenHeight
     )
    );
}

void ForecastLayer::BuildWeeklyForecast()
{
    float xStart     {m_screenWidth * k_Margin};
    float yStart     {m_screenHeight * k_WeeklyY};
    float cardWidth  {m_screenWidth * (6.0f / 8.0f)};
    float cardHeight {64.0f};

    auto weekFunc = [&forecast = std::as_const(m_weatherData)] (void)
    {
        std::vector<utility::DailyForecast> vec;
        vec.reserve(1 + forecast.weeklyForecast.size());
        vec.emplace_back("Today", forecast.currentTemperature,forecast.high, forecast.low, forecast.condition);
        vec.insert(vec.end(), forecast.weeklyForecast.cbegin(), forecast.weeklyForecast.cend());

        return vec;
    };

    m_compositor.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::WeekPanelElement>
         (
         weekFunc(), weekFunc, m_signal,
         &m_font, xStart, yStart, cardWidth, cardHeight, k_Bluish
         ),
         m_screenWidth, m_screenHeight
     )
    );
}
}// namespace Layers
