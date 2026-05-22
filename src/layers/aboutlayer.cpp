#include "aboutlayer.h"

#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include <string_view>

#include "render/colordecorator.h"
#include "render/cullingdecorator.h"
#include "render/imageelement.h"
#include "render/paneldecorator.h"
#include "render/textelement.h"
#include "titlelayer.h"

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

constexpr std::string_view k_fontPath {"resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf"};
constexpr std::string_view k_audioPath{"resources/audio/redaudio.mp3"};
constexpr std::string_view k_imagePath{"resources/images/redimage.png"};

constexpr std::string_view k_title    {"About"};
constexpr std::string_view k_subTitle {"Cpp-Weather"};
constexpr std::string_view k_footer   {"[1] Back | Scroll Wheel / Arrow keys to navigate"};

constexpr std::string_view k_quotationsA =
    {
    "Revolution is not a crime; rebellion is justified!"
    };
constexpr std::string_view k_quotationsB =
    {
    "All reactionaries are paper tigers. In appearance "
    "the reactionaries are terrifying, but in reality, "
    "they are not so powerfull."
    };
constexpr std::string_view k_quotationsC =
    {
    "Yet struggle is inherent in identity "
    "and without struggle there can be no identity"
    };

float MaxScroll(float contentHeight, float screenHeight) noexcept
{
    return std::max(0.0f, contentHeight - screenHeight);
}
}// anonymous namespace

AboutLayer::AboutLayer()
    :m_screenWidth(GetScreenWidth()),
    m_screenHeight(GetScreenHeight()),
    m_layerScrollOffset(0.0f),
    m_targetScroll(0.0f),
    m_contentHeight(0.0f),
    m_audioPaused(false)
{
    InitAudioDevice();

    m_contentHeight = static_cast<float>(m_screenHeight * 2.0f);

    m_font     = LoadFont(k_fontPath.data());
    m_redAudio = LoadMusicStream(k_audioPath.data());
    m_redImage = LoadTexture(k_imagePath.data());

    PlayMusicStream(m_redAudio);
    SetMusicVolume(m_redAudio, 0.6f);

    m_composition.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::TextElement>
         (
          &m_font, k_title, (m_screenWidth - MeasureTextEx(m_font, k_title.data(), k_fontSizeTitle, k_fontSpacing).x) * 0.5f + 2.0f,
          60.0f + 2.0f, m_screenWidth, k_fontSizeTitle, k_fontSpacing, Color{0, 0, 0, 100}
         ),m_screenWidth, m_screenHeight
     )
    );

    m_composition.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::TextElement>
         (
          &m_font, k_title, (m_screenWidth - MeasureTextEx(m_font, k_title.data(), k_fontSizeTitle, k_fontSpacing).x) * 0.5f,
          60.0f, m_screenWidth, k_fontSizeTitle, k_fontSpacing, WHITE
         ),m_screenWidth, m_screenHeight
     )
    );

    m_composition.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::ColorDecorator>
         (
          std::make_unique<render::TextElement>
          (
           &m_font, k_subTitle,
           (m_screenWidth - MeasureTextEx(m_font, k_subTitle.data(), k_fontSizeSubTitle, k_fontSpacing).x) * 0.50f,
           128.0f, m_screenWidth * 0.50f, k_fontSizeSubTitle, k_fontSpacing, WHITE
          )
         ),
        m_screenWidth, m_screenHeight
     )
    );

    m_composition.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::ImageElement>
         (
          &m_redImage, m_screenWidth * 0.37f, m_screenHeight * 0.35f, 0.0f, 0.3f, WHITE, false
         ),
         m_screenWidth, m_screenHeight
     )
    );

    m_composition.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::PanelDecorator>
         (
          std::make_unique<render::TextElement>
          (
           &m_font, k_quotationsA, m_screenWidth * 0.125f, m_screenHeight * 0.25f, 276.0f, k_fontSizeBody, k_fontSpacing, RED
          ),
          Fade(GREEN, 0.40f)
         ),
         m_screenWidth, m_screenHeight
     )
    );

    m_composition.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
         std::make_unique<render::PanelDecorator>
         (
          std::make_unique<render::TextElement>
          (
          &m_font, k_quotationsB, m_screenWidth * 0.25f, m_screenHeight * 0.55f, m_screenWidth * 0.50f, k_fontSizeBody, k_fontSpacing, GREEN
          ),
          Fade(BLUE, 0.5f)
         ),
         m_screenWidth, m_screenHeight
     )
    );

    m_composition.emplace_back
    (
     std::make_unique<render::CullingDecorator>
     (
        std::make_unique<render::PanelDecorator>
        (
         std::make_unique<render::ColorDecorator>
         (
             std::make_unique<render::TextElement>
             (
              &m_font, k_quotationsC, m_screenWidth * 0.15f, m_screenHeight * 0.75f, m_screenWidth * 0.75f, k_fontSizeBody, k_fontSpacing, RED
             )
         ),
         YELLOW
        ),
        m_screenWidth, m_screenHeight
     )
    );

    m_composition.emplace_back
    (
     std::make_unique<render::TextElement>
     (
      &m_font, k_footer, 10.0f, m_screenHeight - 28.0f, m_screenWidth, 10.0f, k_fontSpacing, Color{255, 255, 255, 150}, false
     )
    );
}

AboutLayer::~AboutLayer()
{
    m_composition.clear();

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
        m_audioPaused = !m_audioPaused;
        if (m_audioPaused)
        {
            PauseMusicStream(m_redAudio);
        }
        else
        {
            ResumeMusicStream(m_redAudio);
        }
    }

    for (auto& comp: m_composition)
    {
        comp->OnEvent();
    }
}

void AboutLayer::OnUpdate(float deltatime)
{
    UpdateMusicStream(m_redAudio);

    // lerp allows smooth transition between two points: offset and target.
    m_layerScrollOffset = lerp(m_layerScrollOffset, m_targetScroll, k_scrollSmooth * deltatime);
    for (auto& comp: m_composition)
    {
        comp->OnUpdate(m_layerScrollOffset);
    }
}

void AboutLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(BLACK);

        this->DrawBackground();
        for (auto& comp: m_composition)
        {
            comp->OnRender();
        }
    EndDrawing();
}

void AboutLayer::DrawBackground() const
{
    DrawRectangle(0, 0, m_screenWidth, m_screenHeight, BROWN);
    DrawRectangleLinesEx(Rectangle{0.0f, 0.0f, static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight)}, 6, Fade(BLACK, 0.3f));
}
}// namespace Layers
