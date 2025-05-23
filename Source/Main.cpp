#include "Framework.h"

#include "Engine/Application.h"

int main()
{
    try
    {
        g_App.Initialize();
        g_App.Run();
        g_App.Deinitialize();
    }
    catch (const std::exception& ex)
    {
        Log(std::string("Exception: ") + ex.what(), LogLevel::Error);
        return EXIT_FAILURE;
    }

    DeinitializeDebug();
    return EXIT_SUCCESS;
}
