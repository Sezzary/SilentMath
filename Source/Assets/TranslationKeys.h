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
    constexpr char KEY_SYS_GAMEPAD_BATTERY_IS_LOW[]  = "sys_GamepadBatteryIsLow";

    // ======
    // Intro
    // ======

    constexpr char KEY_INTRO_WARNING[]       = "intro_Warning";
    constexpr char KEY_INTRO_FEAR_OF_BLOOD[] = "intro_FearOfBlood";

    // ======
    // Menus
    // ======

    constexpr char KEY_MAIN_MENU_OPENING_QUOTE[] = "mainMenu_OpeningQuote";
    constexpr char KEY_MAIN_MENU_CLOSING_QUOTE[] = "mainMenu_ClosingQuote";
    constexpr char KEY_MAIN_MENU_START[]         = "mainMenu_Start";
    constexpr char KEY_MAIN_MENU_CONTINUE[]      = "mainMenu_Continue";
    constexpr char KEY_MAIN_MENU_LOAD[]          = "mainMenu_Load";
    constexpr char KEY_MAIN_MENU_OPTION[]        = "mainMenu_Option";
    constexpr char KEY_MAIN_MENU_QUIT[]          = "mainMenu_Quit";
    constexpr char KEY_DIFF_MENU_EASY[]          = "mainMenu_Easy";
    constexpr char KEY_MAIN_MENU_NORMAL[]        = "mainMenu_Normal";
    constexpr char KEY_MAIN_MENU_HARD[]          = "mainMenu_Hard";

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
    constexpr char KEY_OPTIONS_MENU_LANGUAGE[]         = "optionsMenu_Language";
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
    constexpr char KEY_PAPER_MAP_PHARMACY[]             = "paperMap_Pharmacy";
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

    // ======
    // Items
    // ======

    constexpr char KEY_ITEM_HEALTH_DRINK[]           = "item_HealthDrink";
    constexpr char KEY_ITEM_FIRST_AID_KIT[]          = "item_FirstAidKit";
    constexpr char KEY_ITEM_AMPOULE[]                = "item_Ampoule";
    constexpr char KEY_ITEM_LOBBY_KEY[]              = "item_LobbyKey";
    constexpr char KEY_ITEM_HOUSE_KEY[]              = "item_HouseKey";
    constexpr char KEY_ITEM_KEY_OF_LION[]            = "item_KeyOfLion";
    constexpr char KEY_ITEM_KEY_OF_WOODMAN[]         = "item_KeyOfWoodman";
    constexpr char KEY_ITEM_KEY_OF_SCARECROW[]       = "item_KeyOfScarecrow";
    constexpr char KEY_ITEM_LIBRARY_RESERVE_KEY[]    = "item_LibraryReserveKey";
    constexpr char KEY_ITEM_CLASSROOM_KEY[]          = "item_ClassroomKey";
    constexpr char KEY_ITEM_K_GORDON_KEY[]           = "item_KGordonKey";
    constexpr char KEY_ITEM_DRAWBRIDGE_KEY[]         = "item_DrawbridgeKey";
    constexpr char KEY_ITEM_BASEMENT_KEY[]           = "item_BasementKey";
    constexpr char KEY_ITEM_BASEMENT_STOREROOM_KEY[] = "item_BasementStoreroomKey";
    constexpr char KEY_ITEM_EXAMINATION_ROOM_KEY[]   = "item_ExaminationRoomKey";
    constexpr char KEY_ITEM_ANTIQUE_SHOP_KEY[]       = "item_AntiqueShopKey";
    constexpr char KEY_ITEM_SEWER_KEY[]              = "item_SewerKey";
    constexpr char KEY_ITEM_KEY_OF_OPHIEL[]          = "item_KeyOfOphiel";
    constexpr char KEY_ITEM_KEY_OF_HAGITH[]          = "item_KeyOfHagith";
    constexpr char KEY_ITEM_KEY_OF_PHALEG[]          = "item_KeyOfPhaleg";
    constexpr char KEY_ITEM_KEY_OF_BETHOR[]          = "item_KeyOfBethor";
    constexpr char KEY_ITEM_KEY_OF_ARATRON[]         = "item_KeyOfAratron";
    constexpr char KEY_ITEM_A_NOTE_TO_SCHOOL[]       = "item_ANoteToSchool";
    constexpr char KEY_ITEM_A_NOTE_DOGHOUSE[]        = "item_ANoteDoghouse";
    constexpr char KEY_ITEM_PICTURE_CARD[]           = "item_PictureCard";
    constexpr char KEY_ITEM_SEWER_EXIT_KEY[]         = "item_SewerExitKey";
    constexpr char KEY_ITEM_CHANNELING_STONE[]       = "item_ChannelingStone";
    constexpr char KEY_ITEM_CHEMICAL[]               = "item_Chemical";
    constexpr char KEY_ITEM_GOLD_MEDALLION[]         = "item_GoldMedallion";
    constexpr char KEY_ITEM_SILVER_MEDALLION[]       = "item_SilverMedallion";
    constexpr char KEY_ITEM_RUBBER_BALL[]            = "item_RubberBall";
    constexpr char KEY_ITEM_FLAUROS[]                = "item_Flauros";
    constexpr char KEY_ITEM_PLASTIC_BOTTLE[]         = "item_PlasticBottle";
    constexpr char KEY_ITEM_UNKNOWN_LIQUID[]         = "item_UnknownLiquid";
    constexpr char KEY_ITEM_PLATE_OF_TURTLE[]        = "item_PlateOfTurtle";
    constexpr char KEY_ITEM_PLATE_OF_HATTER[]        = "item_PlateOfHatter";
    constexpr char KEY_ITEM_PLATE_OF_CAT[]           = "item_PlateOfCat";
    constexpr char KEY_ITEM_PLATE_OF_QUEEN[]         = "item_PlateOfQueen";
    constexpr char KEY_ITEM_BLOOD_PACK[]             = "item_BloodPack";
    constexpr char KEY_ITEM_DISINFECTING_ALCOHOL[]   = "item_DisinfectingAlcohol";
    constexpr char KEY_ITEM_LIGHTER[]                = "item_Lighter";
    constexpr char KEY_ITEM_VIDEO_TAPE[]             = "item_Video tape";
    constexpr char KEY_ITEM_KAUFMANN_KEY[]           = "item_KaufmannKey";
    constexpr char KEY_ITEM_RECEIPT[]                = "item_Receipt";
    constexpr char KEY_ITEM_SAFE_KEY[]               = "item_SafeKey";
    constexpr char KEY_ITEM_MAGNET[]                 = "item_Magnet";
    constexpr char KEY_ITEM_MOTORCYCLE_KEY[]         = "item_MotorcycleKey";
    constexpr char KEY_ITEM_BIRD_CAGE_KEY[]          = "item_BirdCageKey";
    constexpr char KEY_ITEM_PLIERS[]                 = "item_Pliers";
    constexpr char KEY_ITEM_SCREWDRIVER[]            = "item_Screwdriver";
    constexpr char KEY_ITEM_CAMERA[]                 = "item_Camera";
    constexpr char KEY_ITEM_RING_OF_CONTRACT[]       = "item_RingOfContract";
    constexpr char KEY_ITEM_STONE_OF_TIME[]          = "item_StoneOfTime";
    constexpr char KEY_ITEM_AMULET_OF_SOLOMON[]      = "item_AmuletOfSolomon";
    constexpr char KEY_ITEM_CREST_OF_MERCURY[]       = "item_CrestOfMercury";
    constexpr char KEY_ITEM_ANKH[]                   = "item_Ankh";
    constexpr char KEY_ITEM_DAGGER_OF_MELCHIOR[]     = "item_DaggerOdMelchior";
    constexpr char KEY_ITEM_DISK_OF_OUROBOROS[]      = "item_DiskOfOuroboros";
    constexpr char KEY_ITEM_KITCHEN_KNIFE[]          = "item_KitchenKnife";
    constexpr char KEY_ITEM_STEEL_PIPE[]             = "item_SteelPipe";
    constexpr char KEY_ITEM_ROCK_DRILL[]             = "item_RockDrill";
    constexpr char KEY_ITEM_HAMMER[]                 = "item_Hammer";
    constexpr char KEY_ITEM_CHAINSAW[]               = "item_Chainsaw";
    constexpr char KEY_ITEM_KATANA[]                 = "item_Katana";
    constexpr char KEY_ITEM_AXE[]                    = "item_Axe";
    constexpr char KEY_ITEM_HANDGUN[]                = "item_Handgun";
    constexpr char KEY_ITEM_HUNTING_RIFLE[]          = "item_HuntingRifle";
    constexpr char KEY_ITEM_SHOTGUN[]                = "item_Shotgun";
    constexpr char KEY_ITEM_HYPER_BLASTER[]          = "item_HyperBlaster";
    constexpr char KEY_ITEM_HANDGUN_BULLETS[]        = "item_HandgunBullets";
    constexpr char KEY_ITEM_RIFLE_SHELLS[]           = "item_RifleShells";
    constexpr char KEY_ITEM_SHOTGUN_SHELLS[]         = "item_ShotgunShells";
    constexpr char KEY_ITEM_FLASHLIGHT[]             = "item_Flashlight";
    constexpr char KEY_ITEM_POCKET_RADIO[]           = "item_PocketRadio";
    constexpr char KEY_ITEM_GASOLINE_TANK[]          = "item_GasolineTank";

    // ==================
    // Item Descriptions
    // ==================

    constexpr char KEY_ITEM_DESC_HEALTH_DRINK[]        = "itemDesc_HealthDrink";
    constexpr char KEY_ITEM_DESC_FIRST_AID_KIT[]       = "itemDesc_FirstAidKit";
    constexpr char KEY_ITEM_DESC_AMPULE[]              = "itemDesc_Ampule";
    constexpr char KEY_ITEM_DESC_LOBBY_KEY[]           = "itemDesc_LobbyKey";
    constexpr char KEY_ITEM_DESC_HOUSE_KEY[]           = "itemDesc_HouseKey";
    constexpr char KEY_ITEM_DESC_KEY_OF_LION[]         = "itemDesc_KeyOfLion";
    constexpr char KEY_ITEM_DESC_KEY_OF_WOODMAN[]      = "itemDesc_KeyOfWoodman";
    constexpr char KEY_ITEM_DESC_KEY_OF_SCARECROW[]    = "itemDesc_KeyOfScarecrow";
    constexpr char KEY_ITEM_DESC_LIBRARY_RESERVE_KEY[] = "itemDesc_LibraryReserveKey";
    constexpr char KEY_ITEM_DESC_CLASSROOM_KEY[]       = "itemDesc_ClassroomKey";
    constexpr char KEY_ITEM_DESC_K_GORDON_KEY[]        = "itemDesc_KGordonKey";
    constexpr char KEY_ITEM_DESC_DRAWBRIDGE_KEY[]      = "itemDesc_DrawbridgeKey";
    constexpr char KEY_ITEM_DESC_BASEMENT_KEY[]        = "itemDesc_BasementKey";
    constexpr char KEY_ITEM_DESC_BASEMENT_STOREROOM[]  = "itemDesc_BasementStoreroomKey";
    constexpr char KEY_ITEM_DESC_EXAMINATION_ROOM[]    = "itemDesc_ExaminationRoomKey";
    constexpr char KEY_ITEM_DESC_ANTIQUE_SHOP_KEY[]    = "itemDesc_AntiqueShopKey";
    constexpr char KEY_ITEM_DESC_SEWER_KEY[]           = "itemDesc_SewerKey";
    constexpr char KEY_ITEM_DESC_KEY_OF_OPHIEL[]       = "itemDesc_KeyOfOphiel";
    constexpr char KEY_ITEM_DESC_KEY_OF_HAGITH[]       = "itemDesc_KeyOfHagith";
    constexpr char KEY_ITEM_DESC_KEY_OF_PHALEG[]       = "itemDesc_KeyOfPhaleg";
    constexpr char KEY_ITEM_DESC_KEY_OF_BETHOR[]       = "itemDesc_KeyOfBethor";
    constexpr char KEY_ITEM_DESC_KEY_OF_ARATRON[]      = "itemDesc_KeyOfAratron";
    constexpr char KEY_ITEM_DESC_A_NOTE_TO_SCHOOL[]    = "itemDesc_ANoteToSchool";
    constexpr char KEY_ITEM_DESC_A_NOTE_DOGHOUSE[]     = "itemDesc_ANoteDoghouse";
    constexpr char KEY_ITEM_DESC_PICTURE_CARD[]        = "itemDesc_PictureCard";
    constexpr char KEY_ITEM_DESC_SEWER_EXIT_KEY[]      = "itemDesc_SewerExitKey";
    constexpr char KEY_ITEM_DESC_CHANNELING_STONE[]    = "itemDesc_ChannelingStone";
    constexpr char KEY_ITEM_DESC_CHEMICAL[]            = "itemDesc_Chemical";
    constexpr char KEY_ITEM_DESC_GOLD_MEDALLION[]      = "itemDesc_GoldMedallion";
    constexpr char KEY_ITEM_DESC_SILVER_MEDALLION[]    = "itemDesc_SilverMedallion";
    constexpr char KEY_ITEM_DESC_RUBBER_BALL[]         = "itemDesc_RubberBall";
    constexpr char KEY_ITEM_DESC_FLAUROS[]             = "itemDesc_Flauros";
    constexpr char KEY_ITEM_DESC_PLASTIC_BOTTLE[]      = "itemDesc_PlasticBottle";
    constexpr char KEY_ITEM_DESC_UNKNOWN_LIQUID[]      = "itemDesc_UnknownLiquid";
    constexpr char KEY_ITEM_DESC_PLATE_OF_TURTLE[]     = "itemDesc_PlateOfTurtle";
    constexpr char KEY_ITEM_DESC_PLATE_OF_HATTER[]     = "itemDesc_PlateOfHatter";
    constexpr char KEY_ITEM_DESC_PLATE_OF_CAT[]        = "itemDesc_PlateOfCat";
    constexpr char KEY_ITEM_DESC_PLATE_OF_QUEEN[]      = "itemDesc_PlateOfQueen";
    constexpr char KEY_ITEM_DESC_BLOOD_PACK[]          = "itemDesc_BloodPack";
    constexpr char KEY_ITEM_DESC_DISINFECT_ALCOHOL[]   = "itemDesc_DisinfectingAlcohol";
    constexpr char KEY_ITEM_DESC_LIGHTER[]             = "itemDesc_Lighter";
    constexpr char KEY_ITEM_DESC_VIDEO_TAPE[]          = "itemDesc_VideoTape";
    constexpr char KEY_ITEM_DESC_KAUFMANN_KEY[]        = "itemDesc_KaufmannKey";
    constexpr char KEY_ITEM_DESC_RECEIPT[]             = "itemDesc_Receipt";
    constexpr char KEY_ITEM_DESC_SAFE_KEY[]            = "itemDesc_SafeKey";
    constexpr char KEY_ITEM_DESC_MAGNET[]              = "itemDesc_Magnet";
    constexpr char KEY_ITEM_DESC_MOTORCYCLE_KEY[]      = "itemDesc_MotorcycleKey";
    constexpr char KEY_ITEM_DESC_BIRD_CAGE_KEY[]       = "itemDesc_BirdCageKey";
    constexpr char KEY_ITEM_DESC_PLIERS[]              = "itemDesc_Pliers";
    constexpr char KEY_ITEM_DESC_SCREWDRIVER[]         = "itemDesc_Screwdriver";
    constexpr char KEY_ITEM_DESC_CAMERA[]              = "itemDesc_Camera";
    constexpr char KEY_ITEM_DESC_RING_OF_CONTRACT[]    = "itemDesc_RingOfContract";
    constexpr char KEY_ITEM_DESC_STONE_OF_TIME[]       = "itemDesc_StoneOfTime";
    constexpr char KEY_ITEM_DESC_AMULET_OF_SOLOMON[]   = "itemDesc_AmuletOfSolomon";
    constexpr char KEY_ITEM_DESC_CREST_OF_MERCURY[]    = "itemDesc_CrestOfMercury";
    constexpr char KEY_ITEM_DESC_ANKH[]                = "itemDesc_Ankh";
    constexpr char KEY_ITEM_DESC_DAGGER_OF_MELCHIOR[]  = "itemDesc_DaggerOfMelchior";
    constexpr char KEY_ITEM_DESC_DISK_OF_OUROBOROS[]   = "itemDesc_DiskOfOuroboros";
    constexpr char KEY_ITEM_DESC_KITCHEN_KNIFE[]       = "itemDesc_KitchenKnife";
    constexpr char KEY_ITEM_DESC_STEEL_PIPE[]          = "itemDesc_SteelPipe";
    constexpr char KEY_ITEM_DESC_ROCK_DRILL[]          = "itemDesc_RockDrill";
    constexpr char KEY_ITEM_DESC_HAMMER[]              = "itemDesc_Hammer";
    constexpr char KEY_ITEM_DESC_CHAINSAW[]            = "itemDesc_Chainsaw";
    constexpr char KEY_ITEM_DESC_KATANA[]              = "itemDesc_Katana";
    constexpr char KEY_ITEM_DESC_AXE[]                 = "itemDesc_Axe";
    constexpr char KEY_ITEM_DESC_HANDGUN[]             = "itemDesc_Handgun";
    constexpr char KEY_ITEM_DESC_HUNTING_RIFLE[]       = "itemDesc_HuntingRifle";
    constexpr char KEY_ITEM_DESC_SHOTGUN[]             = "itemDesc_Shotgun";
    constexpr char KEY_ITEM_DESC_HYPER_BLASTER[]       = "itemDesc_HyperBlaster";
    constexpr char KEY_ITEM_DESC_HANDGUN_BULLETS[]     = "itemDesc_HandgunBullets";
    constexpr char KEY_ITEM_DESC_RIFLE_SHELLS[]        = "itemDesc_RifleShells";
    constexpr char KEY_ITEM_DESC_SHOTGUN_SHELLS[]      = "itemDesc_ShotgunShells";
    constexpr char KEY_ITEM_DESC_FLASHLIGHT[]          = "itemDesc_Flashlight";
    constexpr char KEY_ITEM_DESC_POCKET_RADIO[]        = "itemDesc_PocketRadio";
    constexpr char KEY_ITEM_DESC_GASOLINE_TANK[]       = "itemDesc_GasolineTank";

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

    constexpr char KEY_COMMON_MSG_YES[]                  = "commonMsg_Yes";
    constexpr char KEY_COMMON_MSG_NO[]                   = "commonMsg_No";
    constexpr char KEY_COMMON_MSG_SOMEDAY[]              = "commonMsg_Someday";
    constexpr char KEY_COMMON_MSG_NO_MAP[]               = "commonMsg_NoMap";
    constexpr char KEY_COMMON_MSG_TOO_DARK_FOR_MAP[]     = "commonMsg_TooDarkForMap";
    constexpr char KEY_COMMON_MSG_TAKE_FIRST_AID_KIT[]   = "commonMsg_TakeFirstAidKit";
    constexpr char KEY_COMMON_MSG_TAKE_HEALTH_DRINK[]    = "commonMsg_TakeHealthDrink";
    constexpr char KEY_COMMON_MSG_TAKE_AMPOULE[]         = "commonMsg_TakeAmpoule";
    constexpr char KEY_COMMON_MSG_TAKE_HANDGUN_BULLETS[] = "commonMsg_TakeHandgunBullets";
    constexpr char KEY_COMMON_MSG_TAKE_RIFLE_SHELLS[]    = "commonMsg_TakeRifleShells";
    constexpr char KEY_COMMON_MSG_TAKE_SHOTGUN_SHELLS[]  = "commonMsg_TakeShotgunShells";
    constexpr char KEY_COMMON_MSG_DOOR_JAMMED[]          = "commonMsg_DoorJammed";
    constexpr char KEY_COMMON_MSG_DOOR_LOCKED[]          = "commonMsg_DoorLocked";
    constexpr char KEY_COMMON_MSG_DOOR_UNLOCKED[]        = "commonMsg_DoorUnlocked";
    constexpr char KEY_COMMON_MSG_NOW_MAKING[]           = "commonMsg_NowMaking";

    // =========================
    // Map 0, Stage 00 Messages
    // =========================

    constexpr char KEY_M0S00_MSG_1[]  = "m0s00Msg_1";
    constexpr char KEY_M0S00_MSG_2[]  = "m0s00Msg_2";
    constexpr char KEY_M0S00_MSG_3[]  = "m0s00Msg_3";
    constexpr char KEY_M0S00_MSG_4[]  = "m0s00Msg_4";
    constexpr char KEY_M0S00_MSG_5[]  = "m0s00Msg_5";
    constexpr char KEY_M0S00_MSG_6[]  = "m0s00Msg_6";
    constexpr char KEY_M0S00_MSG_7[]  = "m0s00Msg_7";
    constexpr char KEY_M0S00_MSG_8[]  = "m0s00Msg_8";
    constexpr char KEY_M0S00_MSG_9[]  = "m0s00Msg_9";
    constexpr char KEY_M0S00_MSG_10[] = "m0s00Msg_10";
    constexpr char KEY_M0S00_MSG_11[] = "m0s00Msg_11";
    constexpr char KEY_M0S00_MSG_12[] = "m0s00Msg_12";
    constexpr char KEY_M0S00_MSG_13[] = "m0s00Msg_13";
    constexpr char KEY_M0S00_MSG_14[] = "m0s00Msg_14";
    constexpr char KEY_M0S00_MSG_15[] = "m0s00Msg_15";
    constexpr char KEY_M0S00_MSG_16[] = "m0s00Msg_16";
    constexpr char KEY_M0S00_MSG_17[] = "m0s00Msg_17";
    constexpr char KEY_M0S00_MSG_18[] = "m0s00Msg_18";
    constexpr char KEY_M0S00_MSG_19[] = "m0s00Msg_19";
    constexpr char KEY_M0S00_MSG_20[] = "m0s00Msg_20";
    constexpr char KEY_M0S00_MSG_21[] = "m0s00Msg_21";
    constexpr char KEY_M0S00_MSG_22[] = "m0s00Msg_22";
    constexpr char KEY_M0S00_MSG_23[] = "m0s00Msg_23";
    constexpr char KEY_M0S00_MSG_24[] = "m0s00Msg_24";

    // @todo
}
