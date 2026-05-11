#ifndef PANELDECORATOR_H
#define PANELDECORATOR_H

#include "drawablecomponent.h"

#include <memory>
#include <raylib.h>

namespace render
{
class PanelDecorator: public DrawableComponent
{
    private:

        std::unique_ptr<DrawableComponent> _component;

        Color _color;

    public:

        PanelDecorator(std::unique_ptr<DrawableComponent>&& component, Color color);

        void OnEvent() override;
        void OnUpdate(const float delta) override;
        void OnRender(const float scrollOffset) const override;

        Rectangle GetBounds(const float scrollOffset) const override;
        void UpdateColor(const Color color) override;
};
}// namespace render
#endif
