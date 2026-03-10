#include "layer.h"

namespace Layers
{
/*
 * Set lambda that allows for layers to be swapped in application class.
 */
void Layer::SetTransitionCallback(std::function<void(std::unique_ptr<Layer>)> cb)
{
    m_transitionCallback = std::move(cb);
}

/*
 * Adds layer to transition queue.
 */
void Layer::QueueTransition(std::unique_ptr<Layer> newLayer)
{
    if (m_transitionCallback)
    {
        m_transitionCallback(std::move(newLayer));
    }
}
}// namespace Layers
