#include "textelement.h"
#include <raylib.h>

#include "textdecorator.h"

namespace render
{
TextElement::TextElement(const Font* font, const std::string_view text, const float xPosition, const float yPosition, const float width, const float fontSize, const float fontSpacing, Color color, bool scroll)
    :_font(font),
    _text(text),
    _xPosition(xPosition),
    _yPosition(yPosition),
    _textWidth(width),
    _fontSize(fontSize),
    _fontSpacing(fontSpacing),
    _color(color),
    _doScroll(scroll)
{
    _textHeight = MeasureWrappedTextHeight(*_font, _text.data(), _textWidth, _fontSize, _fontSpacing);
    _yParentScrolloffset = 0.0f;
}

void TextElement::OnUpdate(const float scrollOffset)
{
    _yParentScrolloffset = scrollOffset;
}

void TextElement::OnRender() const
{
    const Rectangle bounds{GetBounds()};
    render::DrawWrappedText(*_font, bounds, _text.data(), _fontSize, _fontSpacing, _color);
}

Rectangle TextElement::GetBounds() const
{
    auto yPos {_yPosition};
    if (_doScroll)
    {
        yPos -= _yParentScrolloffset;
    }

    return Rectangle{_xPosition, yPos, _textWidth, _textHeight};
}

void TextElement::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
