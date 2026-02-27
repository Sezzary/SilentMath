#include "Framework.h"
#include "Game/Main/FsMem.h"

namespace Silent::Game
{
    s_FsMemState g_FsMemory;

    void Fs_InitializeMem(u8* start, u32 size)
    {
        // @stub
    }

    void Fs_AllocMem(s32 size)
    {
        // @stub
    }

    static inline u8* Fs_ClampToHeapBounds(u8* ptr)
    {
        // @stub
        return ptr;
    }

    s32 Fs_ClampMemBlock(u8* start, u8* end)
    {
        // @stub
        return 0;
    }

    bool Fs_FreeMem(u8* ptr)
    {
        // @stub
        return true;
    }

    void Fs_RelinkMemBlock(s_FsMemBlock* from, s_FsMemBlock* to, u8* start, u32 size)
    {
        // @stub
    }
}
