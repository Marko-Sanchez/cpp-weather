#ifndef COMPONENTDECORATOR_H
#define COMPONENTDECORATOR_H

#include "drawablecomponent.h"

#include <raylib.h>
#include <memory>

namespace render
{
class ComponentDecorator: public DrawableComponent
{
    protected:

        std::unique_ptr<DrawableComponent> _component;

    public:

        ComponentDecorator(std::unique_ptr<DrawableComponent>&& component)
            :_component(std::move(component))
        {}

        void OnEvent() override {_component->OnEvent();}
        void OnUpdate(const float delta = 0.0f) override {_component->OnUpdate(delta);}
        void OnRender(const float scrollOffset = 0.0f) const override {_component->OnRender(scrollOffset);}

        Rectangle GetBounds(const float scrollOffset = 0.0f) const override {return _component->GetBounds(scrollOffset);}
        void UpdateColor(const Color color) override {_component->UpdateColor(color);}
};
}// namespace render
#endif
