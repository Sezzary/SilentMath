#pragma once

namespace Silent::Game
{
    /** Seeks for the English title screen background graphic. */
    void GameFs_TitleGfxSeek(void);

    /** Loads the English title screen background graphic. */
    void GameFs_TitleGfxLoad(void);

    /** Seeks for the stream overlay. */
    void GameFs_StreamBinSeek(void);

    /** Loads the stream overlay. */
    void GameFs_StreamBinLoad(void);

    /** Loads the options background graphic and overlay. */
    void GameFs_OptionBinLoad(void);

    /** Loads the save/load background graphic and overlay. */
    void GameFs_SaveLoadBinLoad(void);
}
