#pragma once

namespace Silent::Assets
{
    // =======
    // System
    // =======

    constexpr char KEY_SYS_GAMEPAD_CONNECTED[]    = "sys_GamepadConnected";
    constexpr char KEY_SYS_GAMEPAD_DISCONNECTED[] = "sys_GamepadDisconnected";

    // ========================
    // Splash Screen and Intro
    // ========================

    constexpr char KEY_SPLASH_SCREEN_WARNING[] = "splashScreen_Warning";
    constexpr char KEY_INTRO_FEAR_OF_BLOOD[]   = "intro_FearOfBlood";

    // ======
    // Menus
    // ======

    constexpr char KEY_MAIN_OPTIONS_MENU_OPTIONS[]      = "mainOptMenu_Options";
    constexpr char KEY_MAIN_OPTIONS_MENU_EXIT[]         = "mainOptMenu_Exit";
    constexpr char KEY_MAIN_OPTIONS_MENU_BRIGHT_LEVEL[] = "mainOptMenu_BrightLevel";
    constexpr char KEY_MAIN_OPTIONS_MENU_CONT_CONFIG[]  = "mainOptMenu_ContConfig";
    constexpr char KEY_MAIN_OPTIONS_MENU_SCREEN_POS[]   = "mainOptMenu_ScreenPos";
    constexpr char KEY_MAIN_OPTIONS_MENU_VIBRATION[]    = "mainOptMenu_Vibration";
    constexpr char KEY_MAIN_OPTIONS_MENU_AUTO_LOAD[]    = "mainOptMenu_AutoLoad";
    constexpr char KEY_MAIN_OPTIONS_MENU_SOUND[]        = "mainOptMenu_Sound";
    constexpr char KEY_MAIN_OPTIONS_MENU_BGM_VOL[]      = "mainOptMenu_BgmVol";
    constexpr char KEY_MAIN_OPTIONS_MENU_SE_VOL[]       = "mainOptMenu_SeVol";
    constexpr char KEY_MAIN_OPTIONS_MENU_ON[]           = "mainOptMenu_On";
    constexpr char KEY_MAIN_OPTIONS_MENU_OFF[]          = "mainOptMenu_Off";
    constexpr char KEY_MAIN_OPTIONS_MENU_STEREO[]       = "mainOptMenu_Stereo";
    constexpr char KEY_MAIN_OPTIONS_MENU_MONAURAL[]     = "mainOptMenu_Monaural";

    constexpr char KEY_EXTRA_OPTIONS_MENU_EXTRA[]          = "extraOptionsMenu_Extra";
    constexpr char KEY_EXTRA_OPTIONS_MENU_WEAPON_CONTROL[] = "extraOptionsMenu_WeaponControl";
    constexpr char KEY_EXTRA_OPTIONS_MENU_BLOOD_COLOR[]    = "extraOptionsMenu_BloodColor";
    constexpr char KEY_EXTRA_OPTIONS_MENU_VIEW_CONTROL[]   = "extraOptionsMenu_ViewControl";
    constexpr char KEY_EXTRA_OPTIONS_MENU_RETREAT_TURN[]   = "extraOptionsMenu_RetreatTurn";
    constexpr char KEY_EXTRA_OPTIONS_MENU_WALK_RUN[]       = "extraOptionsMenu_WalkRun";
    constexpr char KEY_EXTRA_OPTIONS_MENU_CONTROL[]        = "extraOptionsMenu_Control";
    constexpr char KEY_EXTRA_OPTIONS_MENU_AUTO_AIMING[]    = "extraOptionsMenu_AutoAiming";
    constexpr char KEY_EXTRA_OPTIONS_MENU_VIEW_MODE[]      = "extraOptionsMenu_ViewMode";
    constexpr char KEY_EXTRA_OPTIONS_MENU_BULLET_ADJUST[]  = "extraOptionsMenu_BulletAdjust";

    constexpr char KEY_INV_MENU_EXIT[] = "invMenuExit";

    // @todo

    // ===============
    // Save Locations
    // ===============

    constexpr char KEY_SAVE_LOC_ANYWHERE[]       = "saveLoc_Anywhere";
    constexpr char KEY_SAVE_LOC_CAFE[]           = "saveLoc_Cafe";
    constexpr char KEY_SAVE_LOC_BUS[]            = "saveLoc_Bus";
    constexpr char KEY_SAVE_LOC_STORE[]          = "saveLoc_Store";
    constexpr char KEY_SAVE_LOC_INFIRMARY[]      = "saveLoc_Infirmary";
    constexpr char KEY_SAVE_LOC_DOGHOUSE[]       = "saveLoc_Doghouse";
    constexpr char KEY_SAVE_LOC_GORDON[]         = "saveLoc_Gordon";
    constexpr char KEY_SAVE_LOC_CHURCH[]         = "saveLoc_Church";
    constexpr char KEY_SAVE_LOC_GARAGE[]         = "saveLoc_Garage";
    constexpr char KEY_SAVE_LOC_POLICE[]         = "saveLoc_Police";
    constexpr char KEY_SAVE_LOC_RECEPTION[]      = "saveLoc_Reception";
    constexpr char KEY_SAVE_LOC_ROOM_302[]       = "saveLoc_Room302";
    constexpr char KEY_SAVE_LOC_DIRECTORS[]      = "saveLoc_Directors";
    constexpr char KEY_SAVE_LOC_JEWELLERY_SHOP[] = "saveLoc_JewelleryShop";
    constexpr char KEY_SAVE_LOC_POOL_HALL[]      = "saveLoc_PoolHall";
    constexpr char KEY_SAVE_LOC_ANTIQUE_SHOP[]   = "saveLoc_AntiqueShop";
    constexpr char KEY_SAVE_LOC_THEME_PARK[]     = "saveLoc_ThemePark";
    constexpr char KEY_SAVE_LOC_BOAT[]           = "saveLoc_Boat";
    constexpr char KEY_SAVE_LOC_BRIDGE[]         = "saveLoc_Bridge";
    constexpr char KEY_SAVE_LOC_MOTEL[]          = "saveLoc_Motel";
    constexpr char KEY_SAVE_LOC_LIGHTHOUSE[]     = "saveLoc_Lighthouse";
    constexpr char KEY_SAVE_LOC_SEWER[]          = "saveLoc_Sewer";
    constexpr char KEY_SAVE_LOC_NOWHERE[]        = "saveLoc_Nowhere";
    constexpr char KEY_SAVE_LOC_CHILDS_ROOM[]    = "saveLoc_ChildsRoom";
    constexpr char KEY_SAVE_LOC_NEXT_FEAR[]      = "saveLoc_NextFear";

    // ==================
    // Item Descriptions
    // ==================

    constexpr char KEY_ITEM_DESC_KITCHEN_KNIFE[] = "itemDescKitchenKnife";

    // @todo

    // ==========
    // Game Over
    // ==========

    constexpr char KEY_GAME_OVER_HEADING[]      = "gameOver_Heading";
    constexpr char KEY_GAME_OVER_TIPS_HEADING[] = "gameOver_TipsHeading";
    constexpr char KEY_GAME_OVER_TIP_1[]        = "gameOver_Tip1";
    constexpr char KEY_GAME_OVER_TIP_2[]        = "gameOver_Tip2";
    constexpr char KEY_GAME_OVER_TIP_3[]        = "gameOver_Tip3";
    constexpr char KEY_GAME_OVER_TIP_4[]        = "gameOver_Tip4";
    constexpr char KEY_GAME_OVER_TIP_5[]        = "gameOver_Tip5";
    constexpr char KEY_GAME_OVER_TIP_6[]        = "gameOver_Tip6";
    constexpr char KEY_GAME_OVER_TIP_7[]        = "gameOver_Tip7";
    constexpr char KEY_GAME_OVER_TIP_8[]        = "gameOver_Tip8";
    constexpr char KEY_GAME_OVER_TIP_9[]        = "gameOver_Tip9";
    constexpr char KEY_GAME_OVER_TIP_10[]       = "gameOver_Tip10";
    constexpr char KEY_GAME_OVER_TIP_11[]       = "gameOver_Tip11";
    constexpr char KEY_GAME_OVER_TIP_12[]       = "gameOver_Tip12";
    constexpr char KEY_GAME_OVER_TIP_13[]       = "gameOver_Tip13";
    constexpr char KEY_GAME_OVER_TIP_14[]       = "gameOver_Tip14";
    constexpr char KEY_GAME_OVER_TIP_15[]       = "gameOver_Tip15";

    // ====================
    // Common Map Messages
    // ====================

    constexpr char KEY_MAP_MSG_YES[]                  = "mapMsg_Yes";
    constexpr char KEY_MAP_MSG_NO[]                   = "mapMsg_No";
    constexpr char KEY_MAP_MSG_SOMEDAY[]              = "mapMsg_Someday";
    constexpr char KEY_MAP_MSG_NO_MAP[]               = "mapMsg_NoMap";
    constexpr char KEY_MAP_MSG_TOO_DARK_FOR_MAP[]     = "mapMsg_TooDarkForMap";
    constexpr char KEY_MAP_MSG_TAKE_FIRST_AID_KIT[]   = "mapMsg_TakeFirstAidKit";
    constexpr char KEY_MAP_MSG_TAKE_HEALTH_DRINK[]    = "mapMsg_TakeHealthDrink";
    constexpr char KEY_MAP_MSG_TAKE_AMPOULE[]         = "mapMsg_TakeAmpoule";
    constexpr char KEY_MAP_MSG_TAKE_HANDGUN_BULLETS[] = "mapMsg_TakeHandgunBullets";
    constexpr char KEY_MAP_MSG_TAKE_RIFLE_SHELLS[]    = "mapMsg_TakeRifleShells";
    constexpr char KEY_MAP_MSG_TAKE_SHOTGUN_SHELLS[]  = "mapMsg_TakeShotgunShells";
    constexpr char KEY_MAP_MSG_DOOR_JAMMED[]          = "mapMsg_DoorJammed";
    constexpr char KEY_MAP_MSG_DOOR_LOCKED[]          = "mapMsg_DoorLocked";
    constexpr char KEY_MAP_MSG_DOOR_UNLOCKED[]        = "mapMsg_DoorUnlocked";
    constexpr char KEY_MAP_MSG_NOW_MAKING[]           = "mapMsg_NowMaking";

    // =========================
    // Map 0, Stage 00 Messages
    // =========================

    constexpr char KEY_M0S00_MSG_0[] = "m0s00Msg0";

    // @todo
}
