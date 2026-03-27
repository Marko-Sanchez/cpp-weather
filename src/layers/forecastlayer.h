#ifndef FORECASTLAYER_H
#define FORECASTLAYER_H

#include <raylib.h>

#include "layer.h"
#include "utility/weatherdata.h"

namespace Layers
{
class ForecastLayer: public Layer
{
private:

    const int m_screenWidth;
    const int m_screenHeight;

    Font m_font;

    utility::WeatherData m_weatherData;

    // Draw Helpers.
    void DrawBackground() const;
    void DrawTitle() const;
    void DrawHourlyForecast() const;
    void DrawWeeklyForecast() const;

    float CenterX(float elementwidth) const noexcept;
public:

    ForecastLayer();
    ~ForecastLayer();

    void OnWeatherUpdate(const utility::WeatherData& data);

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
