#pragma once

namespace Silent::Game
{
    /** `Demo_FrameCount` */
    extern s32 g_Demo_FrameCount;

    // TODO: Make local. Used in `Demo_Update`.
    extern char D_8002B2D8[]; // "STEP ERROR:[H:"
    extern char D_8002B2E8[]; // "]/[M:"
    extern char D_8002B2F0[]; // "]"

    extern s32 g_Demo_DemoId;

    extern u16 g_Demo_RandSeed;

    extern s32 D_800AFDF0;

    extern e_FsFile g_Demo_DemoFileIdx;

    extern e_FsFile g_Demo_PlayFileIdx;

    extern s_SaveUserConfig g_Demo_UserConfigBackup;

    extern u32 g_Demo_PrevRandSeed;

    extern u32 g_Demo_RandSeedBackup;

    extern s32 g_Demo_DemoStep;

    extern s32 g_Demo_VideoPresentInterval;

    /** Something for demo handling. Set in `Fs_QueueDoThingWhenEmpty`. */
    extern bool D_800C489C;

    bool Demo_SequenceAdvance(s32 incrementAmt);

    void Demo_DemoDataRead(void);

    void Demo_PlayDataRead(void);

    s32 Demo_PlayFileBufferSetup(void);

    void Demo_DemoFileSavegameUpdate(void);

    void Demo_GameGlobalsUpdate(void);

    void Demo_GameGlobalsRestore(void);

    void Demo_GameRandSeedUpdate(void);

    void Demo_GameRandSeedRestore(void);

    void Demo_Start(void);

    void Demo_Stop(void);

    bool func_8008F434(s32 arg0);

    /** @brief Gets the active demo state.
     *
     * @param gameState Active game state (`e_GameState`).
     * @return Active demo state `(e_DemoState`).
     */
    s32 Demo_StateGet(s32 gameState);

    void Demo_ExitDemo(void);

    bool func_8008F520(void);

    /** Generates the backup random demo seed and stores it in `Demo_RandSeedBackup`. */
    void Demo_DemoRandSeedBackup(void);

    void Demo_DemoRandSeedRestore(void);

    void Demo_DemoRandSeedAdvance(void);

    bool Demo_Update(void);

    bool Demo_ControllerDataUpdate(void);

    bool Demo_PresentIntervalUpdate(void);

    bool Demo_GameRandSeedSet(void);

    bool func_8008F914(q19_12 posX, q19_12 posZ);
}
