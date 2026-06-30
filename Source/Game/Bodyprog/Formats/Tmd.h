#pragma once

#include "Psx.h"

namespace Silent::Game
{
    /** @brief TMD model file info. */
    struct s_TmdFile
    {
        s32               id;
        s32               flags;
        s32               modelCount;
        //TMD_STRUCT models[1];
    };
}
