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

        const float _panelRoundness {0.2f};
        const int   _panelSegments  {0};

        Color _color;
        bool  _isRounded;

    public:

        PanelDecorator(std::unique_ptr<DrawableComponent>&& component, const Color color, const bool isRounded = false);

        void OnRender(const float scrollOffset) const override;

        void UpdateColor(const Color color) override;
};
}// namespace render
#endif
