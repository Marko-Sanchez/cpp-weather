#include "hourlystripelement.h"

#include <algorithm>
#include <cctype>
#include <raylib.h>

namespace render
{
namespace
{
Color GetTemperatureColor(int temp) noexcept
{
    if (temp >= 85) return Color {255, 69, 0, 255};  // Hot red-orange.
    if (temp >= 75) return Color {255, 140, 0, 255}; // Warm orange.
    if (temp >= 65) return Color {255, 215, 0, 255}; // Mild yellow.
    if (temp >= 55) return Color {99, 153, 34, 255}; // green.

    return BLUE;                                     // Cold blue.
}

bool CheckBounds(const Vector2& mouse, const Rectangle& rect) noexcept
{
    return rect.x <= mouse.x &&
           mouse.x <= (rect.x + rect.width) &&
           rect.y <= mouse.y &&
           mouse.y <= (rect.y + rect.height);
}
}// anonymous namespace

HourlyStripElement::HourlyStripElement(const HourArray& forecast, std::function<HourArray(void)> provider, utility::Signal& signal, const render::WeatherIconAtlas* weatherIcons, const Font* font, const float xPosition, const float yPosition, const float viewWidth, const float viewHeight, const float fontSize, const float fonstSpacing, const Color color)
    :_forecast(forecast),
    _provider(provider),
    _token(signal.Connect([this](){RefreshText();})),
    _weatherIcons(weatherIcons),
    _font(font),
    _xPosition(xPosition),
    _yPosition(yPosition),
    _viewWidth(viewWidth),
    _viewHeight(viewHeight),
    _fontSize(fontSize),
    _fontSpacing(fonstSpacing),
    _color(color)
{
    _yParentScrollOffset = 0.0f;
}

void HourlyStripElement::RefreshText()
{
    _forecast = _provider();
}

void HourlyStripElement::OnEvent()
{
    const Rectangle bounds     {this->GetBounds()};
    const Vector2 currMousePos {GetMousePosition()};

    const auto totalWidth {_weatherIcons->GetIconSize() * _forecast.size()};
    const auto maxScroll  {std::max(totalWidth - _viewWidth, 0.0f)};

    if (CheckBounds(currMousePos, bounds))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            _isDragging = true;
            _lastMousePos = currMousePos;
        }
    }

    if (_isDragging)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            float deltaX{currMousePos.x - _lastMousePos.x};
            _horizontalScrollOffset -= deltaX;
            _lastMousePos = currMousePos;
        }
        else
        {
            _isDragging = false;
        }
    }

    _horizontalScrollOffset = std::clamp(_horizontalScrollOffset, 0.0f, maxScroll);
}

void HourlyStripElement::OnUpdate(const float scrollOffset)
{
    _yParentScrollOffset = scrollOffset;
}

void HourlyStripElement::OnRender() const
{
    // TODO: some type of check to see if _forecast data is available ?
    const auto yScrolled {_yPosition - _yParentScrollOffset};

    const auto iconSize  {_weatherIcons->GetIconSize()};
    const auto iconScale {1.0f};

    const auto topBound {yScrolled};
    const auto botBound {yScrolled + _viewHeight};
    const auto workingArea {botBound - topBound};

    // divide into three sections.
    const auto sections   {workingArea * 0.33f};
    const auto topSection {topBound + (sections * 0.5f)};
    const auto midSection {topBound + (sections * 1.5f)};
    const auto botSection {topBound + (sections * 2.5f)};

    // Y coordinates.
    const auto yTemp      {topSection - (_fontSize * 0.5f)};
    const auto yCondition {midSection - (iconSize * 0.5f)};
    const auto yHour      {botSection - (_fontSize * 0.5f)};

    BeginScissorMode(_xPosition, yScrolled, _viewWidth, _viewHeight);

        for (size_t i {0}; i < _forecast.size(); ++i)
        {
            const auto& hour {_forecast[i]};
            const auto xHour {_xPosition - _horizontalScrollOffset + (i * iconSize) + (iconSize * 0.5f)};

            const Vector2 tempSize {MeasureTextEx(*_font, hour.temperature.c_str(), _fontSize, _fontSpacing)};
            const Vector2 tempPos  {xHour - (tempSize.x * 0.5f) , yTemp};

            int tempVal {0};
            if (!hour.temperature.empty() && isdigit(hour.temperature.back()))
            {
                tempVal = stoi(hour.temperature);
            }
            DrawTextEx(*_font, hour.temperature.c_str(), tempPos, _fontSize, _fontSpacing, GetTemperatureColor(tempVal));

            const auto iconWidth {iconSize * iconScale};
            Vector2 iconPos {xHour - (iconWidth * 0.5f), yCondition};
            _weatherIcons->DrawWeatherIcon(hour.condition, iconPos, iconScale);


            Vector2 hourSize{MeasureTextEx(*_font, hour.hour.c_str(), _fontSize, _fontSpacing)};
            Vector2 hourPos{xHour - (hourSize.x / 2.0f), yHour};
            DrawTextEx(*_font, hour.hour.c_str(), hourPos, _fontSize, _fontSpacing, _color);
        }

    EndScissorMode();
}

Rectangle HourlyStripElement::GetBounds() const
{
    return Rectangle {_xPosition, _yPosition - _yParentScrollOffset, _viewWidth, _viewHeight};
}

void HourlyStripElement::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
