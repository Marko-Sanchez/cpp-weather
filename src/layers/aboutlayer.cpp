#include "aboutlayer.h"

#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include <sstream>
#include <string_view>

#include "titlelayer.h"

namespace Layers
{
namespace
{
// how many pixels we want to scroll per frame tick.
constexpr float k_scrollSpeed{60.0f};
// multiplied by frametime, allows us to move between points smoothly. Higher number = instant, lower number = sluggish.
constexpr float k_scrollSmooth{12.0f};
// GetMouseWheel() returns a small number hence we multiply by WHEEL_MULTIPLIER to scroll through more pixels.
// since if we where not using a multiplier we would only move between [-1.0, 1.0] pixels instead of 60.
constexpr float k_wheelMultiplier{60.0f};

constexpr int k_fontSizeTitle{48};
constexpr int k_fontSizeSubTitle{32};
constexpr int k_fontSizeBody{16};
constexpr int k_fontSpacing{2};

constexpr std::string_view k_title{"About"};
constexpr std::string_view k_subTitle{"Cpp-Weather"};

float MaxScroll(float contentHeight, float screenHeight) noexcept
{
    return std::max(0.0f, contentHeight - screenHeight);
}

// TODO: Move function out of private namespace, to allow other layers to use.
void DrawTextInBounds(Rectangle bounds, const char* text, Font font, float fontsize, float spacing)
{
    const unsigned int textLength{TextLength(text)}; // length in bytes.

    const float scaleFactor{fontsize / static_cast<float>(font.baseSize)};

    const float xPadding{8.0f};    // padding between left bound and text start.
    const float Y_INCREMENT{(font.baseSize + static_cast<float>(font.baseSize / 10.0f) * scaleFactor)};// distance between lines.

    // (x + xOffSet, y + yOffSet)
    float yOffSet{8.0f};  // y off set from y-origin: intialize padding between top bound and first text line.
    float xOffSet{xPadding};  // x off set from begining of line to next / current character.

    enum class STATE:bool
    {
        MEASURE_STATE = 0,
        DRAW_STATE = 1
    } state{STATE::MEASURE_STATE};

    int startLine{-1};
    int endLine{-1};

    for (int byteIndex{0}; byteIndex < textLength; ++byteIndex)
    {
        int codepointByteCount{0};
        int codepoint{GetCodepoint(&text[byteIndex], &codepointByteCount)};
        int index{GetGlyphIndex(font, codepoint)};

        // if error defaults to '?'
        if (codepoint == 0x3f/*?*/)
        {
            codepointByteCount = 1;
        }

        // increment by current characters byte size '-1' since ++byteIndex.
        byteIndex += (codepointByteCount - 1);

        float glyphWidth{0};
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ?
                        font.recs[index].width * scaleFactor :
                        font.glyphs[index].advanceX * scaleFactor;

            if (byteIndex + 1 < textLength)
            {
                glyphWidth = glyphWidth + spacing;
            }
        }

        // measure how many characters we can put on a line until rectangle bounds is reached.
        if (state == STATE::MEASURE_STATE)
        {
            // when word bounding is encountered, save state as a clean place for wrapping.
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n'))
            {
                endLine = byteIndex;
            }


            // edge of bounds reached, find safest endline before drawing.
            if ((xOffSet + glyphWidth) > bounds.width)
            {
                // no word boundary encountered, end on current character.
                if (endLine < 0)
                {
                    endLine = byteIndex;
                }
                // overflow character is a word bound, remove it.
                else if (endLine == byteIndex)
                {
                    endLine -= codepointByteCount;
                }

                state = STATE::DRAW_STATE;
            }
            // end of string.
            else if ((byteIndex + 1) == textLength)
            {
                endLine = byteIndex;
                state = STATE::DRAW_STATE;
            }
            else if (codepoint == '\n')
            {
                state = STATE::DRAW_STATE;
            }

            // stopped measuring save character position.
            if (state == STATE::DRAW_STATE)
            {
                xOffSet = xPadding;
                glyphWidth = 0;
                byteIndex = startLine;
            }
        }
        // Draw text.
        else
        {
            // bottom bound reach, stop drawing.
            if ((yOffSet + font.baseSize * scaleFactor) > bounds.height)
            {
                break;
            }
            if (codepoint == '\n')
            {
                yOffSet += Y_INCREMENT;
                xOffSet = xPadding;
            }
            // Draw current character glyph.
            else if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint(font, codepoint, Vector2{bounds.x + xOffSet, bounds.y + yOffSet}, fontsize, BLACK);
            }

            // reset.
            if (byteIndex == endLine)
            {
                xOffSet = xPadding;
                yOffSet += Y_INCREMENT;

                startLine = endLine;
                endLine = -1;
                state = STATE::MEASURE_STATE;
            }
        }

        // avoid leading spaces.
        if ((xOffSet != 0) || (codepoint != ' '))
        {
            xOffSet += glyphWidth;
        }
    }// for (int byteIndex{0}; byteIndex < textLength; ++byteIndex)
}

float ProcessWrappedText(Font font, const char* text, Rectangle bounds, float fontsize, float fontspacing, Color color, bool toDraw)
{
    const float lineHeight{fontsize + fontspacing};
    float xPos{bounds.x};
    float yPos{bounds.y};

    std::string word;
    std::string currentLine;

    std::istringstream stream{text};

    auto flushLine = [&](const std::string& line)
    {
        if (toDraw && !line.empty())
        {
            DrawTextEx(font, line.c_str(), {xPos, yPos}, fontsize, fontspacing, color);
        }

        yPos += lineHeight;
        currentLine.clear();
    };

    while (stream >> word)
    {
        std::string canidate{currentLine.empty() ? word : currentLine + " " + word};
        Vector2 textSize{MeasureTextEx(font, canidate.c_str(), fontsize, fontspacing)};

        if (textSize.x > bounds.width && !currentLine.empty())
        {
            flushLine(currentLine);
            currentLine = word;
        }
        else
        {
            currentLine = canidate;
        }
    }

    if (!currentLine.empty())
    {
        flushLine(currentLine);
    }

    return yPos - bounds.y;
}

float MeasureWrappedTextHeight(Font font, const char* text, float rectwidth, float fontsize, float spacing)
{
    Rectangle panel{0, 0, rectwidth, 0};
    return ProcessWrappedText(font, text, panel, fontsize, spacing, WHITE, false);
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
    m_font = LoadFont("resources/fonts/UbuntuMonoNerdFontMono-Regular.ttf");
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

    // mouse wheel scroll.
    float wheel{GetMouseWheelMove()};
    if (wheel != 0.0f)
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

    // Audio.
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
    const int yStarterPos{60};
    const int textWidth{MeasureText(k_title.data(), k_fontSizeTitle)};

    const int xText{(m_screenWidth - textWidth) / 2};
    const int yText{static_cast<int>(yStarterPos - m_scrollOffset)};
    const int xTextShadow{xText + 2};
    const int yTextShadow{yText + 2};

    // Only draw if we are above the screenheight (bottom of screen) or below the -fontsize (past the top to include text).
    if (yText > -k_fontSizeTitle && yText < m_screenHeight)
    {
        DrawText(k_title.data(), xTextShadow, yTextShadow, k_fontSizeTitle, Color{0, 0, 0, 100});
        DrawText(k_title.data(), xText, yText, k_fontSizeTitle, WHITE);
    }
}

void AboutLayer::DrawSubTitle() const
{
    const int yStarterPos{128};
    const Vector2 textsize{MeasureTextEx(m_font, k_subTitle.data(), k_fontSizeSubTitle, k_fontSpacing)};

    const float x{(m_screenWidth - textsize.x) / 2};
    const float y{yStarterPos - m_scrollOffset};

    // Only draw if we are above the screenheight (bottom of screen) or below the -fontsize (past the top to include text).
    if (y > -k_fontSizeSubTitle && y < m_screenHeight)
    {
        DrawTextEx(m_font, k_subTitle.data(), Vector2{x, y}, k_fontSizeSubTitle, k_fontSpacing, m_colorRandom);

        // Add padding between text and rectangle line.
        const float linepadding{8};
        const float linepadding2{16};
        Rectangle rectline{x - linepadding, y - linepadding, textsize.x + linepadding2, textsize.y + linepadding2};
        DrawRectangleLinesEx(rectline, 4, MAROON);
    }
}

void AboutLayer::DrawBody() const
{
    const char* body =
    "Revolution is not a crime; rebellion is justified!";

    const float x_origin{m_screenWidth / 8.0f};
    const float y_origin{(m_screenHeight / 4.0f) - m_scrollOffset};

    const float rectWidth{276.0f};
    const float rectHeight = MeasureWrappedTextHeight(m_font, body, rectWidth, k_fontSizeBody, k_fontSpacing);

    if (y_origin > -1.0f * rectHeight && y_origin < (m_screenHeight + rectHeight))
    {
        const Rectangle rect{x_origin, y_origin, rectWidth, rectHeight};

        DrawRectangleRec(rect, Fade(GREEN, 0.4f));
        ProcessWrappedText(m_font, body, rect, k_fontSizeBody, k_fontSpacing, RED, true);
    }
}

void AboutLayer::DrawOtherBody() const
{
    const char* quote = "All reactionaries are paper tigers. In appearance \
                        the reactionaries are terrifying, but in reality, \
                        they are not so powerfull.";

    const float xPosition{m_screenWidth * 0.25f};
    const float yPosition{m_screenHeight * 0.55f - m_scrollOffset};

    const float rectWidth{m_screenWidth * 0.5f};
    const float rectHeight{MeasureWrappedTextHeight(m_font, quote, rectWidth, k_fontSizeBody, k_fontSpacing)};

    if (yPosition > -1.0f * rectHeight && yPosition < (m_screenHeight + rectHeight))
    {
        const Rectangle rect{xPosition, yPosition, rectWidth, rectHeight};

        DrawRectangleRec(rect, Fade(BLUE, 0.5f));
        ProcessWrappedText(m_font, quote, rect, k_fontSizeBody, k_fontSpacing, GREEN, true);
    }
}
}// namespace Layers
