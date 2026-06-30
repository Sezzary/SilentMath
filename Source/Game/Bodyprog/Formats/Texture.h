#pragma once

#include "Game/Main/FsQueue.h"

namespace Silent::Game
{
    /** @brief Model texture. */
    struct s_Texture
    {
        s_FsImageDesc imageDesc;
        u_Filename    name;
        u32           queueIdx;
        s8            refCount;
    };
}
