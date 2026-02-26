#include "Framework.h"
#include "Scripting/Lua.h"

namespace Silent::Scripting
{
    /*LuaManager::LuaManager(sol::state& state)
    {
        _state = &state;
    }

    sol::state& LuaManager::GetState()
    {
        return *_state;
    };

    void LuaManager::ExecuteScript(const std::filesystem::path& file)
    {
        /*if (!std::filesystem::is_regular_file(file))
        {
            return;
        }

        auto result = _state->safe_script_file(file.string(), sol::script_pass_on_error);
        if (!result.valid())
        {
            //auto error = (sol::error)result;
            //throw std::runtime_error(error.what());
        }*/
    //}
};
