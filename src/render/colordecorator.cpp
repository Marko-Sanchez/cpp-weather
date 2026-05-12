#include "colordecorator.h"

namespace render
{
ColorDecorator::ColorDecorator(std::unique_ptr<DrawableComponent>&& component)
    :ComponentDecorator(std::move(component)),
    _color(RED),
    _frameCounter(0)
{}

void ColorDecorator::OnEvent()
{
    if (++_frameCounter % 45 == 0)
    {
        _color.r = static_cast<char>(GetRandomValue(0, 255));
        _color.g = static_cast<char>(GetRandomValue(0, 255));
        _color.b = static_cast<char>(GetRandomValue(0, 255));
        _color.a = 200;

        _frameCounter = 0;
    }

    _component->OnEvent();
}

void ColorDecorator::OnUpdate(const float delta)
{
    _component->UpdateColor(_color);

    _component->OnUpdate(delta);
}
}// namespace render
