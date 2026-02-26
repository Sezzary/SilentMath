#pragma once

namespace Silent::Game
{
    void MainMenu_SelectedOptionIdxReset(void);

    void Gfx_MainMenu_MainTextDraw(void);

    void Gfx_MainMenu_DifficultyTextDraw(s32 idx);

    void Gfx_MainMenu_BackgroundDraw(void);

    void func_8003B7BC(void);

    u32 func_8003B7FC(s32 idx);

    PACKET* Gfx_MainMenu_FogPacketGet(GsOT* ot, PACKET* packet);

    void Gfx_MainMenu_FogDraw(void);

    void Gfx_MainMenu_FogRandomize(void);

    void Gfx_MainMenu_FogScatter(void);

    void Gfx_MainMenu_FogUpdate(void);

    /** Related to main menu fog. */
    void func_8003BCF4(void);
}
