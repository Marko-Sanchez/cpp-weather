#include "paneldecorator.h"

#include <raylib.h>

namespace render
{
PanelDecorator::PanelDecorator(std::unique_ptr<DrawableComponent>&& component, const Color color, const bool isRounded)
    :ComponentDecorator(std::move(component)),
    _color(color),
    _isRounded(isRounded)
{}

void PanelDecorator::OnRender() const
{
    auto panel = _component->GetBounds();
    if (_isRounded)
    {
        DrawRectangleRounded(panel, _panelRoundness, _panelSegments, _color);
    }
    else
    {
        DrawRectangleRec(panel, _color);
    }

    _component->OnRender();
}

void PanelDecorator::UpdateColor(const Color color)
{
    _color = color;
}
}// namespace render
