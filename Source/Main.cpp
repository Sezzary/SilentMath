#include "Framework.h"

#include "Engine/Application.h"

int main()
{
    auto app = ApplicationManager();

    try
    {
        app.Initialize();
        app.Run();
        app.Deinitialize();
    }
    catch (const std::exception& ex)
    {
        Log(ex.what(), LogLevel::Error);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
