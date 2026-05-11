#ifndef COLORDECORATOR_H
#define COLORDECORATOR_H

#include "drawablecomponent.h"

#include <memory>
#include <raylib.h>

namespace render
{
class ColorDecorator: public DrawableComponent
{
    private:

        std::unique_ptr<DrawableComponent> _component;

        Color _color;
        int   _frameCounter;

    public:

        ColorDecorator(std::unique_ptr<DrawableComponent>&& component);

        void OnEvent() override;
        void OnUpdate(const float delta) override;
        void OnRender(const float scrollOffset) const override;

        Rectangle GetBounds(const float scrollOffset) const override;
};
}// namespace render
#endif
