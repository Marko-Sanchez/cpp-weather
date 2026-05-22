#include "imageelement.h"
#include <raylib.h>

namespace render
{
ImageElement::ImageElement(Texture2D* image, const float xPosition, const float yPosition, const float rotation, const float scale, const Color color, const bool isBackground)
    :_image(image),
    _xPosition(xPosition),
    _yPosition(yPosition),
    _rotation(rotation),
    _scale(scale),
    _color(color),
    _isBackground(isBackground)
{
    _yParentScrollOffset = 0.0f;
}

void ImageElement::OnUpdate(const float scrollOffset)
{
    _yParentScrollOffset = scrollOffset;
}

void ImageElement::OnRender() const
{
    auto yPos {_yPosition};
    if (!_isBackground) yPos -= _yParentScrollOffset;

    DrawTextureEx(*_image, Vector2{_xPosition, yPos}, _rotation, _scale, _color);
}

Rectangle ImageElement::GetBounds() const
{
    auto yPos {_yPosition};
    if (_isBackground) yPos -= _yParentScrollOffset;

    return Rectangle{_xPosition, yPos, static_cast<float>(_image->width), static_cast<float>(_image->height)};
}
}// namespace render
