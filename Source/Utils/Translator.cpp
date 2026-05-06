#include "Framework.h"
#include "Utils/Translator.h"

#include "Services/Filesystem.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Services;

namespace Silent::Utils
{
    void TranslationManager::Initialize(const std::filesystem::path& localesPath)
    {
        // Set path.
        _localesPath = localesPath;
        if (!std::filesystem::exists(_localesPath) || !std::filesystem::is_directory(_localesPath))
        {
            Debug::Log("Locales path doesn't exist.", Debug::LogLevel::Error);
            return;
        }

        // Register locale names.
        _localeNames.clear();
        for (const auto& localeName : std::filesystem::directory_iterator(_localesPath))
        {
            if (!localeName.is_directory())
            {
                continue;
            }

            _localeNames.push_back(localeName.path().filename().string());
        }

        if (_localeNames.empty())
        {
            Debug::Log("No translator locales registered.", Debug::LogLevel::Error);
            return;
        }

        // Set first locale as default.
        LoadActiveLocale(_localeNames.front());
    }

    const std::string& TranslationManager::GetActiveLocaleName() const
    {
        return _activeLocaleName;
    }

    const std::vector<std::string>& TranslationManager::GetLocaleNames() const
    {
        return _localeNames;
    }

    void TranslationManager::SetActiveLocale(const std::string& localeName)
    {
        // Check if new locale is already active or queued.
        if (localeName == _activeLocaleName ||
            localeName == _queuedLocaleName)
        {
            return;
        }

        // Check if new locale is registered.
        if (!Contains(_localeNames, localeName))
        {
            Debug::Log(Fmt("Attempted to set unregistered translator locale `{}`.", localeName), Debug::LogLevel::Error);
            return;
        }

        // Load new locale or queue to load when translator becomes unlocked.
        if (_isLocked)
        {
            _queuedLocaleName = localeName;
        }
        else
        {
            LoadActiveLocale(localeName);
        }
    }

    void TranslationManager::Lock()
    {
        _isLocked = true;
    }

    void TranslationManager::Unlock()
    {
        _isLocked = false;

        // Load new queued locale.
        if (!_queuedLocaleName.empty())
        {
            LoadActiveLocale(_queuedLocaleName);
            _queuedLocaleName = {};
        }
    }

    std::string TranslationManager::operator()(const std::string& translationKey) const
    {
        if (_activeLocale.empty())
        {
            return translationKey;
        }

        return _activeLocale.value(translationKey, translationKey);
    }

    void TranslationManager::LoadActiveLocale(const std::string& localeName)
    {
        // Open locale file stream and set new locale.
        auto localePath = _localesPath / localeName / Fmt("{}{}", ASSETS_LOCALE_FILENAME, JSON_FILE_EXT);
        auto stream     = Stream(localePath, true, false);
        if (!stream.IsOpen())
        {
            Debug::Log(Fmt("Failed to load translator locale `{}`.", localeName), Debug::LogLevel::Warning);
            _activeLocale = json();
        }
        else
        {
            _activeLocale = stream.ReadJson();
        }

        _activeLocaleName = localeName;
        stream.Close();
    }
}
