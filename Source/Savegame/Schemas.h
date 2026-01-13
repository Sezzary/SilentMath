#pragma once

namespace Silent::Savegame::Schemas
{
    struct StuffData
    {
        int Stuff = 0;
    };

    struct ThingsData
    {
        int Things = 0;
    };

    struct Savegame
    {
        StuffData  Stuff  = {};
        ThingsData Things = {};
    };
}
