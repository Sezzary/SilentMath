#pragma once

namespace Silent::Assets
{
    // =======
    // System
    // =======

    constexpr char KEY_SYS_FULLSCREEN_HINT_GENERIC[] = "sys_FullscreenHintGeneric";
    constexpr char KEY_SYS_FULLSCREEN_HINT_MAC[]     = "sys_FullscreenHintMac";
    constexpr char KEY_SYS_GAMEPAD_CONNECTED[]       = "sys_GamepadConnected";
    constexpr char KEY_SYS_GAMEPAD_DISCONNECTED[]    = "sys_GamepadDisconnected";

    // ======
    // Intro
    // ======

    constexpr char KEY_INTRO_WARNING[]       = "intro_Warning";
    constexpr char KEY_INTRO_FEAR_OF_BLOOD[] = "intro_FearOfBlood";

    // ======
    // Menus
    // ======

    constexpr char KEY_MAIN_MENU_START[]    = "mainMenu_Start";
    constexpr char KEY_MAIN_MENU_CONTINUE[] = "mainMenu_Continue";
    constexpr char KEY_MAIN_MENU_LOAD[]     = "mainMenu_Load";
    constexpr char KEY_MAIN_MENU_OPTION[]   = "mainMenu_Option";

    // @todo Create submenus to better organise new options.
    constexpr char KEY_OPTIONS_MENU_GRAPHICS[] = "optionsMenu_Graphics";
    constexpr char KEY_OPTIONS_MENU_GAMEPLAY[] = "optionsMenu_Gameplay";
    constexpr char KEY_OPTIONS_MENU_INPUT[] = "optionsMenu_Input";
    constexpr char KEY_OPTIONS_MENU_ENHANCEMENTS[] = "optionsMenu_Enhancements";
    constexpr char KEY_OPTIONS_MENU_SYSTEM[] = "optionsMenu_System";

    constexpr char KEY_OPTIONS_MENU_OPTIONS[]          = "optionsMenu_Options";
    constexpr char KEY_OPTIONS_MENU_EXIT[]             = "optionsMenu_Exit";
    constexpr char KEY_OPTIONS_MENU_BRIGHT_LEVEL[]     = "optionsMenu_BrightLevel";
    constexpr char KEY_OPTIONS_MENU_CONT_CONFIG[]      = "optionsMenu_ContConfig";
    constexpr char KEY_OPTIONS_MENU_VIBRATION[]        = "optionsMenu_Vibration";
    constexpr char KEY_OPTIONS_MENU_AUTO_LOAD[]        = "optionsMenu_AutoLoad";
    constexpr char KEY_OPTIONS_MENU_SOUND[]            = "optionsMenu_Sound";
    constexpr char KEY_OPTIONS_MENU_BGM_VOL[]          = "optionsMenu_BgmVol";
    constexpr char KEY_OPTIONS_MENU_SE_VOL[]           = "optionsMenu_SeVol";
    constexpr char KEY_OPTIONS_MENU_EXTRA_OPTIONS[]    = "optionsMenu_ExtraOptions";
    constexpr char KEY_OPTIONS_MENU_WEAPON_CONTROL[]   = "optionsMenu_WeaponControl";
    constexpr char KEY_OPTIONS_MENU_BLOOD_COLOR[]      = "optionsMenu_BloodColor";
    constexpr char KEY_OPTIONS_MENU_VIEW_CONTROL[]     = "optionsMenu_ViewControl";
    constexpr char KEY_OPTIONS_MENU_RETREAT_TURN[]     = "optionsMenu_RetreatTurn";
    constexpr char KEY_OPTIONS_MENU_WALK_RUN_CONTROL[] = "optionsMenu_WalkRunControl";
    constexpr char KEY_OPTIONS_MENU_CONTROL[]          = "optionsMenu_Control";
    constexpr char KEY_OPTIONS_MENU_AUTO_AIMING[]      = "optionsMenu_AutoAiming";
    constexpr char KEY_OPTIONS_MENU_VIEW_MODE[]        = "optionsMenu_ViewMode";
    constexpr char KEY_OPTIONS_MENU_BULLET_ADJUST[]    = "optionsMenu_BulletAdjust";
    constexpr char KEY_OPTIONS_MENU_ON[]               = "optionsMenu_On";
    constexpr char KEY_OPTIONS_MENU_OFF[]              = "optionsMenu_Off";
    constexpr char KEY_OPTIONS_MENU_STEREO[]           = "optionsMenu_Stereo";
    constexpr char KEY_OPTIONS_MENU_MONAURAL[]         = "optionsMenu_Monaural";
    constexpr char KEY_OPTIONS_MENU_PRESS[]            = "optionsMenu_Press";
    constexpr char KEY_OPTIONS_MENU_SWITCH[]           = "optionsMenu_Switch";
    constexpr char KEY_OPTIONS_MENU_NORMAL[]           = "optionsMenu_Normal";
    constexpr char KEY_OPTIONS_MENU_GREEN[]            = "optionsMenu_Green";
    constexpr char KEY_OPTIONS_MENU_VIOLET[]           = "optionsMenu_Violet";
    constexpr char KEY_OPTIONS_MENU_BLACK[]            = "optionsMenu_Black";
    constexpr char KEY_OPTIONS_MENU_REVERSE[]          = "optionsMenu_Reverse";
    constexpr char KEY_OPTIONS_MENU_SELF_VIEW[]        = "optionsMenu_SelfView";

    constexpr char KEY_BRIGHT_MENU_PROMPT[] = "brightMenu_Prompt";
    constexpr char KEY_BRIGHT_MENU_LEVEL[]  = "brightMenu_Level";

    constexpr char KEY_CONT_MENU_KEYBOARD_MOUSE[]   = "contMenu_KeyboardMouse";
    constexpr char KEY_CONT_MENU_CONTROLLER[]       = "contMenu_Controller";
    constexpr char KEY_CONT_MENU_GENERAL_ACTIONS[]  = "contMenu_GeneralActions";
    constexpr char KEY_CONT_MENU_MENU_ACTIONS[]     = "contMenu_MenuActions";
    constexpr char KEY_CONT_MENU_GAMEPLAY_ACTIONS[] = "contMenu_GameplayActions";
    constexpr char KEY_CONT_MENU_EXIT[]             = "contMenu_Exit";
    constexpr char KEY_CONT_MENU_TYPE_1[]           = "contMenu_Type1";
    constexpr char KEY_CONT_MENU_TYPE_2[]           = "contMenu_Type2";
    constexpr char KEY_CONT_MENU_TYPE_3[]           = "contMenu_Type3";
    constexpr char KEY_CONT_MENU_ENTER[]            = "contMenu_Enter";
    constexpr char KEY_CONT_MENU_CANCEL[]           = "contMenu_Cancel";
    constexpr char KEY_CONT_MENU_ACTION[]           = "contMenu_Action";
    constexpr char KEY_CONT_MENU_AIM[]              = "contMenu_Aim";
    constexpr char KEY_CONT_MENU_LIGHT[]            = "contMenu_Light";
    constexpr char KEY_CONT_MENU_RUN[]              = "contMenu_Run";
    constexpr char KEY_CONT_MENU_VIEW[]             = "contMenu_View";
    constexpr char KEY_CONT_MENU_STEP_L[]           = "contMenu_StepL";
    constexpr char KEY_CONT_MENU_STEP_R[]           = "contMenu_StepR";
    constexpr char KEY_CONT_MENU_PAUSE[]            = "contMenu_Pause";
    constexpr char KEY_CONT_MENU_ITEM[]             = "contMenu_Item";
    constexpr char KEY_CONT_MENU_MAP[]              = "contMenu_Map";
    constexpr char KEY_CONT_MENU_OPTION[]           = "contMenu_Option";
    constexpr char KEY_CONT_MENU_RESET[]            = "contMenu_Reset";
    constexpr char KEY_CONT_MENU_WAITING[]          = "contMenu_Waiting";

    constexpr char KEY_INV_MENU_EXIT[] = "invMenu_Exit";
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

    // =================
    // Paper Map Labels
    // =================

    constexpr char KEY_PAPER_MAP_SILENT_HILL[]          = "paperMap_SilentHill";
    constexpr char KEY_PAPER_MAP_OLD_SILENT_HILL[]      = "paperMap_OldSilentHill";
    constexpr char KEY_PAPER_MAP_TO_CENTRAL[]           = "paperMap_ToCentral";
    constexpr char KEY_PAPER_MAP_BRIDGE_CONTROL[]       = "paperMap_BridgeControl";
    constexpr char KEY_PAPER_MAP_MIDWICH_ST[]           = "paperMap_MidwichSt";
    constexpr char KEY_PAPER_MAP_BRADBURY_ST[]          = "paperMap_BradburySt";
    constexpr char KEY_PAPER_MAP_BLOCH_ST[]             = "paperMap_BlochSt";
    constexpr char KEY_PAPER_MAP_MATHESON_ST[]          = "paperMap_MathesonSt";
    constexpr char KEY_PAPER_MAP_LEVIN_ST[]             = "paperMap_LevinSt";
    constexpr char KEY_PAPER_MAP_FINNEY_ST[]            = "paperMap_FinneySt";
    constexpr char KEY_PAPER_MAP_BACHMAN_RD[]           = "paperMap_BachmanRd";
    constexpr char KEY_PAPER_MAP_ELLROY_ST[]            = "paperMap_EllroySt";
    constexpr char KEY_PAPER_MAP_BALKAN_CHURCH[]        = "paperMap_BalkanChurch";
    constexpr char KEY_PAPER_MAP_SCHOOL[]               = "paperMap_School";
    constexpr char KEY_PAPER_MAP_CAFE_5TO2[]            = "paperMap_Cafe5to2";
    constexpr char KEY_PAPER_MAP_QUEEN_BURGER[]         = "paperMap_QueenBurger";
    constexpr char KEY_PAPER_MAP_CHAINSAWS[]            = "paperMap_Chainsaws";
    constexpr char KEY_PAPER_MAP_CONVENIENCE_STORE_8[]  = "paperMap_ConvenienceStore8";
    constexpr char KEY_PAPER_MAP_GAS_STATION[]          = "paperMap_GasStation";
    constexpr char KEY_PAPER_MAP_FOOD_AND_LIQUOR[]      = "paperMap_FoodAndLiquor";
    constexpr char KEY_PAPER_MAP_ALGERNON[]             = "paperMap_Algernon";
    constexpr char KEY_PAPER_MAP_METROPOL[]             = "paperMap_Metropol";
    constexpr char KEY_PAPER_MAP_BLUE_BELLE[]           = "paperMap_BlueBelle";
    constexpr char KEY_PAPER_MAP_CRABS[]                = "paperMap_Crabs";
    constexpr char KEY_PAPER_MAP_HIGH_LIFE[]            = "paperMap_HighLife";
    constexpr char KEY_PAPER_MAP_FISH_N_FRIES[]         = "paperMap_FishNFries";
    constexpr char KEY_PAPER_MAP_PHARMACY[]             = "paperMap_Pharpacy";
    constexpr char KEY_PAPER_MAP_BOOK_STORE[]           = "paperMap_BookStore";
    constexpr char KEY_PAPER_MAP_JULIOS_AUTO_PARTS[]    = "paperMap_JuliosAutoParts";
    constexpr char KEY_PAPER_MAP_HORNED_MOON_DANCE[]    = "paperMap_HornedMoonDance";
    constexpr char KEY_PAPER_MAP_FRESH_ROASTED_COFFEE[] = "paperMap_FreshRoastedCoffee";
    constexpr char KEY_PAPER_MAP_POSTON_MARKET[]        = "paperMap_PostonMarket";
    constexpr char KEY_PAPER_MAP_ICE_CREAM_BAR[]        = "paperMap_IceCreamBar";
    constexpr char KEY_PAPER_MAP_ICHIRONS_DRUGS[]       = "paperMap_IchironsDrugs";
    constexpr char KEY_PAPER_MAP_GHOUL[]                = "paperMap_Ghoul";
    constexpr char KEY_PAPER_MAP_JIM_BEAM[]             = "paperMap_JimBeam";
    constexpr char KEY_PAPER_MAP_SHOE_REPAIR[]          = "paperMap_ShoeRapair";
    constexpr char KEY_PAPER_MAP_SOUVENIRS[]            = "paperMap_Souvenirs";
    constexpr char KEY_PAPER_MAP_ROGERS_PASTRY[]        = "paperMap_RogersPastry";
    constexpr char KEY_PAPER_MAP_LIGHTING_CO[]          = "paperMap_LightingCo";
    constexpr char KEY_PAPER_MAP_SUNDRIES_SHOP[]        = "paperMap_SundriesShop";
    constexpr char KEY_PAPER_MAP_MARKET[]               = "paperMap_Market";
    constexpr char KEY_PAPER_MAP_TOP_SALES[]            = "paperMap_TopSales";

    //constexpr char KEY_PAPER_MAP_SCHOOL[]      = "paperMap_School";
    constexpr char KEY_PAPER_MAP_HOSPITAL[]    = "paperMap_Hospital";
    constexpr char KEY_PAPER_MAP_BASEMENT[]    = "paperMap_Basement";
    constexpr char KEY_PAPER_MAP_FLOOR_1[]     = "paperMap_Floor1";
    constexpr char KEY_PAPER_MAP_FLOOR_2[]     = "paperMap_Floor2";
    constexpr char KEY_PAPER_MAP_FLOOR_3[]     = "paperMap_Floor3";
    constexpr char KEY_PAPER_MAP_ROOF[]        = "paperMap_Roof";
    constexpr char KEY_PAPER_MAP_LADDER[]      = "paperMap_Ladder";
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
