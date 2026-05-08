#pragma once

namespace Silent::Assets
{
    // =======
    // System
    // =======

    constexpr char KEY_SYS_FULLSCREEN_HINT_GENERIC[] = "Sys_FullscreenHintGeneric";
    constexpr char KEY_SYS_FULLSCREEN_HINT_MAC[]     = "Sys_FullscreenHintMac";
    constexpr char KEY_SYS_GAMEPAD_CONNECTED[]       = "Sys_GamepadConnected";
    constexpr char KEY_SYS_GAMEPAD_DISCONNECTED[]    = "Sys_GamepadDisconnected";
    constexpr char KEY_SYS_GAMEPAD_BATTERY_IS_LOW[]  = "Sys_GamepadBatteryIsLow";

    // ======
    // Intro
    // ======

    constexpr char KEY_INTRO_WARNING[]       = "Intro_Warning";
    constexpr char KEY_INTRO_FEAR_OF_BLOOD[] = "Intro_FearOfBlood";

    // ======
    // Menus
    // ======

    constexpr char KEY_MAIN_MENU_OPENING_QUOTE[] = "MainMenu_OpeningQuote";
    constexpr char KEY_MAIN_MENU_CLOSING_QUOTE[] = "MainMenu_ClosingQuote";
    constexpr char KEY_MAIN_MENU_START[]         = "MainMenu_Start";
    constexpr char KEY_MAIN_MENU_CONTINUE[]      = "MainMenu_Continue";
    constexpr char KEY_MAIN_MENU_LOAD[]          = "MainMenu_Load";
    constexpr char KEY_MAIN_MENU_OPTION[]        = "MainMenu_Option";
    constexpr char KEY_MAIN_MENU_QUIT[]          = "MainMenu_Quit";
    constexpr char KEY_MAIN_MENU_EASY[]          = "MainMenu_Easy";
    constexpr char KEY_MAIN_MENU_NORMAL[]        = "MainMenu_Normal";
    constexpr char KEY_MAIN_MENU_HARD[]          = "MainMenu_Hard";

    constexpr char KEY_OPTIONS_MENU_HEADING[]          = "OptionsMenu_Heading";
    constexpr char KEY_OPTIONS_MENU_EXTRA_OPTIONS[]    = "OptionsMenu_ExtraOptions";
    constexpr char KEY_OPTIONS_MENU_GRAPHICS[]         = "OptionsMenu_Graphics";
    constexpr char KEY_OPTIONS_MENU_GAMEPLAY[]         = "OptionsMenu_Gameplay";
    constexpr char KEY_OPTIONS_MENU_INPUT[]            = "OptionsMenu_Input";
    constexpr char KEY_OPTIONS_MENU_ENHANCEMENTS[]     = "OptionsMenu_Enhancements";
    constexpr char KEY_OPTIONS_MENU_SYSTEM[]           = "OptionsMenu_System";

    constexpr char KEY_OPTIONS_MENU_EXIT[]             = "OptionsMenu_Exit";
    constexpr char KEY_OPTIONS_MENU_BRIGHT_LEVEL[]     = "OptionsMenu_BrightLevel";
    constexpr char KEY_OPTIONS_MENU_CONT_CONFIG[]      = "OptionsMenu_ContConfig";
    constexpr char KEY_OPTIONS_MENU_VIBRATION[]        = "OptionsMenu_Vibration";
    constexpr char KEY_OPTIONS_MENU_AUTO_LOAD[]        = "OptionsMenu_AutoLoad";
    constexpr char KEY_OPTIONS_MENU_SOUND[]            = "OptionsMenu_Sound";
    constexpr char KEY_OPTIONS_MENU_BGM_VOL[]          = "OptionsMenu_BgmVol";
    constexpr char KEY_OPTIONS_MENU_SE_VOL[]           = "OptionsMenu_SeVol";
    constexpr char KEY_OPTIONS_MENU_LANGUAGE[]         = "OptionsMenu_Language";
    
    constexpr char KEY_OPTIONS_MENU_WEAPON_CONTROL[]   = "OptionsMenu_WeaponControl";
    constexpr char KEY_OPTIONS_MENU_BLOOD_COLOR[]      = "OptionsMenu_BloodColor";
    constexpr char KEY_OPTIONS_MENU_VIEW_CONTROL[]     = "OptionsMenu_ViewControl";
    constexpr char KEY_OPTIONS_MENU_RETREAT_TURN[]     = "OptionsMenu_RetreatTurn";
    constexpr char KEY_OPTIONS_MENU_WALK_RUN_CONTROL[] = "OptionsMenu_WalkRunControl";
    constexpr char KEY_OPTIONS_MENU_CONTROL[]          = "OptionsMenu_Control";
    constexpr char KEY_OPTIONS_MENU_AUTO_AIMING[]      = "OptionsMenu_AutoAiming";
    constexpr char KEY_OPTIONS_MENU_VIEW_MODE[]        = "OptionsMenu_ViewMode";
    constexpr char KEY_OPTIONS_MENU_BULLET_ADJUST[]    = "OptionsMenu_BulletAdjust";

    constexpr char KEY_OPTIONS_MENU_ON[]               = "OptionsMenu_On";
    constexpr char KEY_OPTIONS_MENU_OFF[]              = "OptionsMenu_Off";
    constexpr char KEY_OPTIONS_MENU_STEREO[]           = "OptionsMenu_Stereo";
    constexpr char KEY_OPTIONS_MENU_MONAURAL[]         = "OptionsMenu_Monaural";
    constexpr char KEY_OPTIONS_MENU_PRESS[]            = "OptionsMenu_Press";
    constexpr char KEY_OPTIONS_MENU_SWITCH[]           = "OptionsMenu_Switch";
    constexpr char KEY_OPTIONS_MENU_NORMAL[]           = "OptionsMenu_Normal";
    constexpr char KEY_OPTIONS_MENU_GREEN[]            = "OptionsMenu_Green";
    constexpr char KEY_OPTIONS_MENU_VIOLET[]           = "OptionsMenu_Violet";
    constexpr char KEY_OPTIONS_MENU_BLACK[]            = "OptionsMenu_Black";
    constexpr char KEY_OPTIONS_MENU_REVERSE[]          = "OptionsMenu_Reverse";
    constexpr char KEY_OPTIONS_MENU_SELF_VIEW[]        = "OptionsMenu_SelfView";

    constexpr char KEY_BRIGHT_MENU_PROMPT[] = "BrightMenu_Prompt";
    constexpr char KEY_BRIGHT_MENU_LEVEL[]  = "BrightMenu_Level";

    constexpr char KEY_CONT_MENU_KEYBOARD_MOUSE[]   = "ContMenu_KeyboardMouse";
    constexpr char KEY_CONT_MENU_CONTROLLER[]       = "ContMenu_Controller";
    constexpr char KEY_CONT_MENU_GENERAL_ACTIONS[]  = "ContMenu_GeneralActions";
    constexpr char KEY_CONT_MENU_MENU_ACTIONS[]     = "ContMenu_MenuActions";
    constexpr char KEY_CONT_MENU_GAMEPLAY_ACTIONS[] = "ContMenu_GameplayActions";
    constexpr char KEY_CONT_MENU_EXIT[]             = "ContMenu_Exit";
    constexpr char KEY_CONT_MENU_TYPE_1[]           = "ContMenu_Type1";
    constexpr char KEY_CONT_MENU_TYPE_2[]           = "ContMenu_Type2";
    constexpr char KEY_CONT_MENU_TYPE_3[]           = "ContMenu_Type3";
    constexpr char KEY_CONT_MENU_ENTER[]            = "ContMenu_Enter";
    constexpr char KEY_CONT_MENU_CANCEL[]           = "ContMenu_Cancel";
    constexpr char KEY_CONT_MENU_ACTION[]           = "ContMenu_Action";
    constexpr char KEY_CONT_MENU_AIM[]              = "ContMenu_Aim";
    constexpr char KEY_CONT_MENU_LIGHT[]            = "ContMenu_Light";
    constexpr char KEY_CONT_MENU_RUN[]              = "ContMenu_Run";
    constexpr char KEY_CONT_MENU_VIEW[]             = "ContMenu_View";
    constexpr char KEY_CONT_MENU_STEP_L[]           = "ContMenu_StepL";
    constexpr char KEY_CONT_MENU_STEP_R[]           = "ContMenu_StepR";
    constexpr char KEY_CONT_MENU_PAUSE[]            = "ContMenu_Pause";
    constexpr char KEY_CONT_MENU_ITEM[]             = "ContMenu_Item";
    constexpr char KEY_CONT_MENU_MAP[]              = "ContMenu_Map";
    constexpr char KEY_CONT_MENU_OPTION[]           = "ContMenu_Option";
    constexpr char KEY_CONT_MENU_RESET[]            = "ContMenu_Reset";
    constexpr char KEY_CONT_MENU_WAITING[]          = "ContMenu_Waiting";

    // @todo Add PAUSE string

    constexpr char KEY_INV_MENU_EXIT[] = "invMenu_Exit";
    // @todo

    // ===============
    // Save Locations
    // ===============

    constexpr char KEY_SAVE_LOC_ANYWHERE[]       = "SaveLoc_Anywhere";
    constexpr char KEY_SAVE_LOC_CAFE[]           = "SaveLoc_Cafe";
    constexpr char KEY_SAVE_LOC_BUS[]            = "SaveLoc_Bus";
    constexpr char KEY_SAVE_LOC_STORE[]          = "SaveLoc_Store";
    constexpr char KEY_SAVE_LOC_INFIRMARY[]      = "SaveLoc_Infirmary";
    constexpr char KEY_SAVE_LOC_DOGHOUSE[]       = "SaveLoc_Doghouse";
    constexpr char KEY_SAVE_LOC_GORDON[]         = "SaveLoc_Gordon";
    constexpr char KEY_SAVE_LOC_CHURCH[]         = "SaveLoc_Church";
    constexpr char KEY_SAVE_LOC_GARAGE[]         = "SaveLoc_Garage";
    constexpr char KEY_SAVE_LOC_POLICE[]         = "SaveLoc_Police";
    constexpr char KEY_SAVE_LOC_RECEPTION[]      = "SaveLoc_Reception";
    constexpr char KEY_SAVE_LOC_ROOM_302[]       = "SaveLoc_Room302";
    constexpr char KEY_SAVE_LOC_DIRECTORS[]      = "SaveLoc_Directors";
    constexpr char KEY_SAVE_LOC_JEWELLERY_SHOP[] = "SaveLoc_JewelleryShop";
    constexpr char KEY_SAVE_LOC_POOL_HALL[]      = "SaveLoc_PoolHall";
    constexpr char KEY_SAVE_LOC_ANTIQUE_SHOP[]   = "SaveLoc_AntiqueShop";
    constexpr char KEY_SAVE_LOC_THEME_PARK[]     = "SaveLoc_ThemePark";
    constexpr char KEY_SAVE_LOC_BOAT[]           = "SaveLoc_Boat";
    constexpr char KEY_SAVE_LOC_BRIDGE[]         = "SaveLoc_Bridge";
    constexpr char KEY_SAVE_LOC_MOTEL[]          = "SaveLoc_Motel";
    constexpr char KEY_SAVE_LOC_LIGHTHOUSE[]     = "SaveLoc_Lighthouse";
    constexpr char KEY_SAVE_LOC_SEWER[]          = "SaveLoc_Sewer";
    constexpr char KEY_SAVE_LOC_NOWHERE[]        = "SaveLoc_Nowhere";
    constexpr char KEY_SAVE_LOC_CHILDS_ROOM[]    = "SaveLoc_ChildsRoom";
    constexpr char KEY_SAVE_LOC_NEXT_FEAR[]      = "SaveLoc_NextFear";

    // =================
    // Paper Map Labels
    // =================

    constexpr char KEY_PAPER_MAP_OSH_SILENT_HILL[]          = "PaperMap_OshSilentHill";
    constexpr char KEY_PAPER_MAP_OSH_TO_CENTRAL[]           = "PaperMap_OshToCentral";
    constexpr char KEY_PAPER_MAP_OSH_BACHMAN_RD[]           = "PaperMap_OshBachmanRd";
    constexpr char KEY_PAPER_MAP_OSH_BLOCH_ST[]             = "PaperMap_OshBlochSt";
    constexpr char KEY_PAPER_MAP_OSH_BRADBURY_ST[]          = "PaperMap_OshBradburySt";
    constexpr char KEY_PAPER_MAP_OSH_ELLROY_ST[]            = "PaperMap_OshEllroySt";
    constexpr char KEY_PAPER_MAP_OSH_FINNEY_ST[]            = "PaperMap_OshFinneySt";
    constexpr char KEY_PAPER_MAP_OSH_LEVIN_ST[]             = "PaperMap_OshLevinSt";
    constexpr char KEY_PAPER_MAP_OSH_MATHESON_ST[]          = "PaperMap_OshMathesonSt";
    constexpr char KEY_PAPER_MAP_OSH_MIDWICH_ST[]           = "PaperMap_OshMidwichSt";
    constexpr char KEY_PAPER_MAP_OSH_ALGERNON[]             = "PaperMap_OshAlgernon";
    constexpr char KEY_PAPER_MAP_OSH_BALKAN_CHURCH[]        = "PaperMap_OshBalkanChurch";
    constexpr char KEY_PAPER_MAP_OSH_BLUE_BELLE[]           = "PaperMap_OshBlueBelle";
    constexpr char KEY_PAPER_MAP_OSH_BOOK_STORE[]           = "PaperMap_OshBookStore";
    constexpr char KEY_PAPER_MAP_OSH_BRIDGE_CONTROL[]       = "PaperMap_OshBridgeControl";
    constexpr char KEY_PAPER_MAP_OSH_CAFE_5TO2[]            = "PaperMap_OshCafe5to2";
    constexpr char KEY_PAPER_MAP_OSH_CHAINSAWS[]            = "PaperMap_OshChainsaws";
    constexpr char KEY_PAPER_MAP_OSH_CONV_STORE_8[]         = "PaperMap_OshConvStore8";
    constexpr char KEY_PAPER_MAP_OSH_CRABS[]                = "PaperMap_OshCrabs";
    constexpr char KEY_PAPER_MAP_OSH_FISH_N_FRIES[]         = "PaperMap_OshFishNFries";
    constexpr char KEY_PAPER_MAP_OSH_FOOD_AND_LIQUOR[]      = "PaperMap_OshFoodAndLiquor";
    constexpr char KEY_PAPER_MAP_OSH_FRESH_ROASTED_COFFEE[] = "PaperMap_OshFreshRoastedCoffee";
    constexpr char KEY_PAPER_MAP_OSH_GAS_STATION[]          = "PaperMap_OshGasStation";
    constexpr char KEY_PAPER_MAP_OSH_GHOUL[]                = "PaperMap_OshGhoul";
    constexpr char KEY_PAPER_MAP_OSH_HIGH_LIFE[]            = "PaperMap_OshHighLife";
    constexpr char KEY_PAPER_MAP_OSH_HORNED_MOON_DANCE[]    = "PaperMap_OshHornedMoonDance";
    constexpr char KEY_PAPER_MAP_OSH_ICE_CREAM_BAR[]        = "PaperMap_OshIceCreamBar";
    constexpr char KEY_PAPER_MAP_OSH_ICHIRONS_DRUGS[]       = "PaperMap_OshIchironsDrugs";
    constexpr char KEY_PAPER_MAP_OSH_JIM_BEAM[]             = "PaperMap_OshJimBeam";
    constexpr char KEY_PAPER_MAP_OSH_JULIOS_AUTO_PARTS[]    = "PaperMap_OshJuliosAutoParts";
    constexpr char KEY_PAPER_MAP_OSH_LIGHTING_CO[]          = "PaperMap_OshLightingCo";
    constexpr char KEY_PAPER_MAP_OSH_MARKET[]               = "PaperMap_OshMarket";
    constexpr char KEY_PAPER_MAP_OSH_METROPOL[]             = "PaperMap_OshMetropol";
    constexpr char KEY_PAPER_MAP_OSH_PHARMACY[]             = "PaperMap_OshPharmacy";
    constexpr char KEY_PAPER_MAP_OSH_POSTON_MARKET[]        = "PaperMap_OshPostonMarket";
    constexpr char KEY_PAPER_MAP_OSH_QUEEN_BURGER[]         = "PaperMap_OshQueenBurger";
    constexpr char KEY_PAPER_MAP_OSH_ROGERS_PASTRY[]        = "PaperMap_OshRogersPastry";
    constexpr char KEY_PAPER_MAP_OSH_SCHOOL[]               = "PaperMap_OshSchool";
    constexpr char KEY_PAPER_MAP_OSH_SHOE_REPAIR[]          = "PaperMap_OshShoeRepair";
    constexpr char KEY_PAPER_MAP_OSH_SOUVENIRS[]            = "PaperMap_OshSouvenirs";
    constexpr char KEY_PAPER_MAP_OSH_SUNDRIES_SHOP[]        = "PaperMap_OshSundriesShop";
    constexpr char KEY_PAPER_MAP_OSH_TOP_SALES[]            = "PaperMap_OshTopSales";

    // @todo Central

    constexpr char KEY_PAPER_MAP_RESORT_HEADING[]            = "PaperMap_ResortHeading";
    constexpr char KEY_PAPER_MAP_RESORT_TO_LAKE_SIDE[]       = "PaperMap_ResortToLakeSide";
    constexpr char KEY_PAPER_MAP_RESORT_TO_OLD_SILENT_HILL[] = "PaperMap_ResortToOldSilentHill";
    constexpr char KEY_PAPER_MAP_RESORT_LAKE_SIDE[]          = "PaperMap_ResortLakeSide";
    constexpr char KEY_PAPER_MAP_RESORT_SOUTH_PARK[]         = "PaperMap_ResortSouthPark";
    constexpr char KEY_PAPER_MAP_RESORT_CRAIG_ST[]           = "PaperMap_ResortCraigSt";
    constexpr char KEY_PAPER_MAP_RESORT_BACHMAN_RD[]         = "PaperMap_ResortBachmanRd";
    constexpr char KEY_PAPER_MAP_RESORT_SANDFORD_ST[]        = "PaperMap_ResortSandfordSt";
    constexpr char KEY_PAPER_MAP_RESORT_W_SANDFORD_ST[]      = "PaperMap_ResortWSandfordSt";
    constexpr char KEY_PAPER_MAP_RESORT_WEAVER_ST[]          = "PaperMap_ResortWeaverSt";
    constexpr char KEY_PAPER_MAP_RESORT_AEC_BURGER[]         = "PaperMap_ResortAecBurger";
    constexpr char KEY_PAPER_MAP_RESORT_ANNIES_BAR[]         = "PaperMap_ResortAnniesBar";
    constexpr char KEY_PAPER_MAP_RESORT_EAST_GARAGE[]        = "PaperMap_ResortEastGarage";
    constexpr char KEY_PAPER_MAP_RESORT_HAERBEY_INN[]        = "PaperMap_ResortHaerbeyInn";
    constexpr char KEY_PAPER_MAP_RESORT_INDIAN_RUNNER[]      = "PaperMap_ResortIndianRunner";
    constexpr char KEY_PAPER_MAP_RESORT_LIGHT_HOUSE[]        = "PaperMap_ResortLightHouse";
    constexpr char KEY_PAPER_MAP_RESORT_MOTEL[]              = "PaperMap_ResortMotel";
    constexpr char KEY_PAPER_MAP_RESORT_WEST_GARAGE[]        = "PaperMap_ResortWestGarage";

    constexpr char KEY_PAPER_MAP_SCHOOL_HEADING[]         = "PaperMap_SchoolHeading";
    constexpr char KEY_PAPER_MAP_SCHOOL_FLOOR_BASEMENT[]  = "PaperMap_SchoolFloorBasement";
    constexpr char KEY_PAPER_MAP_SCHOOL_FLOOR_1[]         = "PaperMap_SchoolFloor1";
    constexpr char KEY_PAPER_MAP_SCHOOL_FLOOR_2[]         = "PaperMap_SchoolFloor2";
    constexpr char KEY_PAPER_MAP_SCHOOL_FLOOR_ROOF[]      = "PaperMap_SchoolFloorRoof";
    constexpr char KEY_PAPER_MAP_SCHOOL_DOWN[]            = "PaperMap_SchoolDown";
    constexpr char KEY_PAPER_MAP_SCHOOL_UP[]              = "PaperMap_SchoolUp";
    constexpr char KEY_PAPER_MAP_SCHOOL_BOILER_ROOM[]     = "PaperMap_SchoolBoilerRoom";
    constexpr char KEY_PAPER_MAP_SCHOOL_CHEMISTRY_LAB[]   = "PaperMap_SchoolChemistryLab";
    constexpr char KEY_PAPER_MAP_SCHOOL_CLASS_ROOM[]      = "PaperMap_SchoolClassRoom";
    constexpr char KEY_PAPER_MAP_SCHOOL_CLOCK_TOWER[]     = "PaperMap_SchoolClockTower";
    constexpr char KEY_PAPER_MAP_SCHOOL_COURTYARD[]       = "PaperMap_SchoolCourtyard";
    constexpr char KEY_PAPER_MAP_SCHOOL_ENTRANCE[]        = "PaperMap_SchoolEntrance";
    constexpr char KEY_PAPER_MAP_SCHOOL_HALL[]            = "PaperMap_SchoolHall";
    constexpr char KEY_PAPER_MAP_SCHOOL_INFIRMARY[]       = "PaperMap_SchoolInfirmary";
    constexpr char KEY_PAPER_MAP_SCHOOL_LAB_EQUIP_ROOM[]  = "PaperMap_SchoolLabEquipRoom";
    constexpr char KEY_PAPER_MAP_SCHOOL_LIBRARY[]         = "PaperMap_SchoolLibrary";
    constexpr char KEY_PAPER_MAP_SCHOOL_LIBRARY_RESERVE[] = "PaperMap_SchoolLibraryReserve";
    constexpr char KEY_PAPER_MAP_SCHOOL_LOBBY[]           = "PaperMap_SchoolLobby";
    constexpr char KEY_PAPER_MAP_SCHOOL_LOCKER_ROOM[]     = "PaperMap_SchoolLockerRoom";
    constexpr char KEY_PAPER_MAP_SCHOOL_MUSIC_ROOM[]      = "PaperMap_SchoolMusicRoom";
    constexpr char KEY_PAPER_MAP_SCHOOL_RECEPTION[]       = "PaperMap_SchoolReception";
    constexpr char KEY_PAPER_MAP_SCHOOL_ROOF[]            = "PaperMap_SchoolRoof";
    constexpr char KEY_PAPER_MAP_SCHOOL_STORAGE[]         = "PaperMap_SchoolStorage";
    constexpr char KEY_PAPER_MAP_SCHOOL_TEACHERS_ROOM[]   = "PaperMap_SchoolTeachersRoom";

    constexpr char KEY_PAPER_MAP_HOSPITAL_HEADING[]        = "PaperMap_HospitalHeading";
    constexpr char KEY_PAPER_MAP_HOSPITAL_FLOOR_BASEMENT[] = "PaperMap_HospitalFloorBasement";
    constexpr char KEY_PAPER_MAP_HOSPITAL_FLOOR_1[]        = "PaperMap_HospitalFloor1";
    constexpr char KEY_PAPER_MAP_HOSPITAL_FLOOR_2[]        = "PaperMap_HospitalFloor2";
    constexpr char KEY_PAPER_MAP_HOSPITAL_FLOOR_3[]        = "PaperMap_HospitalFloor3";
    constexpr char KEY_PAPER_MAP_HOSPITAL_DOWN[]           = "PaperMap_HospitalDown";
    constexpr char KEY_PAPER_MAP_HOSPITAL_UP[]             = "PaperMap_HospitalUp";
    constexpr char KEY_PAPER_MAP_HOSPITAL_BOILER_ROOM[]    = "PaperMap_HospitalBoilerRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_CONF_ROOM[]      = "PaperMap_HospitalConfRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_DIRS_OFFICE[]    = "PaperMap_HospitalDirsOffice";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ELEVATOR[]       = "PaperMap_HospitalElevator";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ENTREANCE[]      = "PaperMap_HospitalEntreance";
    constexpr char KEY_PAPER_MAP_HOSPITAL_EXAM_ROOM[]      = "PaperMap_HospitalExamRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_GEN_ROOM[]       = "PaperMap_HospitalGenRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_INTENSIVE_CARE[] = "PaperMap_HospitalIntensiveCare";
    constexpr char KEY_PAPER_MAP_HOSPITAL_KITCHEN[]        = "PaperMap_HospitalKitchen";
    constexpr char KEY_PAPER_MAP_HOSPITAL_LINEN_ROOM[]     = "PaperMap_HospitalLinenRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_MEDICINE_ROOM[]  = "PaperMap_HospitalMedicineRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_MORGUE[]         = "PaperMap_HospitalMorgue";
    constexpr char KEY_PAPER_MAP_HOSPITAL_NURSE_CENTER[]   = "PaperMap_HospitalNurseCenter";
    constexpr char KEY_PAPER_MAP_HOSPITAL_OFFICE[]         = "PaperMap_HospitalOffice";
    constexpr char KEY_PAPER_MAP_HOSPITAL_OP_PREP_ROOM[]   = "PaperMap_HospitalOpPrepRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_OP_ROOM[]        = "PaperMap_HospitalOpRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_RECEPTION[]      = "PaperMap_HospitalReception";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_201[]       = "PaperMap_HospitalRoom201";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_202[]       = "PaperMap_HospitalRoom202";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_203[]       = "PaperMap_HospitalRoom203";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_204[]       = "PaperMap_HospitalRoom204";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_205[]       = "PaperMap_HospitalRoom205";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_206[]       = "PaperMap_HospitalRoom206";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_301[]       = "PaperMap_HospitalRoom301";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_302[]       = "PaperMap_HospitalRoom302";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_303[]       = "PaperMap_HospitalRoom303";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_304[]       = "PaperMap_HospitalRoom304";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_305[]       = "PaperMap_HospitalRoom305";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_306[]       = "PaperMap_HospitalRoom306";
    constexpr char KEY_PAPER_MAP_HOSPITAL_ROOM_307[]       = "PaperMap_HospitalRoom307";
    constexpr char KEY_PAPER_MAP_HOSPITAL_STORAGE_ROOM[]   = "PaperMap_HospitalStorageRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_STORE_ROOM[]     = "PaperMap_HospitalStoreRoom";
    constexpr char KEY_PAPER_MAP_HOSPITAL_WAITING_ROOM[]   = "PaperMap_HospitalWaitingRoom";

    constexpr char KEY_PAPER_MAP_SEWER_LADDER[] = "PaperMap_SewerLadder";

    // ==============
    // Puzzle Labels
    // ==============

    // @todo

    // ======
    // Items
    // ======

    constexpr char KEY_ITEM_HEALTH_DRINK[]           = "Item_HealthDrink";
    constexpr char KEY_ITEM_FIRST_AID_KIT[]          = "Item_FirstAidKit";
    constexpr char KEY_ITEM_AMPOULE[]                = "Item_Ampoule";
    constexpr char KEY_ITEM_LOBBY_KEY[]              = "Item_LobbyKey";
    constexpr char KEY_ITEM_HOUSE_KEY[]              = "Item_HouseKey";
    constexpr char KEY_ITEM_KEY_OF_LION[]            = "Item_KeyOfLion";
    constexpr char KEY_ITEM_KEY_OF_WOODMAN[]         = "Item_KeyOfWoodman";
    constexpr char KEY_ITEM_KEY_OF_SCARECROW[]       = "Item_KeyOfScarecrow";
    constexpr char KEY_ITEM_LIBRARY_RESERVE_KEY[]    = "Item_LibraryReserveKey";
    constexpr char KEY_ITEM_CLASSROOM_KEY[]          = "Item_ClassroomKey";
    constexpr char KEY_ITEM_K_GORDON_KEY[]           = "Item_KGordonKey";
    constexpr char KEY_ITEM_DRAWBRIDGE_KEY[]         = "Item_DrawbridgeKey";
    constexpr char KEY_ITEM_BASEMENT_KEY[]           = "Item_BasementKey";
    constexpr char KEY_ITEM_BASEMENT_STOREROOM_KEY[] = "Item_BasementStoreroomKey";
    constexpr char KEY_ITEM_EXAMINATION_ROOM_KEY[]   = "Item_ExaminationRoomKey";
    constexpr char KEY_ITEM_ANTIQUE_SHOP_KEY[]       = "Item_AntiqueShopKey";
    constexpr char KEY_ITEM_SEWER_KEY[]              = "Item_SewerKey";
    constexpr char KEY_ITEM_KEY_OF_OPHIEL[]          = "Item_KeyOfOphiel";
    constexpr char KEY_ITEM_KEY_OF_HAGITH[]          = "Item_KeyOfHagith";
    constexpr char KEY_ITEM_KEY_OF_PHALEG[]          = "Item_KeyOfPhaleg";
    constexpr char KEY_ITEM_KEY_OF_BETHOR[]          = "Item_KeyOfBethor";
    constexpr char KEY_ITEM_KEY_OF_ARATRON[]         = "Item_KeyOfAratron";
    constexpr char KEY_ITEM_A_NOTE_TO_SCHOOL[]       = "Item_ANoteToSchool";
    constexpr char KEY_ITEM_A_NOTE_DOGHOUSE[]        = "Item_ANoteDoghouse";
    constexpr char KEY_ITEM_PICTURE_CARD[]           = "Item_PictureCard";
    constexpr char KEY_ITEM_SEWER_EXIT_KEY[]         = "Item_SewerExitKey";
    constexpr char KEY_ITEM_CHANNELING_STONE[]       = "Item_ChannelingStone";
    constexpr char KEY_ITEM_CHEMICAL[]               = "Item_Chemical";
    constexpr char KEY_ITEM_GOLD_MEDALLION[]         = "Item_GoldMedallion";
    constexpr char KEY_ITEM_SILVER_MEDALLION[]       = "Item_SilverMedallion";
    constexpr char KEY_ITEM_RUBBER_BALL[]            = "Item_RubberBall";
    constexpr char KEY_ITEM_FLAUROS[]                = "Item_Flauros";
    constexpr char KEY_ITEM_PLASTIC_BOTTLE[]         = "Item_PlasticBottle";
    constexpr char KEY_ITEM_UNKNOWN_LIQUID[]         = "Item_UnknownLiquid";
    constexpr char KEY_ITEM_PLATE_OF_TURTLE[]        = "Item_PlateOfTurtle";
    constexpr char KEY_ITEM_PLATE_OF_HATTER[]        = "Item_PlateOfHatter";
    constexpr char KEY_ITEM_PLATE_OF_CAT[]           = "Item_PlateOfCat";
    constexpr char KEY_ITEM_PLATE_OF_QUEEN[]         = "Item_PlateOfQueen";
    constexpr char KEY_ITEM_BLOOD_PACK[]             = "Item_BloodPack";
    constexpr char KEY_ITEM_DISINFECTING_ALCOHOL[]   = "Item_DisinfectingAlcohol";
    constexpr char KEY_ITEM_LIGHTER[]                = "Item_Lighter";
    constexpr char KEY_ITEM_VIDEO_TAPE[]             = "Item_Video tape";
    constexpr char KEY_ITEM_KAUFMANN_KEY[]           = "Item_KaufmannKey";
    constexpr char KEY_ITEM_RECEIPT[]                = "Item_Receipt";
    constexpr char KEY_ITEM_SAFE_KEY[]               = "Item_SafeKey";
    constexpr char KEY_ITEM_MAGNET[]                 = "Item_Magnet";
    constexpr char KEY_ITEM_MOTORCYCLE_KEY[]         = "Item_MotorcycleKey";
    constexpr char KEY_ITEM_BIRD_CAGE_KEY[]          = "Item_BirdCageKey";
    constexpr char KEY_ITEM_PLIERS[]                 = "Item_Pliers";
    constexpr char KEY_ITEM_SCREWDRIVER[]            = "Item_Screwdriver";
    constexpr char KEY_ITEM_CAMERA[]                 = "Item_Camera";
    constexpr char KEY_ITEM_RING_OF_CONTRACT[]       = "Item_RingOfContract";
    constexpr char KEY_ITEM_STONE_OF_TIME[]          = "Item_StoneOfTime";
    constexpr char KEY_ITEM_AMULET_OF_SOLOMON[]      = "Item_AmuletOfSolomon";
    constexpr char KEY_ITEM_CREST_OF_MERCURY[]       = "Item_CrestOfMercury";
    constexpr char KEY_ITEM_ANKH[]                   = "Item_Ankh";
    constexpr char KEY_ITEM_DAGGER_OF_MELCHIOR[]     = "Item_DaggerOdMelchior";
    constexpr char KEY_ITEM_DISK_OF_OUROBOROS[]      = "Item_DiskOfOuroboros";
    constexpr char KEY_ITEM_KITCHEN_KNIFE[]          = "Item_KitchenKnife";
    constexpr char KEY_ITEM_STEEL_PIPE[]             = "Item_SteelPipe";
    constexpr char KEY_ITEM_ROCK_DRILL[]             = "Item_RockDrill";
    constexpr char KEY_ITEM_HAMMER[]                 = "Item_Hammer";
    constexpr char KEY_ITEM_CHAINSAW[]               = "Item_Chainsaw";
    constexpr char KEY_ITEM_KATANA[]                 = "Item_Katana";
    constexpr char KEY_ITEM_AXE[]                    = "Item_Axe";
    constexpr char KEY_ITEM_HANDGUN[]                = "Item_Handgun";
    constexpr char KEY_ITEM_HUNTING_RIFLE[]          = "Item_HuntingRifle";
    constexpr char KEY_ITEM_SHOTGUN[]                = "Item_Shotgun";
    constexpr char KEY_ITEM_HYPER_BLASTER[]          = "Item_HyperBlaster";
    constexpr char KEY_ITEM_HANDGUN_BULLETS[]        = "Item_HandgunBullets";
    constexpr char KEY_ITEM_RIFLE_SHELLS[]           = "Item_RifleShells";
    constexpr char KEY_ITEM_SHOTGUN_SHELLS[]         = "Item_ShotgunShells";
    constexpr char KEY_ITEM_FLASHLIGHT[]             = "Item_Flashlight";
    constexpr char KEY_ITEM_POCKET_RADIO[]           = "Item_PocketRadio";
    constexpr char KEY_ITEM_GASOLINE_TANK[]          = "Item_GasolineTank";

    // ==================
    // Item Descriptions
    // ==================

    constexpr char KEY_ITEM_DESC_HEALTH_DRINK[]        = "ItemDesc_HealthDrink";
    constexpr char KEY_ITEM_DESC_FIRST_AID_KIT[]       = "ItemDesc_FirstAidKit";
    constexpr char KEY_ITEM_DESC_AMPULE[]              = "ItemDesc_Ampule";
    constexpr char KEY_ITEM_DESC_LOBBY_KEY[]           = "ItemDesc_LobbyKey";
    constexpr char KEY_ITEM_DESC_HOUSE_KEY[]           = "ItemDesc_HouseKey";
    constexpr char KEY_ITEM_DESC_KEY_OF_LION[]         = "ItemDesc_KeyOfLion";
    constexpr char KEY_ITEM_DESC_KEY_OF_WOODMAN[]      = "ItemDesc_KeyOfWoodman";
    constexpr char KEY_ITEM_DESC_KEY_OF_SCARECROW[]    = "ItemDesc_KeyOfScarecrow";
    constexpr char KEY_ITEM_DESC_LIBRARY_RESERVE_KEY[] = "ItemDesc_LibraryReserveKey";
    constexpr char KEY_ITEM_DESC_CLASSROOM_KEY[]       = "ItemDesc_ClassroomKey";
    constexpr char KEY_ITEM_DESC_K_GORDON_KEY[]        = "ItemDesc_KGordonKey";
    constexpr char KEY_ITEM_DESC_DRAWBRIDGE_KEY[]      = "ItemDesc_DrawbridgeKey";
    constexpr char KEY_ITEM_DESC_BASEMENT_KEY[]        = "ItemDesc_BasementKey";
    constexpr char KEY_ITEM_DESC_BASEMENT_STOREROOM[]  = "ItemDesc_BasementStoreroomKey";
    constexpr char KEY_ITEM_DESC_EXAMINATION_ROOM[]    = "ItemDesc_ExaminationRoomKey";
    constexpr char KEY_ITEM_DESC_ANTIQUE_SHOP_KEY[]    = "ItemDesc_AntiqueShopKey";
    constexpr char KEY_ITEM_DESC_SEWER_KEY[]           = "ItemDesc_SewerKey";
    constexpr char KEY_ITEM_DESC_KEY_OF_OPHIEL[]       = "ItemDesc_KeyOfOphiel";
    constexpr char KEY_ITEM_DESC_KEY_OF_HAGITH[]       = "ItemDesc_KeyOfHagith";
    constexpr char KEY_ITEM_DESC_KEY_OF_PHALEG[]       = "ItemDesc_KeyOfPhaleg";
    constexpr char KEY_ITEM_DESC_KEY_OF_BETHOR[]       = "ItemDesc_KeyOfBethor";
    constexpr char KEY_ITEM_DESC_KEY_OF_ARATRON[]      = "ItemDesc_KeyOfAratron";
    constexpr char KEY_ITEM_DESC_A_NOTE_TO_SCHOOL[]    = "ItemDesc_ANoteToSchool";
    constexpr char KEY_ITEM_DESC_A_NOTE_DOGHOUSE[]     = "ItemDesc_ANoteDoghouse";
    constexpr char KEY_ITEM_DESC_PICTURE_CARD[]        = "ItemDesc_PictureCard";
    constexpr char KEY_ITEM_DESC_SEWER_EXIT_KEY[]      = "ItemDesc_SewerExitKey";
    constexpr char KEY_ITEM_DESC_CHANNELING_STONE[]    = "ItemDesc_ChannelingStone";
    constexpr char KEY_ITEM_DESC_CHEMICAL[]            = "ItemDesc_Chemical";
    constexpr char KEY_ITEM_DESC_GOLD_MEDALLION[]      = "ItemDesc_GoldMedallion";
    constexpr char KEY_ITEM_DESC_SILVER_MEDALLION[]    = "ItemDesc_SilverMedallion";
    constexpr char KEY_ITEM_DESC_RUBBER_BALL[]         = "ItemDesc_RubberBall";
    constexpr char KEY_ITEM_DESC_FLAUROS[]             = "ItemDesc_Flauros";
    constexpr char KEY_ITEM_DESC_PLASTIC_BOTTLE[]      = "ItemDesc_PlasticBottle";
    constexpr char KEY_ITEM_DESC_UNKNOWN_LIQUID[]      = "ItemDesc_UnknownLiquid";
    constexpr char KEY_ITEM_DESC_PLATE_OF_TURTLE[]     = "ItemDesc_PlateOfTurtle";
    constexpr char KEY_ITEM_DESC_PLATE_OF_HATTER[]     = "ItemDesc_PlateOfHatter";
    constexpr char KEY_ITEM_DESC_PLATE_OF_CAT[]        = "ItemDesc_PlateOfCat";
    constexpr char KEY_ITEM_DESC_PLATE_OF_QUEEN[]      = "ItemDesc_PlateOfQueen";
    constexpr char KEY_ITEM_DESC_BLOOD_PACK[]          = "ItemDesc_BloodPack";
    constexpr char KEY_ITEM_DESC_DISINFECT_ALCOHOL[]   = "ItemDesc_DisinfectingAlcohol";
    constexpr char KEY_ITEM_DESC_LIGHTER[]             = "ItemDesc_Lighter";
    constexpr char KEY_ITEM_DESC_VIDEO_TAPE[]          = "ItemDesc_VideoTape";
    constexpr char KEY_ITEM_DESC_KAUFMANN_KEY[]        = "ItemDesc_KaufmannKey";
    constexpr char KEY_ITEM_DESC_RECEIPT[]             = "ItemDesc_Receipt";
    constexpr char KEY_ITEM_DESC_SAFE_KEY[]            = "ItemDesc_SafeKey";
    constexpr char KEY_ITEM_DESC_MAGNET[]              = "ItemDesc_Magnet";
    constexpr char KEY_ITEM_DESC_MOTORCYCLE_KEY[]      = "ItemDesc_MotorcycleKey";
    constexpr char KEY_ITEM_DESC_BIRD_CAGE_KEY[]       = "ItemDesc_BirdCageKey";
    constexpr char KEY_ITEM_DESC_PLIERS[]              = "ItemDesc_Pliers";
    constexpr char KEY_ITEM_DESC_SCREWDRIVER[]         = "ItemDesc_Screwdriver";
    constexpr char KEY_ITEM_DESC_CAMERA[]              = "ItemDesc_Camera";
    constexpr char KEY_ITEM_DESC_RING_OF_CONTRACT[]    = "ItemDesc_RingOfContract";
    constexpr char KEY_ITEM_DESC_STONE_OF_TIME[]       = "ItemDesc_StoneOfTime";
    constexpr char KEY_ITEM_DESC_AMULET_OF_SOLOMON[]   = "ItemDesc_AmuletOfSolomon";
    constexpr char KEY_ITEM_DESC_CREST_OF_MERCURY[]    = "ItemDesc_CrestOfMercury";
    constexpr char KEY_ITEM_DESC_ANKH[]                = "ItemDesc_Ankh";
    constexpr char KEY_ITEM_DESC_DAGGER_OF_MELCHIOR[]  = "ItemDesc_DaggerOfMelchior";
    constexpr char KEY_ITEM_DESC_DISK_OF_OUROBOROS[]   = "ItemDesc_DiskOfOuroboros";
    constexpr char KEY_ITEM_DESC_KITCHEN_KNIFE[]       = "ItemDesc_KitchenKnife";
    constexpr char KEY_ITEM_DESC_STEEL_PIPE[]          = "ItemDesc_SteelPipe";
    constexpr char KEY_ITEM_DESC_ROCK_DRILL[]          = "ItemDesc_RockDrill";
    constexpr char KEY_ITEM_DESC_HAMMER[]              = "ItemDesc_Hammer";
    constexpr char KEY_ITEM_DESC_CHAINSAW[]            = "ItemDesc_Chainsaw";
    constexpr char KEY_ITEM_DESC_KATANA[]              = "ItemDesc_Katana";
    constexpr char KEY_ITEM_DESC_AXE[]                 = "ItemDesc_Axe";
    constexpr char KEY_ITEM_DESC_HANDGUN[]             = "ItemDesc_Handgun";
    constexpr char KEY_ITEM_DESC_HUNTING_RIFLE[]       = "ItemDesc_HuntingRifle";
    constexpr char KEY_ITEM_DESC_SHOTGUN[]             = "ItemDesc_Shotgun";
    constexpr char KEY_ITEM_DESC_HYPER_BLASTER[]       = "ItemDesc_HyperBlaster";
    constexpr char KEY_ITEM_DESC_HANDGUN_BULLETS[]     = "ItemDesc_HandgunBullets";
    constexpr char KEY_ITEM_DESC_RIFLE_SHELLS[]        = "ItemDesc_RifleShells";
    constexpr char KEY_ITEM_DESC_SHOTGUN_SHELLS[]      = "ItemDesc_ShotgunShells";
    constexpr char KEY_ITEM_DESC_FLASHLIGHT[]          = "ItemDesc_Flashlight";
    constexpr char KEY_ITEM_DESC_POCKET_RADIO[]        = "ItemDesc_PocketRadio";
    constexpr char KEY_ITEM_DESC_GASOLINE_TANK[]       = "ItemDesc_GasolineTank";

    // ==========
    // Game Over
    // ==========

    constexpr char KEY_GAME_OVER_HEADING[]      = "GameOver_Heading";
    constexpr char KEY_GAME_OVER_TIPS_HEADING[] = "GameOver_Heading";
    constexpr char KEY_GAME_OVER_TIP_1[]        = "GameOver_Tip1";
    constexpr char KEY_GAME_OVER_TIP_2[]        = "GameOver_Tip2";
    constexpr char KEY_GAME_OVER_TIP_3[]        = "GameOver_Tip3";
    constexpr char KEY_GAME_OVER_TIP_4[]        = "GameOver_Tip4";
    constexpr char KEY_GAME_OVER_TIP_5[]        = "GameOver_Tip5";
    constexpr char KEY_GAME_OVER_TIP_6[]        = "GameOver_Tip6";
    constexpr char KEY_GAME_OVER_TIP_7[]        = "GameOver_Tip7";
    constexpr char KEY_GAME_OVER_TIP_8[]        = "GameOver_Tip8";
    constexpr char KEY_GAME_OVER_TIP_9[]        = "GameOver_Tip9";
    constexpr char KEY_GAME_OVER_TIP_10[]       = "GameOver_Tip10";
    constexpr char KEY_GAME_OVER_TIP_11[]       = "GameOver_Tip11";
    constexpr char KEY_GAME_OVER_TIP_12[]       = "GameOver_Tip12";
    constexpr char KEY_GAME_OVER_TIP_13[]       = "GameOver_Tip13";
    constexpr char KEY_GAME_OVER_TIP_14[]       = "GameOver_Tip14";
    constexpr char KEY_GAME_OVER_TIP_15[]       = "GameOver_Tip15";

    // ====================
    // Common Map Messages
    // ====================

    constexpr char KEY_COMMON_MSG_YES[]                  = "CommonMsg_Yes";
    constexpr char KEY_COMMON_MSG_NO[]                   = "CommonMsg_No";
    constexpr char KEY_COMMON_MSG_SOMEDAY[]              = "CommonMsg_Someday";
    constexpr char KEY_COMMON_MSG_NO_MAP[]               = "CommonMsg_NoMap";
    constexpr char KEY_COMMON_MSG_TOO_DARK_FOR_MAP[]     = "CommonMsg_TooDarkForMap";
    constexpr char KEY_COMMON_MSG_TAKE_FIRST_AID_KIT[]   = "CommonMsg_TakeFirstAidKit";
    constexpr char KEY_COMMON_MSG_TAKE_HEALTH_DRINK[]    = "CommonMsg_TakeHealthDrink";
    constexpr char KEY_COMMON_MSG_TAKE_AMPOULE[]         = "CommonMsg_TakeAmpoule";
    constexpr char KEY_COMMON_MSG_TAKE_HANDGUN_BULLETS[] = "CommonMsg_TakeHandgunBullets";
    constexpr char KEY_COMMON_MSG_TAKE_RIFLE_SHELLS[]    = "CommonMsg_TakeRifleShells";
    constexpr char KEY_COMMON_MSG_TAKE_SHOTGUN_SHELLS[]  = "CommonMsg_TakeShotgunShells";
    constexpr char KEY_COMMON_MSG_DOOR_JAMMED[]          = "CommonMsg_DoorJammed";
    constexpr char KEY_COMMON_MSG_DOOR_LOCKED[]          = "CommonMsg_DoorLocked";
    constexpr char KEY_COMMON_MSG_DOOR_UNLOCKED[]        = "CommonMsg_DoorUnlocked";
    constexpr char KEY_COMMON_MSG_NOW_MAKING[]           = "CommonMsg_NowMaking";

    // =========================
    // Map 0, Stage 00 Messages
    // =========================

    constexpr char KEY_M0S00_MSG_1[]  = "M0S00Msg_1";
    constexpr char KEY_M0S00_MSG_2[]  = "M0S00Msg_2";
    constexpr char KEY_M0S00_MSG_3[]  = "M0S00Msg_3";
    constexpr char KEY_M0S00_MSG_4[]  = "M0S00Msg_4";
    constexpr char KEY_M0S00_MSG_5[]  = "M0S00Msg_5";
    constexpr char KEY_M0S00_MSG_6[]  = "M0S00Msg_6";
    constexpr char KEY_M0S00_MSG_7[]  = "M0S00Msg_7";
    constexpr char KEY_M0S00_MSG_8[]  = "M0S00Msg_8";
    constexpr char KEY_M0S00_MSG_9[]  = "M0S00Msg_9";
    constexpr char KEY_M0S00_MSG_10[] = "M0S00Msg_10";
    constexpr char KEY_M0S00_MSG_11[] = "M0S00Msg_11";
    constexpr char KEY_M0S00_MSG_12[] = "M0S00Msg_12";
    constexpr char KEY_M0S00_MSG_13[] = "M0S00Msg_13";
    constexpr char KEY_M0S00_MSG_14[] = "M0S00Msg_14";
    constexpr char KEY_M0S00_MSG_15[] = "M0S00Msg_15";
    constexpr char KEY_M0S00_MSG_16[] = "M0S00Msg_16";
    constexpr char KEY_M0S00_MSG_17[] = "M0S00Msg_17";
    constexpr char KEY_M0S00_MSG_18[] = "M0S00Msg_18";
    constexpr char KEY_M0S00_MSG_19[] = "M0S00Msg_19";
    constexpr char KEY_M0S00_MSG_20[] = "M0S00Msg_20";
    constexpr char KEY_M0S00_MSG_21[] = "M0S00Msg_21";
    constexpr char KEY_M0S00_MSG_22[] = "M0S00Msg_22";
    constexpr char KEY_M0S00_MSG_23[] = "M0S00Msg_23";
    constexpr char KEY_M0S00_MSG_24[] = "M0S00Msg_24";

    // @todo
}
