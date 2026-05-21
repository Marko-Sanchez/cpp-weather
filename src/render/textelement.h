#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "drawablecomponent.h"
#include <raylib.h>

#include <string_view>

namespace render
{
class TextElement: public DrawableComponent
{
private:

    const Font* _font;

    const std::string_view _text;
    const float _xPosition;
    const float _yPosition;
    const float _textWidth;
    const float _fontSize;
    const float _fontSpacing;
    Color       _color;
    const bool  _isScrollable;

    float _textHeight;

public:

    TextElement(const Font* font, const std::string_view text, const float xPosition, const float yPosition, const float width, const float fontSize, const float fontSpacing, Color color, bool isScrollable);

    void OnRender(const float scrollOffset) const override;

    Rectangle GetBounds(const float scrollOffset) const override;
    void UpdateColor(const Color color) override;
};
}// namespace render
#endif
