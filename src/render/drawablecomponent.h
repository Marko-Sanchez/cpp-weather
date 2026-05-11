#ifndef DRAWABLEACOMPONENT_H
#define DRAWABLEACOMPONENT_H

#include <raylib.h>

namespace render
{
/*
 * Composing drawable objects to represent part-whole hierarchies. Where
 * a 'whole' object (in this case classes derived from 'Layer')
 * is composed of multiple "parts" (classes derived from 'DrawableComponent').
 *
 * These are then combined with *Decorators adding behaviors, without affecting
 * the drawable elements. Decorators must conform to *Elements, to avoid affecting them.
 */
class DrawableComponent
{
    public:

        virtual ~DrawableComponent() = default;

        virtual void OnEvent() {}
        virtual void OnUpdate(const float delta) {}
        virtual void OnRender(const float scrollOffset) const {}

        virtual Rectangle GetBounds(const float scrollOffset) const {return Rectangle{0.0f, 0.0f, 0.0f, 0.0f};}
        virtual void UpdateColor(const Color color) {}
};
}// namespace render
#endif
