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

        Font m_font;

        // reference to static variable.
        utility::WeatherData& m_weatherData;
        utility::Signal       m_signal;

        render::WeatherIconAtlas m_iconAtlas;

        std::list<std::unique_ptr<render::DrawableComponent>> m_compositor;

        void DrawBackground() const;

        void BuildTitle();
        void BuildHourlyForecast();
        void BuildWeeklyForecast();

    public:

        ForecastLayer();
        ~ForecastLayer();

        void OnEvent() override;
        void OnUpdate(float deltatime) override;
        void OnRender() override;
};
}// namespace Layers
#endif
