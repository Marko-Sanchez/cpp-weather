#include "titlelayer.h"

#include "aboutlayer.h"

namespace Layers
{
TitleLayer::TitleLayer():
m_screenWidth(512),
m_screenHeight(1024),
m_title("Cpp-Weather")
{
    m_backgroundTexture = LoadTexture("resources/images/title.png");
    m_isTextureLoaded = (m_backgroundTexture.id > 0);

    int textureWidth{MeasureText(m_title.c_str(), 42)};
    m_titleSize = Vector2((static_cast<float>(m_screenWidth) - textureWidth) / 2, 60);
}

TitleLayer::~TitleLayer()
{
    if (m_isTextureLoaded)
    {
        UnloadTexture(m_backgroundTexture);
    }
}

void TitleLayer::OnEvent()
{
    if (IsKeyPressed(KEY_ONE))
    {
        TransitionTo<Layers::AboutLayer>();
    }
}


void TitleLayer::OnRender()
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(m_backgroundTexture, -128, 0, WHITE);
        DrawText(m_title.c_str(), m_titleSize.x + 2, m_titleSize.y + 2, 48, BLACK);
        DrawText(m_title.c_str(), m_titleSize.x, m_titleSize.y, 48, WHITE);
    EndDrawing();
}
}
