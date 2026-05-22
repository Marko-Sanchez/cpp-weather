#ifndef COLORDECORATOR_H
#define COLORDECORATOR_H

#include "componentdecorator.h"

#include <memory>
#include <raylib.h>

namespace render
{
class ColorDecorator: public ComponentDecorator
{
    private:

        Color _color;
        int   _frameCounter;

    public:

        ColorDecorator(std::unique_ptr<DrawableComponent>&& component);

        void OnEvent() override;
        void OnUpdate(const float scrollOffset) override;
};
}// namespace render
#endif
