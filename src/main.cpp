#include "cppweather/cppweather.h"
#include "core/application.h"
#include "utility/commandlineparser.h"
#include <print>

int main(int argc, char* argv[])
{
    utility::CommandLineArgs args{std::move(utility::ParseArgs(argc, argv))};

    if (args.showversion)
    {
        std::println("cppweather {} ({})", CPPWEATHER_VERSION, CPPWEATHER_GIT_HASH);

        return 0;
    }
    else if (args.nogui)
    {
        Core::Application app("cppweather", CPPWEATHER_VERSION);
        app.GetWebContents();

        return 0;
    }
    else
    {
        Core::Application app("cppweather", CPPWEATHER_VERSION);
        app.Run();

        return 0;
    }
}
