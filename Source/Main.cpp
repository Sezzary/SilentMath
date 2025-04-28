#include "Framework.h"

#include "Engine/Application.h"

using namespace Silent::Utils;

int main()
{
    auto app = ApplicationManager();

    // Run application.
    app.Initialize();
    app.Run();
    app.Deinitialize();
}
