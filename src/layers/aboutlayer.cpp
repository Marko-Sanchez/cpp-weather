#include "aboutlayer.h"

#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include <string_view>

#include "titlelayer.h"
#include "render/textdecorator.h"

namespace Layers
{
namespace
{
// how many pixels we want to scroll per frame tick.
constexpr float k_scrollSpeed {60.0f};
// multiplied by frametime, allows us to move between points smoothly. Higher number = instant, lower number = sluggish.
constexpr float k_scrollSmooth {12.0f};
// GetMouseWheel() returns a small number hence we multiply by WHEEL_MULTIPLIER to scroll through more pixels.
// since if we where not using a multiplier we would only move between [-1.0, 1.0] pixels instead of 60.
constexpr float k_wheelMultiplier {60.0f};

constexpr int k_fontSizeTitle    {48};
constexpr int k_fontSizeSubTitle {32};
constexpr int k_fontSizeBody     {16};
constexpr int k_fontSpacing      {2};

constexpr std::string_view k_title    {"About"};
constexpr std::string_view k_subTitle {"Cpp-Weather"};
constexpr std::string_view k_footer   {"[1] Back | Scroll Wheel / Arrow keys to navigate"};

float MaxScroll(float contentHeight, float screenHeight) noexcept
{
    return std::max(0.0f, contentHeight - screenHeight);
}
}// anonymous namespace

AboutLayer::AboutLayer():
m_screenWidth(512),
m_screenHeight(1024),
m_scrollOffset(0.0f),
m_targetScroll(0.0f),
m_contentHeight(0.0f),
m_framecounter(0.0f),
m_isPaused(false),
m_colorRandom(RED)
{
    InitAudioDevice();

    m_contentHeight = static_cast<float>(m_screenHeight * 2.0f);

    m_font     = LoadFont("resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf");
    m_redAudio = LoadMusicStream("resources/audio/redaudio.mp3");
    m_redImage = LoadTexture("resources/images/redimage.png");

    PlayMusicStream(m_redAudio);
    SetMusicVolume(m_redAudio, 0.8f);
}

AboutLayer::~AboutLayer()
{
    if (m_font.texture.id > 0 && m_font.texture.id != GetFontDefault().texture.id)
    {
        UnloadFont(m_font);
    }
    if (m_redImage.id > 0)
    {
        UnloadTexture(m_redImage);
    }

    UnloadMusicStream(m_redAudio);
    CloseAudioDevice();
}

void AboutLayer::OnEvent()
{
    if (IsKeyPressed(KEY_ONE))
    {
        TransitionTo<Layers::TitleLayer>();
    }

    if (auto wheel{GetMouseWheelMove()}; wheel != 0.0f)
    {
        m_targetScroll -= wheel * k_wheelMultiplier;
    }

    // How long the last frame took multiplied by our desired pixel speed.
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        m_targetScroll += k_scrollSpeed * GetFrameTime();
    }

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        m_targetScroll -= k_scrollSpeed * GetFrameTime();
    }
    m_targetScroll = Clamp(m_targetScroll, 0.0f, MaxScroll(m_contentHeight, m_screenHeight));

    if (IsKeyPressed(KEY_P))
    {
        m_isPaused = !m_isPaused;
        if (m_isPaused)
        {
            PauseMusicStream(m_redAudio);
        }
        else
        {
            ResumeMusicStream(m_redAudio);
        }
    }
}

void AboutLayer::OnUpdate(float deltatime)
{
    UpdateMusicStream(m_redAudio);

    // lerp allows smooth transition between two points: offset and target.
    m_scrollOffset = lerp(m_scrollOffset, m_targetScroll, k_scrollSmooth * deltatime);

    if (++m_framecounter % 45 == 0)
    {
        m_colorRandom.r = static_cast<char>(GetRandomValue(0, 255));
        m_colorRandom.g = static_cast<char>(GetRandomValue(0, 255));
        m_colorRandom.b = static_cast<char>(GetRandomValue(0, 255));
        m_colorRandom.a = 200;

        m_framecounter = 0;
    }
}

void AboutLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(BLACK);

        this->DrawBackground();
        this->DrawTitle();
        this->DrawSubTitle();
        this->DrawBody();
        this->DrawOtherBody();

        DrawTextureEx(m_redImage, Vector2{m_screenWidth * 0.37f, m_screenHeight * 0.35f - m_scrollOffset}, 0.0f, 0.3f, WHITE);

        DrawText(k_footer.data(), 10, m_screenHeight - 18, 10, Color{255, 255, 255, 100});
    EndDrawing();
}

void AboutLayer::DrawBackground() const
{
    DrawRectangle(0, 0, m_screenWidth, m_screenHeight, BROWN);
    DrawRectangleLinesEx(Rectangle{0.0f, 0.0f, static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight)}, 6, Fade(BLACK, 0.3f));
}

void AboutLayer::DrawTitle() const
{
    const int textWidth   {MeasureText(k_title.data(), k_fontSizeTitle)};
    const int yStarterPos {60};

    const int xPosition       {(m_screenWidth - textWidth) / 2};
    const int yPosition       {static_cast<int>(yStarterPos - m_scrollOffset)};
    const int xShadowPosition {xPosition + 2};
    const int yShadowPosition {yPosition + 2};

    // Only draw if we are above the screenheight (bottom of screen) or below the -fontsize (past the top to include text).
    if (yPosition > -k_fontSizeTitle && yPosition < m_screenHeight)
    {
        DrawText(k_title.data(), xShadowPosition, yShadowPosition, k_fontSizeTitle, Color{0, 0, 0, 100});
        DrawText(k_title.data(), xPosition, yPosition, k_fontSizeTitle, WHITE);
    }
}

void AboutLayer::DrawSubTitle() const
{
    const Vector2 textsize  {MeasureTextEx(m_font, k_subTitle.data(), k_fontSizeSubTitle, k_fontSpacing)};
    const float yStarterPos {128.0f};

    const float xPosition {(m_screenWidth - textsize.x) / 2};
    const float yPosition {yStarterPos - m_scrollOffset};

    // Only draw if we are above the screenheight (bottom of screen) or below the -fontsize (past the top to include text).
    if (yPosition > -k_fontSizeSubTitle && yPosition < m_screenHeight)
    {
        DrawTextEx(m_font, k_subTitle.data(), Vector2{xPosition, yPosition}, k_fontSizeSubTitle, k_fontSpacing, m_colorRandom);

        // Add padding between text and rectangle line.
        const float linepadding  {8.0f};
        const float linepadding2 {16.0f};
        const Rectangle rectline {xPosition - linepadding, yPosition - linepadding, textsize.x + linepadding2, textsize.y + linepadding2};
        DrawRectangleLinesEx(rectline, 4, MAROON);
    }
}

void AboutLayer::DrawBody() const
{
    const char* text =
    "Revolution is not a crime; rebellion is justified!";

    const float xPosition {m_screenWidth / 8.0f};
    const float yPosition {(m_screenHeight / 4.0f) - m_scrollOffset};

    const float rectWidth  {276.0f};
    const float rectHeight {render::MeasureWrappedTextHeight(m_font, text, rectWidth, k_fontSizeBody, k_fontSpacing)};

    if (yPosition > -1.0f * rectHeight && yPosition < (m_screenHeight + rectHeight))
    {
        const Rectangle panel {xPosition, yPosition, rectWidth, rectHeight};

        DrawRectangleRec(panel, Fade(GREEN, 0.4f));
        render::DrawWrappedText(m_font, panel, text, k_fontSizeBody, k_fontSpacing, RED);
    }
}

void AboutLayer::DrawOtherBody() const
{
    const char* text =
    "All reactionaries are paper tigers. In appearance "
    "the reactionaries are terrifying, but in reality, "
    "they are not so powerfull.";

    const float xPosition {m_screenWidth * 0.25f};
    const float yPosition {(m_screenHeight * 0.55f) - m_scrollOffset};

    const float rectWidth  {m_screenWidth * 0.5f};
    const float rectHeight {render::MeasureWrappedTextHeight(m_font, text, rectWidth, k_fontSizeBody, k_fontSpacing)};

    if (yPosition > -1.0f * rectHeight && yPosition < (m_screenHeight + rectHeight))
    {
        const Rectangle panel {xPosition, yPosition, rectWidth, rectHeight};

        DrawRectangleRec(panel, Fade(BLUE, 0.5f));
        render::DrawWrappedText(m_font, panel, text, k_fontSizeBody, k_fontSpacing, GREEN);
    }
}
}// namespace Layers
