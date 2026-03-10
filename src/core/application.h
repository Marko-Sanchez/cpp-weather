#ifndef APPLICATION_H
#define APPLICATION_H

#include <concepts>
#include <list>
#include <string>

#include "core/network.h"
#include "layers/layer.h"

namespace Core
{
class Application
{
private:
    const std::string m_windowname;
    const std::string m_applicationversion;

    Core::Network m_network;

    std::shared_ptr<std::optional<Layers::TransitionLayer>> m_queuedtransition;
    std::list<std::unique_ptr<Layers::Layer>> m_layerstack;

    std::function<void(std::unique_ptr<Layers::Layer>)> TransitionLayerLambda(std::list<std::unique_ptr<Layers::Layer>>::iterator iter);

public:
    Application(const std::string windowname, const std::string version, std::optional<std::pair<std::string, std::string>> stringlocation = {});
    ~Application();

    void GetWebContents();
    void Run();

    void OnEvent();
    void Update();
    void RenderLayers();

    template<typename TLayer, typename ...Args>
    requires(std::derived_from<TLayer, Layers::Layer>)
    void PushLayer(Args&&... args)
    {
        auto it = m_layerstack.insert(m_layerstack.end(), std::make_unique<TLayer>(std::forward<Args>(args)...));
        (*it)->SetTransitionCallback(TransitionLayerLambda(it));
    }
};
}// namespace Core
#endif
