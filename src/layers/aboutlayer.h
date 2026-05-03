#ifndef ABOUTLAYER_H
#define ABOUTLAYER_H

#include "layer.h"

#include <raylib.h>

namespace Layers
{
class AboutLayer: public Layer
{
private:

    const int m_screenWidth;
    const int m_screenHeight;

    float m_scrollOffset;
    float m_targetScroll; // how far down the page should we be.
    float m_contentHeight;// max length to scroll down to.

    int   m_framecounter;
    bool  m_isPaused;
    Color m_colorRandom;

    Font      m_font;
    Music     m_redAudio;
    Texture2D m_redImage;

    // Draw Helpers.
    void DrawBackground() const;
    void DrawTitle() const;
    void DrawSubTitle() const;
    void DrawBody() const;
    void DrawOtherBody() const;
    void DrawScrollbar() const;

public:

    AboutLayer();
    ~AboutLayer();

    virtual void OnEvent() override;
    virtual void OnUpdate(float deltatime) override;
    virtual void OnRender() override;
};
}// namespace Layers
#endif
