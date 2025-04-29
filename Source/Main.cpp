#include "Framework.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Renderer.h"

using namespace Silent::Renderer;

int main()
{
    auto app = HelloTriangleApplication();

    try
    {
        app.Run();
    }
    catch (const std::exception& ex)
    {
        Log(ex.what(), LogLevel::Error);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    /*auto app = ApplicationManager();

    // Run application.
    app.Initialize();
    app.Run();
    app.Deinitialize();

    return EXIT_SUCCESS;*/
}
