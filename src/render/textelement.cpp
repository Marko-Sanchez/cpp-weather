#include "textelement.h"
#include <raylib.h>

#include "textdecorator.h"

namespace render
{
TextElement::TextElement(const Font* font, const std::string_view text, const float xPosition, const float yPosition, const float width, const float fontSize, const float fontSpacing, Color color, bool isScrollable)
    :_font(font),
    _text(text),
    _xPosition(xPosition),
    _yPosition(yPosition),
    _textWidth(width),
    _fontSize(fontSize),
    _fontSpacing(fontSpacing),
    _color(color),
    _isScrollable(isScrollable)
{
    _textHeight = MeasureWrappedTextHeight(*_font, _text.data(), _textWidth, _fontSize, _fontSpacing);
}

void TextElement::OnRender(const float scrollOffset) const
{
    const Rectangle bounds{GetBounds(scrollOffset)};
    render::DrawWrappedText(*_font, bounds, _text.data(), _fontSize, _fontSpacing, _color);
}

Rectangle TextElement::GetBounds(const float scrollOffset) const
{
    auto yPos {_yPosition};
    if (_isScrollable) yPos -= scrollOffset;

    return Rectangle{_xPosition, yPos, _textWidth, _textHeight};
}

void TextElement::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
