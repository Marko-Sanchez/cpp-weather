#include "titlelayer.h"

#include <raylib.h>
#include <string_view>

#include "aboutlayer.h"
#include "forecastlayer.h"

namespace Layers
{
namespace
{
    constexpr int k_fontSizeTitle{42};
    constexpr int k_fontSizeCredit{13};
    constexpr int k_fontSpacing{2};

    constexpr std::string_view k_title{"Cpp-Weather"};
    constexpr std::string_view k_credit{"image credits: @pumbey"};
}// anonymous namespace

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
    const int yForeground{60};
    const int yBackground{62};
    const Vector2 textsize{MeasureTextEx(m_font, k_title.data(), k_fontSizeTitle, k_fontSpacing)};

    const float centerX{(m_screenWidth - textsize.x) / 2.0f};

    DrawTextEx(m_font, k_title.data(), Vector2{centerX + 2, yBackground}, k_fontSizeTitle, k_fontSpacing, BLACK);
    DrawTextEx(m_font, k_title.data(), Vector2{centerX, yForeground}, k_fontSizeTitle, k_fontSpacing, WHITE);
}

/*
 * Image background credits @pumbey on discord.
 */
inline void TitleLayer::DrawImageCredits() const
{
    const Vector2 textsize{MeasureTextEx(m_font, k_credit.data(), k_fontSizeCredit, k_fontSpacing)};
    DrawTextEx(m_font, k_credit.data(), Vector2{m_screenWidth - textsize.x, m_screenHeight - textsize.y}, k_fontSizeCredit, k_fontSpacing, GRAY);
}
}// namespace Layers
