#include "Framework.h"
#include "Utils/Utils.h"

#include "Application.h"
#include "Renderer/Renderer.h"

using namespace Silent::Renderer;

namespace Silent::Utils
{
    std::string ToUpper(const std::string& str)
    {
        auto transformedStr = str;
        std::transform(transformedStr.begin(), transformedStr.end(), transformedStr.begin(), [](uchar c)
        {
            return std::toupper(c);
        });

        return transformedStr;
    }

    std::string ToLower(const std::string& str)
    {
        auto transformedStr = str;
        std::transform(transformedStr.begin(), transformedStr.end(), transformedStr.begin(), [](uchar c)
        {
            return std::tolower(c);
        });

        return transformedStr;
    }

    char* CopyString(const char src[], uint size)
    {
        char* dest = (char*)malloc((size + 1) * sizeof(char));
        if (dest != nullptr)
        {
            strcpy(dest, src);
        }

        return (char*)dest;
    }
}
