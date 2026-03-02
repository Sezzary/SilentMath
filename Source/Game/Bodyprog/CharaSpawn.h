#pragma once

namespace Silent::Game
{
    enum   e_CharacterId;
    struct s_LmHeader;
    struct s_FsImageDesc;
    struct s_MapOverlayHeader;
    struct s_SubCharacter;

    bool Chara_Load(s32 modelIdx, s8 charaId, GsCOORDINATE2* coords, s8 forceFree, s_LmHeader* lmHdr, s_FsImageDesc* tex);

    bool Chara_ProcessLoads(void);

    bool Chara_ProcessLoads(void);

    void func_80088D34(s32 idx);

    //s32 Chara_Spawn(e_CharacterId charaId, s32 arg1, q19_12 posX, q19_12 posZ, q3_12 rotY, u32 stateStep);

    void func_80088F94(s_MapOverlayHeader mapHdr, s_SubCharacter* chara, s32 unused1, s32 unused2);

    void func_80088FF4(s_MapOverlayHeader mapHdr, e_CharacterId charaId, s32 spawnIdx, s32 spawnFlags);

    bool func_8008F434(s32 arg0);
}
