#include "imageelement.h"
#include <raylib.h>

namespace render
{
ImageElement::ImageElement(Texture2D* image, const float xPosition, const float yPosition, const float rotation, const float scale, const Color color, const bool isScrollable)
    :_image(image),
    _xPosition(xPosition),
    _yPosition(yPosition),
    _rotation(rotation),
    _scale(scale),
    _color(color),
    _isScrollable(isScrollable)
{}

void ImageElement::OnRender(const float scrollOffset) const
{
    auto yPos {_yPosition};
    if (_isScrollable) yPos -= scrollOffset;

    DrawTextureEx(*_image, Vector2{_xPosition, yPos}, _rotation, _scale, _color);
}

Rectangle ImageElement::GetBounds(const float scrollOffset) const
{
    return Rectangle{_xPosition, _yPosition, static_cast<float>(_image->width), static_cast<float>(_image->height)};
}
}// namespace render
