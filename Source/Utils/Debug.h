#pragma once

namespace Silent::Utils::Debug
{
#ifdef DEBUG
    constexpr bool IS_DEBUG_MODE = true;
#else
    constexpr bool IS_DEBUG_MODE = false;
#endif

    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    void Log(const std::string& msg, LogLevel level = LogLevel::Info, bool allowSpam = false);
    void Assert(bool cond, const std::string& msg);
    void StartTimer();
    void EndTimer();
}
