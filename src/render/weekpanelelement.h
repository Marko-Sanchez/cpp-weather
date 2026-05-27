#ifndef WEEKPANELELEMENT_H
#define WEEKPANELELEMENT_H

#include "drawablecomponent.h"

#include <functional>
#include <vector>

#include <raylib.h>
#include "utility/signal.h"
#include "utility/weatherdata.h"

namespace render
{
class WeekPanelElement: public DrawableComponent
{
    private:

        using WeekForecast = std::vector<utility::DailyForecast>;

        WeekForecast                      _forecast;
        std::function<WeekForecast(void)> _provider;
        utility::Signal::Token            _token;

        const Font* _font;
        const float _xPosition;
        const float _yPosition;
        const float _panelWidth;
        const float _panelHeight;

        Color _color;

        float _yParentScrollOffset;

        float _highLabelWidth;
        float _lowLabelWidth;

        void RefreshData();
        void DrawPanel(const Rectangle& card, const utility::DailyForecast& day) const;

    public:

        WeekPanelElement(WeekForecast weekforecast, std::function<WeekForecast(void)> provider, utility::Signal& signal,
                         const Font* font, const float xPosition, const float yPosition, const float width, const float height, const Color color);

        void OnUpdate(const float scrollOffset) override;
        void OnRender() const override;

        Rectangle GetBounds() const override;
        void UpdateColor(const Color color) override;

};
}// namespace render
#endif
