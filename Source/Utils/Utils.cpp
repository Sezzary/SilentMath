#include "Framework.h"
#include "Utils/Utils.h"

namespace Silent::Utils
{
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
