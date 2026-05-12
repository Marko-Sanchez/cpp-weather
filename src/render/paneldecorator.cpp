#include "paneldecorator.h"

#include <raylib.h>

namespace render
{
PanelDecorator::PanelDecorator(std::unique_ptr<DrawableComponent>&& component, Color color)
    :ComponentDecorator(std::move(component)),
    _color(color)
{}

void PanelDecorator::OnRender(const float scrollOffset) const
{
    auto panel = _component->GetBounds(scrollOffset);
    DrawRectangleRec(panel, _color);

    _component->OnRender(scrollOffset);
}

void PanelDecorator::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
