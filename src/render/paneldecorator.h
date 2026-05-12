#ifndef PANELDECORATOR_H
#define PANELDECORATOR_H

#include "componentdecorator.h"

#include <memory>
#include <raylib.h>

namespace render
{
class PanelDecorator: public ComponentDecorator
{
    private:

        Color _color;

    public:

        PanelDecorator(std::unique_ptr<DrawableComponent>&& component, Color color);

        void OnRender(const float scrollOffset) const override;

        void UpdateColor(const Color color) override;
};
}// namespace render
#endif
