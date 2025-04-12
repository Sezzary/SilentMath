#include "Framework.h"
#include "Utils/Debug.h"

namespace Silent::Utils::Debug
{
    static auto StartTime = std::chrono::high_resolution_clock::time_point{};

    void Log(const std::string& msg, LogLevel level, bool allowSpam)
    {
        static auto prevMsg = std::string();
        if (prevMsg == msg && !allowSpam)
            return;
        prevMsg = msg;

        switch (level)
        {
            default:
            case LogLevel::Info:
                spdlog::info(msg);
                break;
            
            case LogLevel::Warning:
                spdlog::warn(msg);
                break;

            case LogLevel::Error:
                spdlog::error(msg);
                break;
        }
    }

    void Assert(bool cond, const std::string& msg)
    {
        if constexpr (!IS_DEBUG_MODE)
            return;

        if (!cond)
        {
            Log(msg, LogLevel::Error);
            throw std::runtime_error("Assertion failed.");
        }
    };

    void StartTimer()
    {
        if constexpr (!IS_DEBUG_MODE)
            return;

        StartTime = std::chrono::high_resolution_clock::now();
    }

    void EndTimer()
    {
        if constexpr (!IS_DEBUG_MODE)
            return;

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - StartTime);
        
        Log("Exec: " + std::to_string(duration.count()), LogLevel::Info, true);
    }
}
