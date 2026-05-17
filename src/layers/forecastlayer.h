#ifndef FORECASTLAYER_H
#define FORECASTLAYER_H

#include "layer.h"

#include <raylib.h>

#include "render/drawablecomponent.h"
#include "render/weathericonatlas.h"
#include "utility/weatherdata.h"
#include "utility/signal.h"

namespace Layers
{
class ForecastLayer: public Layer
{
    private:

        const int m_screenWidth;
        const int m_screenHeight;
        const int m_contentHeight;

        float   m_targetScrollOffset;
        float   m_layerScrollOffset;
        float   m_hourlyScrollOffset;
        float   m_weeklyScrollOffset;
        bool    m_isDraggingHourly;

        Font m_font;
        Vector2 m_lastMousePos;


        // reference to static variable.
        utility::WeatherData& m_weatherData;
        utility::Signal       m_signal;

        // texture atlas.
        render::WeatherIconAtlas m_iconAtlas;

        std::list<std::unique_ptr<render::DrawableComponent>> m_compositor;

        // Draw Helpers.
        void DrawBackground() const;
        void BuildTitle();
        void DrawHourlyForecast();
        void DrawWeeklyForecast();

        void HandleScrolling(const Rectangle& rect, bool& isDragging);
        void DrawHourScrollIndicator(const Rectangle panel, float maxScroll, float totalWidth) const;
        void DrawDayCard(const Rectangle card, const std::string& day, const std::string& high, const std::string& low, bool isToday);

        float CenterX(float elementwidth) const noexcept;
        bool CheckBounds(const Vector2& mouse, const Rectangle& bounds) const noexcept;

    public:

        ForecastLayer();
        ~ForecastLayer();

        void OnEvent() override;
        void OnUpdate(float deltatime) override;
        void OnRender() override;
};

inline float ForecastLayer::CenterX(float elementwidth) const noexcept
{
    return (m_screenWidth - elementwidth) / 2.0f;
}

inline bool ForecastLayer::CheckBounds(const Vector2& mouse, const Rectangle& rect) const noexcept
{
    return rect.x <= mouse.x &&
           mouse.x <= (rect.x + rect.width) &&
           rect.y <= mouse.y &&
           mouse.y <= (rect.y + rect.height);
}
}// namespace Layers
#endif
