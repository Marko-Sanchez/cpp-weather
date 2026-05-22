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
        Color            _color;
        float            _yParentScrollOffset;
        const bool       _isBackground;

    public:

        ImageElement(Texture2D* image, const float xPosition, const float yPosition, const float rotation, const float scale, const Color color, const bool isBackground);

        void OnUpdate(const float scrollOffset) override;
        void OnRender() const override;

        Rectangle GetBounds() const override;
};
}// namespace render
#endif
