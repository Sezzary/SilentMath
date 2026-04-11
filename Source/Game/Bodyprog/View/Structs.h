#pragma once

#include "Game/Bodyprog/View/Enums.h"

namespace Silent::Game
{
    struct s_SubCharacter;

    constexpr int CAMERA_PATH_COLL_COUNT_MAX = 10;

    /** @brief 2D camera path area constraint on the XZ plane. */
    struct VC_LIMIT_AREA
    {
        q11_4 min_hx;
        q11_4 max_hx;
        q11_4 min_hz;
        q11_4 max_hz;
    };

    /** @brief Internal camera info. */
    struct VC_CAMERA_INTINFO
    {
        u32   mode;        /** Mode state step. */
        u8    mv_smooth;   /** `VC_CAM_MV_TYPE` */
        q3_12 ev_cam_rate; /** Camera elevation rate. */
    };

    /** @brief Camera look-at move parameters.
     * TODO: These don't seem to be angles like the names would suggest, but distances don't fit either.
     */
    struct VC_WATCH_MV_PARAM
    {
        q19_12 ang_accel_x;   /** Angle acceleration on X axis. */
        q19_12 ang_accel_y;   /** Angle acceleration on Y axis. */
        q3_12  max_ang_spd_x; /** Max angle speed on X axis. */
        q3_12  max_ang_spd_y; /** Max angle speed on Y axis. */
    };

    /** @brief Camera move parameters. */
    struct VC_CAM_MV_PARAM
    {
        q19_12 accel_xz;   /** Speed acceleration on XZ plane. */
        q19_12 accel_y;    /** Speed acceleration on Y axis. */
        q19_12 max_spd_xz; /** Max speed on XZ plane. */
        q19_12 max_spd_y;  /** Max speed on Y axis. */
    };

    /** @brief Camera path data.
     *
     * @note In SH2, the `.cam` files contain this struct, while in SH1 this is part of `s_MapOverlayHeader`.
     */
    struct VC_ROAD_DATA
    {
        VC_LIMIT_AREA     lim_sw;
        VC_LIMIT_AREA     lim_rd;
        VC_ROAD_FLAGS     flags          : 8; /** `VC_ROAD_FLAGS` | Camera path flags. */
        VC_AREA_SIZE_TYPE area_size_type : 2;
        VC_ROAD_TYPE      rd_type        : 3; /** Path type. */
        u32               mv_y_type      : 3; /** `VC_CAM_MV_TYPE` */
        q27_4             lim_rd_max_hy  : 8; /** In SH2, `max_hy` and `min_hy` are part of `VC_LIMIT_AREA`.In SH1, these are separate for some reason. */
        q27_4             lim_rd_min_hy  : 8;
        q27_4             ofs_watch_hy   : 8;
        u32               field_15       : 4; // TODO: Should be `_14`.
        s16               cam_mv_type    : 4; /** `VC_CAM_MV_TYPE` */
        q0_7              fix_ang_x;          /** @note Part of union in SH2 `VC_ROAD_DATA`. */
        q0_7              fix_ang_y;
    };

    /** @brief Rail camera parementers. */
    struct VC_THROUGH_DOOR_CAM_PARAM
    {
        u8      active_f;               /** `bool` | Active flag. */
        q19_12  timer;
        q3_12   rail_ang_y;             /** Rail Y angle. */
        VECTOR3 rail_sta_pos;           /** Rail start position. */
        q19_12  rail_sta_to_chara_dist; /** Distance from rail start position to locked-on character position. */
    };

    /** @brief Nearby camera path collision. */
    struct VC_NEAR_ROAD_DATA
    {
        VC_ROAD_DATA* road_p;              /** Path associated with the collision. */
        u8            rd_dir_type;         /** `VC_ROAD_DIR_TYPE` */
        u8            use_priority;        /** Usage priority in case of overlap. Higher values take precedence. */
        q19_12        chara2road_sum_dist; /** Character to path distance. */
        q19_12        chara2road_vec_x;    /** Character to path distance on X axis. */
        q19_12        chara2road_vec_z;    /** Character to path distance on Z axis. */
        VC_LIMIT_AREA rd;                  /** Camera path constraint on XZ plane. */
        VC_LIMIT_AREA sw;                  /** Switch constraint on XZ plane. */
    };

    /** @brief Camera workspace. */
    struct VC_WORK
    {
        u8                        view_cam_active_f;            /** `bool` */
        VC_ROAD_DATA*             vc_road_ary_list;             /** Camera paths for the active map. */
        u32                       flags;                        /** `VC_FLAGS` */
        u8                        through_door_activate_init_f; /** `bool` */
        VC_THROUGH_DOOR_CAM_PARAM through_door;                 /** Active rail camera data? */
        q3_12                     scr_half_ang_wy;
        q3_12                     scr_half_ang_wx;
        s16                       geom_screen_dist;             /** Related to `GsSetProjection`/`g_GameSys.gs_y_res_58A`. */
        VC_CAM_MV_PARAM           user_cam_mv_prm;              /** Look parameters? */
        VECTOR3                   cam_tgt_pos;                  /** Target camera position. */
        VECTOR3                   cam_pos;                      /** Q19.12 | Camera position. */
        q3_12                     cam_mv_ang_y;                 /** Angular velocity on the Y axis. */
        VECTOR3                   cam_velo;                     /** Q19.12 | Camera velocity. */
        q19_12                    old_cam_excl_area_r;          /** Previous exclusion area radius. */
        VC_WATCH_MV_PARAM         user_watch_mv_prm;
        VECTOR3                   watch_tgt_pos;                /** Q19.12 | Target look-at position. */
        q19_12                    watch_tgt_max_y;              /** Max look-at Y offset. */
        q3_12                     watch_tgt_ang_z;              /** Target look-at Z angle. */
        SVECTOR                   cam_mat_ang;                  /** Matrix rotation. */
        MATRIX                    cam_mat;                      /** Matrix. */
        SVECTOR                   ofs_cam_ang;                  /** Offset rotation. */
        SVECTOR                   ofs_cam_ang_spd;              /** Offset rotational speed. */
        SVECTOR                   base_cam_ang;                 /** Base rotation. */
        s8                        unk[8];                       // TODO: Possibly unused or debug data?
        u8                        updateLookAtPoint;            /** `bool` */
        MATRIX                    lookAtMat;
        u8                        updateLookAtMat;              /** `bool` */
        q3_12                     cam_chara2ideal_ang_y;
        VECTOR3                   cam_tgt_velo;                 /** Target velocity. */
        q3_12                     cam_tgt_mv_ang_y;             /** Target Y angles. */
        q19_12                    cam_tgt_spd;                               /** Target speed. */
        VECTOR3                   chara_pos;                                 /** Locked-on character position. */
        q19_12                    chara_bottom_y;                            /** Locked-on character bottom height. */
        q19_12                    chara_top_y;                               /** Locked-on character top height. */
        q19_12                    chara_center_y;                            /** Locked-on character center height. */
        q19_12                    chara_grnd_y;                              /** Locked-on character height from the ground? */
        VECTOR3                   chara_head_pos;                            /** Q19.12 | Locked-on character head position. */
        q19_12                    chara_mv_spd;                              /** Locked-on character movement speed. */
        q3_12                     chara_mv_ang_y;                            /** Locked-on character heading angle. */
        q3_12                     chara_ang_spd_y;                           /** Locked-on character heading angle angular speed. */
        q3_12                     chara_eye_ang_y;                           /** Locked-on character look heading angle. */
        q3_12                     chara_eye_ang_wy;                          /** Unused. */
        q19_12                    chara_watch_xz_r;                          /** Locked-on character radius on the XZ plane. */
        VC_NEAR_ROAD_DATA         near_road_ary[CAMERA_PATH_COLL_COUNT_MAX]; /** Nearby camera path collisions. */
        q19_12                    near_road_suu;                             /** Count of valid `near_road_ary` entries. */
        VC_NEAR_ROAD_DATA         cur_near_road;                             /** Closest camera path? */
        s_SubCharacter*           nearest_enemy;                             /** Closest enemy character. */
        q19_12                    nearest_enemy_xz_dist;                     /** Distance to the closest enemy character on the XZ plane. */
        q19_12                    watch_pos_y;                               /** Guessed name. Camera look-at Y position base? Combined with another value later. */
    };

    /** @brief Camera view matrix. */
    struct VbRVIEW
    {
        VECTOR3        vp;
        VECTOR3        vr;
        s32            rz;
        GsCOORDINATE2* super;
    };

    /** @brief Camera view renderer workspace. */
    struct VW_VIEW_WORK
    {
        VbRVIEW       rview;
        GsCOORDINATE2 vwcoord;  /** Look-at point coord. */
        VECTOR3       worldpos; /** Q19.12 | Camera world position. */
        SVECTOR       worldang; /** Q3.12 | Camera world rotation. */
    };

    /** @brief Camera view cull data. */
    struct s_CameraCullData
    {
        MATRIX  modelMat;
        SVECTOR field_20[8];
        VECTOR3 field_60[8];
        s32     field_C0;
        SVECTOR field_C4[10];
        s32     field_114;
        DVECTOR field_118[24];
        s32     field_178;
    };

    /** @brief 3x3 screen region occupancy flags. */
    struct s_CameraScreenRegionFlags
    {
        u8 flags[3][3];
    };
}
