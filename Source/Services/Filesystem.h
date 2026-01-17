#pragma once

namespace Silent::Services
{
    constexpr char ASSETS_DIR_NAME[]             = "Assets";
    constexpr char ASSETS_FONTS_DIR_NAME[]       = "Fonts";
    constexpr char ASSETS_LOCALES_DIR_NAME[]     = "Locales";
    constexpr char ASSETS_PSX_DIR_NAME[]         = "Stream/Psx"; // @todo Make this simply refer to "Stream" to allow addition of new streamable assets. Requires index -> name refactor.
    constexpr char SAVEGAME_DIR_NAME[]           = "Savegame";
    constexpr char SAVEGAME_FILE_DIR_NAME_BASE[] = "File ";
    constexpr char SCREENSHOTS_DIR_NAME[]        = "Screenshots";
    constexpr char SCREENSHOT_FILENAME_BASE[]    = "Screenshot_";
    constexpr char SHADERS_DIR_NAME[]            = "Shaders";

    constexpr char OPTIONS_FILENAME[] = "Options";

    constexpr char JSON_FILE_EXT[]        = ".json";
    constexpr char PNG_FILE_EXT[]         = ".png";
    constexpr char SAVEGAME_FILE_EXT[]    = ".save";
    constexpr char SPV_SHADER_FILE_EXT[]  = ".spv"; // @todo Use these in `Pipeline.cpp`.
    constexpr char MSL_SHADER_FILE_EXT[]  = ".msl";
    constexpr char DXIL_SHADER_FILE_EXT[] = ".dxil";
    
    constexpr int JSON_INDENT_SIZE = 4;

    /** @brief Filesystem manager. */
    class FilesystemManager
    {
    private:
        // =======
        // Fields
        // =======

        std::filesystem::path _appDir         = {};
        std::filesystem::path _assetsDir      = {};
        std::filesystem::path _workDir        = {};
        std::filesystem::path _savegameDir    = {};
        std::filesystem::path _screenshotsDir = {};
        std::filesystem::path _shadersDir     = {};

    public:
        // =============
        // Constructors
        // =============

        FilesystemManager() = default;

        // ========
        // Getters
        // ========

        const std::filesystem::path& GetAppDirectory() const;
        const std::filesystem::path& GetAssetsDirectory() const;
        const std::filesystem::path& GetWorkDirectory() const;
        const std::filesystem::path& GetSavegameDirectory() const;
        const std::filesystem::path& GetScreenshotsDirectory() const;
        const std::filesystem::path& GetShadersDirectory() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the filesystem. */
        void Initialize();
    };
}
