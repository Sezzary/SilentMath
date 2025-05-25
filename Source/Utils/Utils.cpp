#include "Framework.h"
#include "Utils/Utils.h"

namespace Silent::Utils
{
    std::string ToUpper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
        {
            return std::toupper(c);
        });

        return str;
    }

    std::string ToLower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
        {
            return std::tolower(c);
        });

        return str;
    }

    char* CopyString(const char src[], uint size)
    {
        char* dest = (char*)malloc(((size + 1) * sizeof(char)));
        if (dest != nullptr)
        {
            strcpy(dest, src);
        }

        return (char*)dest;
    }
}
