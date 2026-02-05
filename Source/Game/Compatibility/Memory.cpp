#include "Framework.h"
#include "Game/Compatibility/Memory.h"

namespace Silent::Game
{
    constexpr uint32 SCRATCHPAD_SIZE       = 1024;             /** 1KB. */
    constexpr uint32 RAM_SIZE              = 2 * SQUARE(1024); /** 2MB. */
    constexpr uint32 SCRATCHPAD_START_ADDR = 0x1F800000;       /** CPU data cache. */
    constexpr uint32 RAM_MASK              = 0x00FFFFFF;       /** Mirrored RAM regions. */

    static auto VirtualScratchpad = std::array<byte, SCRATCHPAD_SIZE>{};
    static auto VirtualRam        = std::array<byte, RAM_SIZE>{};

    void* GetScratchpadPtr(uint32 addr)
    {
        uint32 offset = addr - SCRATCHPAD_START_ADDR;
        if (offset > SCRATCHPAD_SIZE)
        {
            std::runtime_error(Fmt("Attempted to get invalid virtual scratchpad pointer at offset {}.", offset));
        }

        return &VirtualScratchpad[offset];
    }

    void* GetRamPtr(uint32 addr)
    {
        uint32 offset = addr & RAM_MASK; 
        if (offset > RAM_SIZE)
        {
            std::runtime_error(Fmt("Attempted to get invalid virtual RAM pointer at offset {}.", offset));
        }

        return &VirtualRam[offset];
    }
}
