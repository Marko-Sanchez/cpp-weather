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

        const Font* _font;

        std::function<std::string(void)> _provider;
        utility::Signal::Token _token;
        std::string _text;

        const float _xPosition;
        const float _yPosition;
        const float _textWidth;
        const float _fontSize;
        const float _fontSpacing;
        const bool  _isScrollable;

        Color _color;
        float _textHeight;

        void RefreshText();

    public:

        DynamicTextElement(Font* font, std::function<std::string(void)> provider, utility::Signal& signal, const std::string& text, const float xPosition, const float yPosition, const float textWidth, const float fontSize, const float fonstSpacing, const Color color, bool isScrollable);

        void OnRender(const float scrollOffset) const override;

        Rectangle GetBounds(const float scrollOffset) const override;
        void UpdateColor(const Color color) override;

};
}// namespace render
#endif
