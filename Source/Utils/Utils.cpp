#include "Framework.h"
#include "Utils/Utils.h"

namespace Silent::Utils
{
    /** @brief Copies `sign`'s sign bit. */
    int CopySign(int value, int sign)
    {
        if ((sign < 0 && value > 0) || (sign >= 0 && value < 0))
        {
            return -value;
        }

        return value;
    }
}
