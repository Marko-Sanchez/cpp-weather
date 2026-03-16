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

    const std::string m_title;
    Vector2 m_titleSize;

    Texture2D m_backgroundTexture;
    bool m_isTextureLoaded;

public:
    TitleLayer();
    ~TitleLayer();

    virtual void OnEvent() override;
    virtual void OnRender() override;
};
}
#endif
