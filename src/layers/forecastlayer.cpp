#include "forecastlayer.h"

#include <raylib.h>

#include "layers/aboutlayer.h"

namespace Layers
{
ForecastLayer::ForecastLayer():
m_screenWidth(512),
m_screenHeight(1024)
{
    m_font = LoadFont("resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf");
}

ForecastLayer::~ForecastLayer()
{
    if (m_font.texture.id > 0 && m_font.texture.id != GetFontDefault().texture.id)
    {
        UnloadFont(m_font);
    }
}

void ForecastLayer::OnEvent()
{
    if (IsKeyPressed(KEY_ONE))
    {
        TransitionTo<Layers::AboutLayer>();
    }
}

void ForecastLayer::OnUpdate(float deltatime)
{
    // weather params ?
}

void ForecastLayer::OnRender()
{
    BeginDrawing();
        this->DrawBackground();
        this->DrawTitle();
    EndDrawing();
}

inline void ForecastLayer::DrawBackground() const
{
    DrawRectangleGradientV(0, 0, m_screenWidth, m_screenHeight, WHITE, BLUE);
}

inline void ForecastLayer::DrawTitle() const
{
    const char* title = "Redmond";
    const int fontsize{42};
    const int spacing{2};

    const Vector2 titlesize{MeasureTextEx(m_font, title, fontsize, spacing)};
    const Vector2 titlepos{(m_screenWidth - titlesize.x) / 2.0f, m_screenHeight / 8.0f};

    DrawTextEx(m_font, title, titlepos, fontsize, spacing, BLACK);

    const char* currenttemp = "53°";
    const char* highlow = "H: 56° L: 43°";
    const int fontct{32};
    const int fonthl{16};

    const Vector2 tempsize{MeasureTextEx(m_font, currenttemp, fontct, spacing)};
    const Vector2 hlsize{MeasureTextEx(m_font, highlow, fonthl, spacing)};

    const Vector2 temppos{(m_screenWidth - tempsize.x) / 2.0f, titlepos.y + titlesize.y};
    const Vector2 hlpos{(m_screenWidth - hlsize.x) / 2.0f, temppos.y + tempsize.y};

    DrawTextEx(m_font, currenttemp, temppos, fontct, spacing, WHITE);
    DrawTextEx(m_font, highlow, hlpos, fonthl, spacing, WHITE);
}
}// namespace Layers
