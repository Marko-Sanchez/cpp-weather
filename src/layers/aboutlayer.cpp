#include "aboutlayer.h"

#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

#include "titlelayer.h"

namespace Layers
{
namespace
{
// how many pixels we want to scroll per frame tick.
const float SCROLL_SPEED{60.0f};
// multiplied by frametime, allows us to move between points smoothly. Higher number = instant, lower number = sluggish.
const float SCROLL_SMOOTH{12.0f};
// GetMouseWheel() returns a small number hence we multiply by WHEEL_MULTIPLIER to scroll through more pixels.
// since if we where not using a multiplier we would only move between [-1.0, 1.0] pixels instead of 60.
const float WHEEL_MULTIPLIER{60.0f};

float MaxScroll(float contentHeight, float screenHeight)
{
    return std::max(0.0f, contentHeight - screenHeight);
}
}// anonymous namespace

AboutLayer::AboutLayer():
m_screenWidth(512),
m_screenHeight(1024),
m_colorRandom(RED)
{
    m_contentHeight = static_cast<float>(m_screenHeight + 256.0f);
    m_font = LoadFont("resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf");
}

AboutLayer::~AboutLayer()
{
    UnloadFont(m_font);
}

void AboutLayer::OnEvent()
{
    if (IsKeyPressed(KEY_ONE))
    {
        TransitionTo<Layers::TitleLayer>();
    }

    // mouse wheel scroll.
    float wheel{GetMouseWheelMove()};
    if (wheel != 0.0f)
    {
        m_targetScroll -= wheel * WHEEL_MULTIPLIER;
    }

    // How long the last frame took multiplied by our desired pixel speed.
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        m_targetScroll += SCROLL_SPEED * GetFrameTime();
    }

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        m_targetScroll -= SCROLL_SPEED * GetFrameTime();
    }

    m_targetScroll = Clamp(m_targetScroll, 0.0f, MaxScroll(m_contentHeight, m_screenHeight));
}

void AboutLayer::OnUpdate(float deltatime)
{
    // lerp allows smooth transition between two points: offset and target.
    m_scrollOffset = lerp(m_scrollOffset, m_targetScroll, SCROLL_SMOOTH * deltatime);

    if (++m_framecounter % 45 == 0)
    {
        m_colorRandom.r = static_cast<char>(GetRandomValue(0, 255));
        m_colorRandom.g = static_cast<char>(GetRandomValue(0, 255));
        m_colorRandom.b = static_cast<char>(GetRandomValue(0, 255));
        m_colorRandom.a = 200;
    }
}

void AboutLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(BLACK);
        this->DrawBackground();
        this->DrawTitle();
        this->DrawSubTitle();

        DrawText("[1] Back | Scroll Wheel / Arrow keys to navigate", 10, m_screenHeight - 18, 10, Color{255, 255, 255, 100});
    EndDrawing();
}

void AboutLayer::DrawBackground() const
{
    DrawRectangle(0, 0, m_screenWidth, m_screenHeight, BROWN);
    DrawRectangleLinesEx(Rectangle{0.0f, 0.0f, static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight)}, 6, Fade(BLACK, 0.3f));
}

void AboutLayer::DrawTitle() const
{
    const int fontSize{48};
    const int yStarterPos{60};

    const char* text = "About";
    const int textWidth{MeasureText(text, fontSize)};

    const int x{(m_screenWidth - textWidth) / 2};
    const int y{static_cast<int>(yStarterPos - m_scrollOffset)};

    // Only draw if we are above the screenheight (bottom of screen) or below the -fontsize (past the top to include text).
    if (y > -fontSize && y < m_screenHeight)
    {
        // text with shadow
        DrawText(text, x + 2, y + 2, fontSize, Color{0, 0, 0, 100});
        DrawText(text, x, y, fontSize, WHITE);
    }
}

void AboutLayer::DrawSubTitle() const
{
    const int fontSize{32};
    const int yStarterPos{128};

    const char* body = "Cpp-Weather";
    const Vector2 textsize{MeasureTextEx(m_font, body, fontSize, 2)};

    const float x{(m_screenWidth - textsize.x) / 2};
    const float y{yStarterPos - m_scrollOffset};

    // Only draw if we are above the screenheight (bottom of screen) or below the -fontsize (past the top to include text).
    if (y > -fontSize && y < m_screenHeight)
    {
        DrawTextEx(m_font, body, Vector2{x, y}, fontSize, 2, m_colorRandom);

        // Add padding between text and rectangle line.
        float linepadding{8};
        float linepadding2{16};
        Rectangle rectline{x - linepadding, y - linepadding, textsize.x + linepadding2, textsize.y + linepadding2};
        DrawRectangleLinesEx(rectline, 4, MAROON);
    }
}
}// namespace Layers
