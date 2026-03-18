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

    float m_scrollOffset{0.0f};
    float m_targetScroll{0.0f}; // how far down the page should we be.
    float m_contentHeight{0.0f};// max length to scroll dow to.

    // text handling.
    int m_framecounter{};
    Color m_colorRandom;
    Font m_font;

    // Draw Helpers.
    void DrawBackground() const;
    void DrawTitle() const;
    void DrawSubTitle() const;
    void DrawBody() const;
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
