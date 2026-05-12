#include "titlelayer.h"

#include <raylib.h>
#include <string_view>

#include "aboutlayer.h"
#include "forecastlayer.h"
#include "render/imageelement.h"
#include "render/textelement.h"

namespace Layers
{
namespace
{
    constexpr int k_fontSizeTitle  {42};
    constexpr int k_fontSizeCredit {13};
    constexpr int k_fontSpacing    {2};
    constexpr int k_sixSeconds     {270};// 6s @ 45fps

    constexpr float k_xPositionTexture {-128.0f};
    constexpr float k_yPositionTexture {0.0f};
    constexpr float k_yPositionTitle   {60.0f};
    constexpr float k_xOffsetCredits   {200.0f};
    constexpr float k_yOffsetCredits   {24.0f};

    constexpr std::string_view k_fontPath  {"resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf"};
    constexpr std::string_view k_imagePath {"resources/images/title.png"};

    constexpr std::string_view k_title     {"Cpp-Weather"};
    constexpr std::string_view k_credits   {"image credits: @pumbey"};
}// anonymous namespace

TitleLayer::TitleLayer()
    :m_screenWidth(GetScreenWidth()),
    m_screenHeight(GetScreenHeight()),
    m_framecounter(0)
{
    m_font              = LoadFont(k_fontPath.data());
    m_backgroundTexture = LoadTexture(k_imagePath.data());

    m_composition.emplace_back
    (
     std::make_unique<render::ImageElement>
     (
      &m_backgroundTexture, k_xPositionTexture, k_yPositionTexture, 0.0f, 1.0f, WHITE, false
     )
    );

    auto titleSize = MeasureTextEx(m_font, k_title.data(), k_fontSizeTitle, k_fontSpacing);
    m_composition.emplace_back
    (
     std::make_unique<render::TextElement>
     (
      &m_font, k_title, (m_screenWidth - titleSize.x) * 0.5f, k_yPositionTitle, m_screenWidth,  k_fontSizeTitle, k_fontSpacing, WHITE, false
     )
    );

    // Background image credits @pumbey on discord.
    m_composition.emplace_back
    (
     std::make_unique<render::TextElement>
     (
      &m_font, k_credits, m_screenWidth - k_xOffsetCredits, m_screenHeight - k_yOffsetCredits, m_screenWidth,  k_fontSizeCredit, k_fontSpacing, GRAY, false
     )
    );
}

TitleLayer::~TitleLayer()
{
    m_composition.clear();

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
    if (++m_framecounter == k_sixSeconds)
    {
        TransitionTo<Layers::ForecastLayer>();
    }
}

void TitleLayer::OnRender()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        for (auto& comp: m_composition)
        {
            comp->OnRender();
        }

    EndDrawing();
}
}// namespace Layers
