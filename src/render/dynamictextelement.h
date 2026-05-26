#ifndef DYNAMICTEXTELEMENT_H
#define DYNAMICTEXTELEMENT_H

#include "drawablecomponent.h"
#include "utility/signal.h"
#include <functional>
#include <string>

namespace render
{
class DynamicTextElement: public DrawableComponent
{
    private:

        std::function<std::string(void)> _provider;
        utility::Signal::Token           _token;
        std::string                      _text;

        const Font* _font;
        const float _xPosition;
        const float _yPosition;
        const float _textWidth;
        const float _fontSize;
        const float _fontSpacing;
        Color       _color;

        float _textHeight;
        float _yParentScrolloffset;
        bool _doScroll;

        void RefreshData();

    public:

        DynamicTextElement(std::function<std::string(void)> provider, utility::Signal& signal, const Font* font, const std::string& text, const float xPosition, const float yPosition, const float textWidth, const float fontSize, const float fonstSpacing, const Color color, bool scroll = true);

        void OnUpdate(const float scrollOffset) override;
        void OnRender() const override;

        Rectangle GetBounds() const override;
        void UpdateColor(const Color color) override;

};
}// namespace render
#endif
