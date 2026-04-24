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
        m_atlas(LoadTexture("resources/images/weatheratlas64x.png")),
        m_iconSize(64.0f),
        m_columns(5)
    {
        m_iconPositions[utility::WeatherCondition::Clear]        = {0, 0};
        m_iconPositions[utility::WeatherCondition::Foggy]        = {0, 1};
        m_iconPositions[utility::WeatherCondition::PartlyCloudy] = {0, 2};
        m_iconPositions[utility::WeatherCondition::Cloudy]       = {0, 3};
        m_iconPositions[utility::WeatherCondition::Drizzle]      = {0, 4};
        m_iconPositions[utility::WeatherCondition::Rainy]        = {1, 0};
        m_iconPositions[utility::WeatherCondition::RainShowers]  = {1, 1};
        m_iconPositions[utility::WeatherCondition::Snowy]        = {1, 2};
        m_iconPositions[utility::WeatherCondition::ThunderStorm] = {1, 3};
        m_iconPositions[utility::WeatherCondition::Unknown]      = {1, 4};
    }

    ~WeatherIconAtlas()
    {
        UnloadTexture(m_atlas);
    }

    float GetIconSize() const noexcept
    {
        return m_iconSize;
    }

    void DrawWeatherIcon(const utility::WeatherCondition condition, const Vector2 pos, const float scale, const Color tint = WHITE) const
    {
        if (!m_iconPositions.contains(condition))
        {
            return;
        }

        const auto& [row, col] = m_iconPositions.at(condition);

        const Vector2 origin{0.0f, 0.0f};
        float rotation{0.0f};

        const Rectangle source
        {
            col * m_iconSize,
            row * m_iconSize,
            m_iconSize,
            m_iconSize
        };

        const Rectangle dest
        {
            pos.x,
            pos.y,
            m_iconSize * scale,
            m_iconSize * scale
        };

        DrawTexturePro(m_atlas, source, dest, origin, rotation, tint);
    }
};
}// namespace render
#endif
