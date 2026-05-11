#include "paneldecorator.h"

#include <raylib.h>

namespace render
{
PanelDecorator::PanelDecorator(std::unique_ptr<DrawableComponent>&& component, Color color)
    :_component(std::move(component)),
    _color(color)
{}

void PanelDecorator::OnEvent()
{
    _component->OnEvent();
}

void PanelDecorator::OnUpdate(const float delta)
{
    _component->OnUpdate(delta);
}

void PanelDecorator::OnRender(const float scrollOffset) const
{
    auto panel = _component->GetBounds(scrollOffset);
    DrawRectangleRec(panel, _color);

    _component->OnRender(scrollOffset);
}

Rectangle PanelDecorator::GetBounds(const float scrollOffset) const
{
    return _component->GetBounds(scrollOffset);
}

void PanelDecorator::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
