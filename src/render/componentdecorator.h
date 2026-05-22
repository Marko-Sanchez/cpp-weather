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
        void OnUpdate(const float scrollOffset = 0.0f) override {_component->OnUpdate(scrollOffset);}
        void OnRender() const override {_component->OnRender();}

        Rectangle GetBounds() const override {return _component->GetBounds();}
        void UpdateColor(const Color color) override {_component->UpdateColor(color);}
};
}// namespace render
#endif
