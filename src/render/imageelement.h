#ifndef IMAGEELEMENT_H
#define IMAGEELEMENT_H

#include "drawablecomponent.h"
#include <raylib.h>

namespace render
{
class ImageElement: public DrawableComponent
{
private:

    const Texture2D* _image;
    const float      _xPosition;
    const float      _yPosition;
    const float      _rotation;
    const float      _scale;
    const bool       _isScrollable;

    Color      _color;

public:

    ImageElement(Texture2D* image, const float xPosition, const float yPosition, const float rotation, const float scale, const Color color, const bool isScrollable);

    void OnRender(const float scrollOffset) const override;

    Rectangle GetBounds(const float scrollOffset) const override;
};
}// namespace render
#endif
