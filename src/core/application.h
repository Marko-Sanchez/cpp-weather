#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <cpp-httplib/httplib.h>

namespace Core
{
class Application
{
private:
    httplib::Client cli;

public:
    Application(const std::string& host, const int port);

    void Connect();
    void Run();
};
}// namespace Core
#endif
