#ifndef CULLINGDECORATOR_H
#define CULLINGDECORATOR_H

#include "drawablecomponent.h"
#include <memory>

namespace render
{
class CullingDecorator: public DrawableComponent
{
    private:

        std::unique_ptr<DrawableComponent> _component;

        const int _screenWidth;
        const int _screenHeight;

    public:

        CullingDecorator(std::unique_ptr<DrawableComponent>&& component, int width, int height);

        void OnEvent() override;
        void OnUpdate(const float delta) override;
        void OnRender(const float scrollOffset) const override;

        Rectangle GetBounds(const float scrollOffset) const override;
};
}// namespace render
#endif
