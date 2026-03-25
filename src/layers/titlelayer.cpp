#include "titlelayer.h"

#include <raylib.h>

#include "aboutlayer.h"
#include "forecastlayer.h"

namespace Layers
{
TitleLayer::TitleLayer():
m_screenWidth(512),
m_screenHeight(1024),
m_framecounter(0)
{
    m_font = LoadFont("resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf");
    m_backgroundTexture = LoadTexture("resources/images/title.png");
}

TitleLayer::~TitleLayer()
{
    if (m_backgroundTexture.id > 0)
    {
        UnloadTexture(m_backgroundTexture);
    }

    if (m_font.texture.id > 0 && m_font.texture.id != GetFontDefault().texture.id)
    {
        UnloadFont(m_font);
    }
}

void TitleLayer::OnEvent()
{
    if (IsKeyPressed(KEY_ONE))
    {
        TransitionTo<Layers::ForecastLayer>();
    }
    else if (IsKeyPressed(KEY_TWO))
    {
        TransitionTo<Layers::AboutLayer>();
    }
}

void TitleLayer::OnUpdate(float deltatime)
{
    ++m_framecounter;
    if (m_framecounter == 270/* six seconds: fps * 6*/)
    {
        TransitionTo<Layers::ForecastLayer>();
    }
}

void TitleLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(m_backgroundTexture, -128, 0, WHITE);
        this->DrawTitle();
        this->DrawImageCredits();
    EndDrawing();
}

inline void TitleLayer::DrawTitle() const
{
    const char* title = "Cpp-Weather";

    const int fontsize{42};
    const Vector2 textsize{MeasureTextEx(m_font, title, fontsize, 2)};

    DrawTextEx(m_font, title, Vector2{((m_screenWidth - textsize.x )/ 2) + 2, 60 + 2}, fontsize, 2, BLACK);
    DrawTextEx(m_font, title, Vector2{(m_screenWidth - textsize.x )/ 2, 60}, fontsize, 2, WHITE);
}

/*
 * Image background credits @pumbey on discord.
 */
inline void TitleLayer::DrawImageCredits() const
{
    const char* credits = "image credits: @pumbey";

    const int fontsize{13};
    const Vector2 textsize{MeasureTextEx(m_font, credits, fontsize, 2)};

    DrawTextEx(m_font, credits, Vector2{m_screenWidth - textsize.x, m_screenHeight - textsize.y}, fontsize, 2, GRAY);
}
}
