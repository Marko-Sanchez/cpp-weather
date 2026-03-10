#include "aboutmelayer.h"

#include <raylib.h>

#include "titlelayer.h"

namespace Layers
{
AboutMeLayer::AboutMeLayer():
m_screenWidth(512),
m_screenHeight(1024)
{
    m_toptext = "C++ programmer";
    m_bottomtext = "A desktop application";

    m_textposition = {m_screenWidth / 3, m_screenHeight / 4};
}

void AboutMeLayer::OnEvent()
{
    if (IsKeyPressed(KEY_ONE))
    {
        TransitionTo<Layers::TitleLayer>();
    }
}

void AboutMeLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(m_toptext.data(), m_textposition.x, m_textposition.y, 16, BLACK);
        DrawText(m_bottomtext.data(), m_textposition.x, m_textposition.y * 3, 16, BLACK);
    EndDrawing();
}
}// namespace Layers
