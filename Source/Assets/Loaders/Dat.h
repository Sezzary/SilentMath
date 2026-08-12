#pragma once

#include "Input/Input.h"

using namespace Silent::Input;

namespace Silent::Assets
{
    /** @brief DAT input recording user options configuration. */
    struct DatOptionsConfig
    {
        int  SoundType         = 0;
        int  VolumeBgm         = 0;
        int  VolumeSe          = 0;
        int  VibrationEnabled  = 0;
        int  Brightness        = 0;
        int  ExtraWeaponCtrl   = 0;
        int  ExtraBloodColor   = 0;
        bool EnableAutoLoad    = false;
        int  ExtraViewCtrl     = 0;
        int  ExtraViewMode     = 0;
        int  ExtraRetreatTurn  = 0;
        int  ExtraWalkRunCtrl  = 0;
        bool ExtraAutoAiming   = false;
        int  ExtraBulletAdjust = 0;
        int  PalLanguageId     = 0;
    };

    /** @brief DAT input recording savegame. */
    struct DatSavegame
    {

    };

    /** @brief DAT asset data. */
    struct DatAsset
    {
        DatOptionsConfig  Options            = {};
        DatSavegame       Savegame           = {};
        Recording         InputRecording     = {};
        uint              BaseSeed           = 0;
        std::vector<uint> Seeds              = {};
        std::vector<int>  ExpectedGameStates = {};
    };

    /** @brief Parses an DAT asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed DAT asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseDat(const stdfs::path& filename);
}
