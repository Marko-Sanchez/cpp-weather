#ifndef FORECASTLAYER_H
#define FORECASTLAYER_H

#include <raylib.h>

#include "layer.h"
#include "render/weathericonatlas.h"
#include "utility/weatherdata.h"

namespace Layers
{
class ForecastLayer: public Layer
{
private:

    const int m_screenWidth;
    const int m_screenHeight;

    Vector2 m_lastMousePos;

    float   m_layerScrollOffset;
    float   m_hourlyScrollOffset;
    float   m_weeklyScrollOffset;
    bool    m_isDraggingHourly;
    bool    m_isDraggingWeekly;

    Font m_font;

    // reference to static variable.
    utility::WeatherData& m_weatherData;

    // texture atlas.
    render::WeatherIconAtlas m_iconAtlas;

    // Draw Helpers.
    void DrawBackground() const;
    void DrawTitle() const;
    void DrawHourlyForecast();
    void DrawWeeklyForecast();

    void HandleScrolling(const Rectangle& rect, bool& isdragging);

    float CenterX(float elementwidth) const noexcept;
    bool CheckBounds(const Vector2& mouse, const Rectangle& bounds) const noexcept;

public:

    ForecastLayer();
    ~ForecastLayer();

    virtual void OnEvent() override;
    virtual void OnUpdate(float deltatime) override;
    virtual void OnRender() override;
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
