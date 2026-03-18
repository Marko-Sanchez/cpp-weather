#ifndef LAYER_H
#define LAYER_H

#include <concepts>
#include <functional>
#include <memory>
#include <list>

namespace Layers
{
class Layer
{
private:
    std::function<void(std::unique_ptr<Layer>)> m_transitionCallback;
    void QueueTransition(std::unique_ptr<Layer> newLayer);

public:
    virtual ~Layer() = default;

    virtual void OnEvent() {}
    virtual void OnUpdate(float delta) {}
    virtual void OnRender() {}

    void SetTransitionCallback(std::function<void(std::unique_ptr<Layer>)> cb);

    template <std::derived_from<Layer> T, typename... Args>
    void TransitionTo(Args&&... args)
    {
        QueueTransition(std::make_unique<T>(std::forward<Args>(args)...));
    }
};

/*
 * Holds an iterator to the layer that is to be replaced, by 'pendinglayer'.
 */
struct TransitionLayer
{
    std::unique_ptr<Layer> pendinglayer;
    std::list<std::unique_ptr<Layer>>::iterator it;
};
}// namespace Layers
#endif
