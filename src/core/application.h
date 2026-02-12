#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include "core/network.h"

namespace Core
{
class Application
{
private:
    Core::Network m_weatherClient;

public:
    Application(const std::string& host, const int port);

    void GetWebContents();
    void Run();
};
}// namespace Core
#endif
