#include "dynamictextelement.h"
#include "raylib.h"
#include "render/textdecorator.h"

namespace render
{
DynamicTextElement::DynamicTextElement(std::function<std::string(void)> provider, utility::Signal& signal, const Font* font, const std::string& text, const float xPosition, const float yPosition, const float textWidth, const float fontSize, const float fonstSpacing, const Color color, bool isScrollable)
    :_provider(provider),
    _token(signal.Connect([this](){RefreshText();})),
    _font(font),
    _text(text),
    _xPosition(xPosition),
    _yPosition(yPosition),
    _textWidth(textWidth),
    _fontSize(fontSize),
    _fontSpacing(fonstSpacing),
    _color(color),
    _isScrollable(isScrollable)
{
    _textHeight = render::MeasureWrappedTextHeight(*_font, _text.data(), _textWidth, _fontSize, _fontSpacing);
}

void DynamicTextElement::RefreshText()
{
    _text = _provider();
}

void DynamicTextElement::OnRender(const float scrollOffset) const
{
    const Rectangle bounds {this->GetBounds(scrollOffset)};
    render::DrawWrappedText(*_font, bounds, _text.data(), _fontSize, _fontSpacing, _color);
}

Rectangle DynamicTextElement::GetBounds(const float scrollOffset) const
{
    auto yPos {_yPosition};
    if (_isScrollable) yPos -= scrollOffset;

    return Rectangle {_xPosition, yPos, _textWidth, _textHeight};
}

void DynamicTextElement::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
