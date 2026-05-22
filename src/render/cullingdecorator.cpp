#include "cullingdecorator.h"

namespace render
{
CullingDecorator::CullingDecorator(std::unique_ptr<DrawableComponent>&& component, int width, int height)
    :ComponentDecorator(std::move(component)),
    _screenWidth(width),
    _screenHeight(height)
{}

void CullingDecorator::OnRender() const
{
    if (auto bounds = _component->GetBounds(); bounds.y > -1.0f * _screenHeight && bounds.y < (_screenHeight + bounds.height))
    {
        _component->OnRender();
    }
}
}// namespace render
