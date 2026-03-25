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

    int m_framecounter;

    Font m_font;
    Texture2D m_backgroundTexture;

    void DrawTitle() const;
    void DrawImageCredits() const;

public:
    TitleLayer();
    ~TitleLayer();

    virtual void OnEvent() override;
    virtual void OnUpdate(float deltatime) override;
    virtual void OnRender() override;
};
}// namespace Layers
#endif
