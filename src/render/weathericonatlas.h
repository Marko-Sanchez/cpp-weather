#ifndef WEATHERICONATLAS_H
#define WEATHERICONATLAS_H

#include <unordered_map>

#include <raylib.h>

#include "utility/weatherdata.h"

namespace render
{
class WeatherIconAtlas
{
private:

    int       m_columns;
    float     m_iconSize;
    Texture2D m_atlas;

    std::unordered_map<utility::WeatherCondition, std::pair<int, int>> m_iconPositions;

public:

    WeatherIconAtlas():
        m_atlas(LoadTexture("resources/images/weather-icon-atlas.png")),
        m_iconSize(32.0f),
        m_columns(4)
    {
        m_iconPositions[utility::WeatherCondition::Clear]        = {2, 3};
        m_iconPositions[utility::WeatherCondition::PartlyCloudy] = {1, 3};
        m_iconPositions[utility::WeatherCondition::Cloudy]       = {0, 6};
        m_iconPositions[utility::WeatherCondition::Rainy]        = {1, 0};
        m_iconPositions[utility::WeatherCondition::RainShowers]  = {1, 1};
        m_iconPositions[utility::WeatherCondition::Snowy]        = {2, 2};
        m_iconPositions[utility::WeatherCondition::Drizzle]      = {0, 0};
        m_iconPositions[utility::WeatherCondition::Foggy]        = {0, 1};
        m_iconPositions[utility::WeatherCondition::Unknown]      = {3, 3};
    }

    ~WeatherIconAtlas()
    {
        UnloadTexture(m_atlas);
    }

    void DrawWeatherIcon(utility::WeatherCondition condition, Vector2 pos, float scale, Color tint = WHITE)
    {
        if (!m_iconPositions.contains(condition))
        {
            return;
        }

        const auto& [row, col] = m_iconPositions[condition];

        Rectangle source
        {
            col * m_iconSize,
            row * m_iconSize,
            m_iconSize,
            m_iconSize
        };

        Rectangle dest
        {
            pos.x,
            pos.y,
            m_iconSize * scale,
            m_iconSize * scale
        };

        DrawTexturePro(m_atlas, source, dest, Vector2{0.0f, 0.0f}, 0.0f, tint);
    }
};
}// namespace render
#endif
