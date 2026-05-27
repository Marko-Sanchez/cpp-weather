#include "weekpanelelement.h"

#include <raylib.h>

namespace render
{
namespace
{
    constexpr int k_FontSizeTemp    {32};
    constexpr int k_FontSizeLabel   {12};
    constexpr int k_FontSpacing     {2};

    constexpr float k_ColumnWidth    {64.0f};
    constexpr float k_XEdgePadding   {12.0f};
    constexpr float k_PanelSpacing   {12.0f};
    constexpr float k_PanelRoundness {0.2f};
    constexpr int k_PanelSegments    {0};

    Color GetTemperatureColor(int temp)
    {
        if (temp >= 85) return Color {255, 69, 0, 255};  // Hot red-orange.
        if (temp >= 75) return Color {255, 140, 0, 255}; // Warm orange.
        if (temp >= 65) return Color {255, 215, 0, 255}; // Mild yellow.
        if (temp >= 55) return Color {99, 153, 34, 255}; // Green.

        return BLUE;                                     // Cold blue.
    }

    int ParseTemperature(const std::string& temp)
    {
        if (temp.empty())
        {
            return 0;
        }
        else if (std::isdigit(temp.front()))
        {
            return std::stoi(temp);
        }

        return 0;
    }

    float CenterInColumn(float columnX, float textWidth)
    {
        return columnX + (k_ColumnWidth - textWidth) * 0.5f;
    }
}// anonymous namespace

WeekPanelElement::WeekPanelElement(WeekForecast weekforecast, std::function<WeekForecast(void)> provider, utility::Signal& signal, const Font* font, const float xPosition, const float yPosition, const float width, const float height, const Color color)
    :_forecast(std::move(weekforecast)),
    _provider(provider),
    _token(signal.Connect([this](void){RefreshData();})),
    _font(font),
    _xPosition(xPosition),
    _yPosition(yPosition),
    _panelWidth(width),
    _panelHeight(height),
    _color(color)
{
    _highLabelWidth = MeasureTextEx(*_font, "HIGH", k_FontSizeLabel, k_FontSpacing).x;
    _lowLabelWidth = MeasureTextEx(*_font, "LOW", k_FontSizeLabel, k_FontSpacing).x;
}

void WeekPanelElement::RefreshData()
{
    _forecast = std::move(_provider());
}

void WeekPanelElement::DrawPanel(const Rectangle& card, const utility::DailyForecast& day) const
{
    DrawRectangleRounded(card, k_PanelRoundness, k_PanelSegments, _color);

    const int hightemp {ParseTemperature(day.high)};
    const int lowtemp  {ParseTemperature(day.low)};

    // Day.
    auto dayTextSize {MeasureTextEx(*_font, day.day.c_str(), k_FontSizeTemp, k_FontSpacing)};
    auto centerY     {(card.height - dayTextSize.y) * 0.5f};
    Vector2 dayPos   {card.x + k_XEdgePadding, card.y + centerY};

    DrawTextEx(*_font, day.day.c_str(), dayPos, k_FontSizeTemp, k_FontSpacing, WHITE);

    const float cardRight  {card.x + card.width};
    const float cardBottom {card.y + card.height};
    const float labelY     {card.y + 8.0f};

    // High temperature.
    float paddingFromRightBound {(k_ColumnWidth * 2.0f) - k_XEdgePadding};

    const auto highTempSize {MeasureTextEx(*_font, day.high.c_str(), k_FontSizeTemp, k_FontSpacing)};
    const float highColumnX {cardRight - paddingFromRightBound};

    // used for both high / low, since font 'Y' length should be the same.
    const float tempYPadding {highTempSize.y + 8.0f};

    Vector2 highLabelPos {CenterInColumn(highColumnX, _highLabelWidth), labelY};
    Vector2 highTempPos  {CenterInColumn(highColumnX, highTempSize.x), cardBottom - tempYPadding};

    DrawTextEx(*_font, "HIGH", highLabelPos, k_FontSizeLabel, k_FontSpacing, WHITE);
    DrawTextEx(*_font, day.high.c_str(), highTempPos, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(hightemp));

    // Low temperature.
    paddingFromRightBound = k_ColumnWidth - k_XEdgePadding;

    const auto lowTempSize {MeasureTextEx(*_font, day.low.c_str(), k_FontSizeTemp, k_FontSpacing)};
    const float lowColumnX {cardRight - paddingFromRightBound};

    Vector2 lowLabelPos {CenterInColumn(lowColumnX, _lowLabelWidth), labelY};
    Vector2 lowTempPos  {CenterInColumn(lowColumnX, lowTempSize.x), cardBottom - tempYPadding};

    DrawTextEx(*_font, "LOW", lowLabelPos, k_FontSizeLabel, k_FontSpacing, WHITE);
    DrawTextEx(*_font, day.low.c_str(), lowTempPos, k_FontSizeTemp, k_FontSpacing, GetTemperatureColor(lowtemp));
}

void WeekPanelElement::OnUpdate(const float scrollOffset)
{
    _yParentScrollOffset = scrollOffset;
}

void WeekPanelElement::OnRender() const
{
    const auto yScrolled {_yPosition - _yParentScrollOffset};

    Rectangle panel
    {
        _xPosition,
        yScrolled,
        _panelWidth,
        _panelHeight
    };

    for (const auto& day: _forecast)
    {
        this->DrawPanel(panel, day);
        panel.y += panel.height + k_PanelSpacing;
    }
}

Rectangle WeekPanelElement::GetBounds() const
{
    if (_forecast.empty())
    {
        return Rectangle {_xPosition, _yPosition - _yParentScrollOffset, _panelWidth, 0.0f};
    }

    auto size {_forecast.size()};
    return Rectangle {_xPosition, _yPosition - _yParentScrollOffset, _panelWidth, _panelHeight * size + k_PanelSpacing * (size - 1)};
}

void WeekPanelElement::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
