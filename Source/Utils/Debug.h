#pragma once

namespace Silent::Utils::Debug
{
#ifdef _DEBUG
    constexpr bool IS_DEBUG = true;
#else
    constexpr bool IS_DEBUG = false;
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
