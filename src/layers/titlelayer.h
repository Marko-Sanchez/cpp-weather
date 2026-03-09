#ifndef TITLELAYER_H
#define TITLELAYER_H

#include "layer.h"

#include <raylib.h>

namespace Layers
{
class TitleLayer: public Layer
{
private:
    const int m_screenWidth;
    const int m_screenHeight;

    Vector2 m_textSize;

    Texture2D m_backgroundImage;

public:
    TitleLayer();
    ~TitleLayer();

    virtual void OnRender() override;
};
}
#endif
