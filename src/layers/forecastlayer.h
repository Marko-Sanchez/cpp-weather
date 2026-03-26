#ifndef FORECASTLAYER_H
#define FORECASTLAYER_H

#include "layer.h"

#include <raylib.h>

namespace Layers
{
class ForecastLayer: public Layer
{
private:

    const int m_screenWidth;
    const int m_screenHeight;

    Font m_font;

    // Draw Helpers.
    void DrawBackground() const;
    void DrawTitle() const;
    void DrawHourlyForecast() const;
    void DrawWeeklyForecast() const;

    float CenterX(float elementwidth) const noexcept;
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
}// namespace Layers
#endif
