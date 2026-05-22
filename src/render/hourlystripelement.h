#ifndef HOURLYSTRIPELEMENT_H
#define HOURLYSTRIPELEMENT_H

#include "drawablecomponent.h"

#include <functional>

#include "render/weathericonatlas.h"
#include "utility/signal.h"
#include "utility/weatherdata.h"

namespace render
{
class HourlyStripElement: public DrawableComponent
{
    private:

        using HourArray = std::array<utility::HourlyForecast, 24>;

        HourArray _forecast;

        std::function<HourArray()> _provider;
        utility::Signal::Token     _token;

        const render::WeatherIconAtlas* _weatherIcons;
        const Font*                     _font;

        const float _xPosition;
        const float _yPosition;
        const float _viewWidth;
        const float _viewHeight;
        const float _fontSize;
        const float _fontSpacing;
        Color       _color;

        float   _yParentScrollOffset;
        float   _horizontalScrollOffset;
        Vector2 _lastMousePos;
        bool    _isDragging;

        void RefreshText();

    public:

        HourlyStripElement(const HourArray& forecast, std::function<HourArray(void)> provider, utility::Signal& signal, const render::WeatherIconAtlas* weatherIcons, const Font* font, const float xPosition, const float yPosition, const float viewWidth, const float viewHeight, const float fontSize, const float fonstSpacing, const Color color);

        void OnEvent() override;
        void OnUpdate(const float scrollOffset) override;
        void OnRender() const override;

        Rectangle GetBounds() const override;
        void UpdateColor(const Color color) override;
};
}// namespace render
#endif
