#include "TitleLayer.h"

namespace Layers
{
TitleLayer::TitleLayer():
m_screenWidth(512),
m_screenHeight(1024)
{
    m_backgroundImage = LoadTexture("resources/images/squiggle2.png");

    int textureWidth{MeasureText("cppweather", 42)};
    m_textSize = Vector2((static_cast<float>(m_screenWidth) - textureWidth) / 2, static_cast<float>(m_screenHeight) / 2);
}

TitleLayer::~TitleLayer()
{
    UnloadTexture(m_backgroundImage);
}

void TitleLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(m_backgroundImage, m_screenWidth / 2 - m_backgroundImage.width / 2, 0, WHITE);
        DrawText("cppweather", m_textSize.x, m_textSize.y, 42, BLACK);
    EndDrawing();
}
}
