#pragma once

namespace Silent::Game
{
    enum   e_CharaId;
    struct s_LmHeader;
    struct s_FsImageDesc;
    struct s_MapOverlayHeader;
    struct s_SubCharacter;

    bool Chara_Load(s32 modelIdx, s8 charaId, GsCOORDINATE2* coords, s8 forceFree, s_LmHeader* lmHdr, s_FsImageDesc* tex);

    bool Chara_ProcessLoads();

    bool Chara_ProcessLoads();

    void Chara_BonesInit(s32 idx);

    s32 Chara_Spawn(e_CharaId charaId, s32 spawnFlags, q19_12 posX, q19_12 posZ, q3_12 rotY, u32 stateStep);

    void Chara_ModelCharaIdClear(s_MapOverlayHeader mapHdr, s_SubCharacter* chara, s32 unused0, s32 unused1);

    void Chara_SpawnFlagsSet(s_MapOverlayHeader mapHdr, e_CharaId charaId, s32 spawnIdx, s32 spawnFlags);
}
