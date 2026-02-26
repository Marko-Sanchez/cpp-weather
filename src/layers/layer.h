#ifndef LAYER_H
#define LAYER_H

#include <concepts>
#include <memory>

namespace Layers
{
class Layer
{
private:
    void QueueTransition(std::unique_ptr<Layer> newLayer);

public:
    virtual ~Layer() = default;

    virtual void OnEvent() {}
    virtual void OnRender() {}

    template <std::derived_from<Layer> T, typename... Args>
    void TransitionTo(Args&&... args)
    {
        QueueTransition(std::unique_ptr<T>(std::forward<Args>(args)...));
    }
};
}// namespace Layers
#endif
