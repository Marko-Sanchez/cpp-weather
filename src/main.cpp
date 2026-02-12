#include <print>
#include <string_view>

#include "cppweather/cppweather.h"
#include "core/application.h"

int main(int argc, char* argv[])
{
    if (argc > 1 && std::string_view(argv[1]) == "--version")
    {
        std::println("cppweather {} ({})", CPPWEATHER_VERSION, CPPWEATHER_GIT_HASH);
        return 0;
    }

    Core::Application app("http://api.open-meteo.com", 8080);
    app.GetWebContents();

    return 0;
}
