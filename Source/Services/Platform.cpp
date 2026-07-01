#include "Framework.h"
#include "Services/Platform.h"

namespace Silent::Services
{
    std::string GetPlatformName()
    {
        switch (PLATFORM_TYPE)
        {
            case PlatformType::Windows:
            {
                return "Windows";
            }
            case PlatformType::MacOs:
            {
                return "macOS";
            }
            case PlatformType::Linux:
            {
                return "Linux";
            }
        }

        return "Unknown";
    }

    stdfs::path GetHomeDirectory()
    {
#if defined(_WIN32) || defined(_WIN64)
        char*  buffer = nullptr;
        size_t length = 0;

        if (_dupenv_s(&buffer, &length, "USERPROFILE") == 0 && buffer != nullptr)
        {
            auto home = std::string(buffer);
            free(buffer);
            return stdfs::path(home);
        }

        return {};
#elif defined(__APPLE__) || defined(__linux__)
        return stdfs::path(getenv("HOME"));
#else
        return {};
#endif
    }
}
