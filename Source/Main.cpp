#include "Framework.h"

#include "Engine/Application.h"

int main()
{
    auto app = ApplicationManager();

    // Run application.
    app.Initialize();
    app.Run();
    app.Deinitialize();
}
