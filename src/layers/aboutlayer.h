#ifndef ABOUTLAYER_H
#define ABOUTLAYER_H

#include "layer.h"

#include <raylib.h>
#include <list>

#include "render/drawablecomponent.h"

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

        bool  m_isPaused;

        Font      m_font;
        Music     m_redAudio;
        Texture2D m_redImage;

        std::list<std::unique_ptr<render::DrawableComponent>> m_composition;

        void DrawBackground() const;

    public:

        AboutLayer();
        ~AboutLayer();

        virtual void OnEvent() override;
        virtual void OnUpdate(float deltatime) override;
        virtual void OnRender() override;
};
}// namespace Layers
#endif
