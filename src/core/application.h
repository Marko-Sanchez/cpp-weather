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

    Core::Network m_weatherclient;

    std::list<std::unique_ptr<Layers::Layer>> m_layerstack;

public:
    Application(const std::string windowname, const std::string version);
    ~Application();

    void GetWebContents();
    void Run();
    void RenderLayers();

    template<typename TLayer, typename ...Args>
    requires(std::derived_from<TLayer, Layers::Layer>)
    void PushLayer(Args&&... args)
    {
        m_layerstack.push_back(std::make_unique<TLayer>(std::forward<Args>(args)...));
    }
};
}// namespace Core
#endif
