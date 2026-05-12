#ifndef CULLINGDECORATOR_H
#define CULLINGDECORATOR_H

#include "componentdecorator.h"
#include <memory>

namespace render
{
class CullingDecorator: public ComponentDecorator
{
    private:

        const int _screenWidth;
        const int _screenHeight;

    public:

        CullingDecorator(std::unique_ptr<DrawableComponent>&& component, int width, int height);

        void OnRender(const float scrollOffset) const override;
};
}// namespace render
#endif
