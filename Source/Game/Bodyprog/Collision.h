#pragma once

namespace Silent::Game
{
    // Related to collision?
    typedef struct
    {
        u16            flags; /** `e_CollisionFlags` */
        u8             triggerZoneCount;
        u8             __pad_3;
        s_TriggerZone* triggerZones[20]; // Guessed size.
    } s_800C4478;

    // emoose: Also works: `extern u16 D_800C4478[];`, `arg0->field_4 = D_800C4478[0];`.
    // Didn't see any array accesses in Ghidra though, struct might be more likely.
    extern s_800C4478 D_800C4478;
}
