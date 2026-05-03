#ifndef TEXTDECORATOR_H
#define TEXTDECORATOR_H

#include <raylib.h>

namespace render
{
inline float DrawTextInBounds(const Font font, const Rectangle bounds, const char* text, const float fontsize, const float spacing, const Color color, const bool toDraw)
{
    const int textLength     {static_cast<int>(TextLength(text))};
    const float scaleFactor  {fontsize / static_cast<float>(font.baseSize)};
    const float glyphHeight  {font.baseSize * scaleFactor};
    const float lineHeight   {glyphHeight + (font.baseSize / 10.0f) * scaleFactor};
    const float xPadding     {8.0f};    // padding between left bound and text start.

    // (x + xOffSet, y + yOffSet)
    float yOffSet {8.0f};  // y off set from y-origin: intialize padding between top bound and first text line.
    float xOffSet {xPadding};  // x off set from begining of line to next / current character.

    int startLine  {-1};
    int endLine    {-1};
    bool measuring {true};

    for (int i{0}; i < textLength; ++i)
    {
        int byteCount  {0};
        int codepoint  {GetCodepoint(&text[i], &byteCount)};
        int glyphIndex {GetGlyphIndex(font, codepoint)};

        if (codepoint == 0x3f) byteCount = 1;

        // increment by current characters byte size '-1' since ++byteIndex.
        i += (byteCount - 1);

        float glyphWidth{0.0f};
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[glyphIndex].advanceX == 0) ?
                font.recs[glyphIndex].width      * scaleFactor :
                font.glyphs[glyphIndex].advanceX * scaleFactor;

            if (i + 1 < textLength)
                glyphWidth = glyphWidth + spacing;
        }

        // measure how many characters we can put on a line until rectangle bounds is reached.
        if (measuring)
        {
            // when word bounding is encountered, save state as a clean place for wrapping.
            if (codepoint == ' ' || codepoint == '\t' || codepoint == '\n')
            {
                endLine = i;
            }


            // edge of bounds reached, find safest endline before drawing.
            if ((xOffSet + glyphWidth) > bounds.width)
            {
                // no word boundary encountered, end on current character.
                if (endLine < 0) endLine = i;
                // overflow character is a word bound, remove it.
                else if (endLine == i) endLine -= byteCount;

                measuring = false;
            }
            // end of string.
            else if ((i + 1) == textLength)
            {
                endLine = i;
                measuring = false;
            }
            else if (codepoint == '\n')
            {
                measuring = false;
            }

            // stopped measuring save character position.
            if (!measuring)
            {
                xOffSet    = xPadding;
                glyphWidth = 0.0f;
                i          = startLine;
            }
        }
        else // draw state.
        {
            // bottom bound reach, stop drawing.
            if (bounds.height > 0.0f && (yOffSet + glyphHeight) > bounds.height)
                break;

            if (codepoint == '\n')
            {
                yOffSet += lineHeight;
                xOffSet = xPadding;
            }
            // Draw current character glyph.
            else if (codepoint != ' ' && codepoint != '\t')
            {
                if (toDraw)
                {
                    DrawTextCodepoint(font, codepoint, Vector2{bounds.x + xOffSet, bounds.y + yOffSet}, fontsize, color);
                }
            }

            // draws complete, back to measuring next line.
            if (i == endLine)
            {
                xOffSet = xPadding;
                yOffSet += lineHeight;

                startLine = endLine;
                endLine   = -1;

                measuring = true;
            }
        }

        xOffSet += glyphWidth;
    }// for (int byteIndex{0}; byteIndex < textLength; ++byteIndex)

    return yOffSet + glyphHeight;
}// float DrawTextInBounds(const Font font, const Rectangle bounds, const char* text, const float fontsize, const float spacing, const Color color, const bool toDraw)

inline float DrawWrappedText(const Font font, const Rectangle panel, const char* text, const float fontsize, const float fontspacing, const Color color)
{
    return DrawTextInBounds(font, panel, text, fontsize, fontspacing, color, true);
}// float DrawWrappedText(const Font font, const char* text, const Rectangle panel, const float fontsize, const float fontspacing, const Color color)

inline float MeasureWrappedTextHeight(const Font font, const char* text, const float rectwidth, const float fontsize, const float fontspacing)
{
    return DrawTextInBounds(font, {0, 0, rectwidth, 0}, text, fontsize, fontspacing, WHITE, false);
}// float MeasureWrappedTextHeight(const Font font, const char* text, const float rectwidth, const float fontsize, const float fontspacing)
}// namespace render
#endif
