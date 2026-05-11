#include "cullingdecorator.h"

namespace render
{
CullingDecorator::CullingDecorator(std::unique_ptr<DrawableComponent>&& component, int width, int height)
    :_component(std::move(component)),
    _screenWidth(width),
    _screenHeight(height)
{}

void CullingDecorator::OnEvent()
{
    _component->OnEvent();
}

void CullingDecorator::OnUpdate(const float delta)
{
    _component->OnUpdate(delta);
}

void CullingDecorator::OnRender(const float scrollOffset) const
{

    auto bounds = _component->GetBounds(scrollOffset);
    if (bounds.y > -1.0f * _screenHeight && bounds.y < (_screenHeight + bounds.height))
    {
        _component->OnRender(scrollOffset);
    }
}

Rectangle CullingDecorator::GetBounds(const float scrollOffset) const
{
    return _component->GetBounds(scrollOffset);
}
}// namespace render
