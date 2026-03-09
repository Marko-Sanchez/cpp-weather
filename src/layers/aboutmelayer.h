#ifndef ABOUTMELAYER_H
#define ABOUTMELAYER_H

#include "layer.h"

#include <raylib.h>

namespace Layers
{
class AboutMeLayer: public Layer
{
private:
    const float m_screenWidth;
    const float m_screenHeight;

    std::string_view m_toptext;
    std::string_view m_bottomtext;

    Vector2 m_textposition;

public:
    AboutMeLayer();

    virtual void OnEvent() override;
    virtual void OnRender() override;
};
}// namespace Layers
#endif
