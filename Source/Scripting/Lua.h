#pragma once

namespace Silent::Scripting
{
    /*class LuaManager
    {
    private:
        // =======
        // Fields
        // =======

        sol::state* _state   = nullptr;
        sol::table  _globals = {};

    public:
        // =============
        // Constructors
        // =============

        LuaManager(sol::state& state);

        // ========
        // Getters
        // ========

        sol::state& GetState();

        // ==========
        // Utilities
        // ==========

        void ExecuteScript(const std::filesystem::path& filename);

        template <typename T>
        void CreateReadOnlyTable(sol::table parent, const std::string& tableName, const T& cont);
    };*/

    /*template <typename T>
    void LuaManager::CreateReadOnlyTable(sol::table parent, const std::string& tableName, const T& cont)
    {
        // Insert all data in metatable.
        auto metatable = tableName + "Meta";
        _state->set(metatable, sol::as_table(cont));

        // Create clean copy of data for iteration before adding metamethods and store it in special metatable field.
        _state->safe_script(Fmt("{}.__data ", metatable));
        _state->safe_script(Fmt("{}.__data = {{}} for k, v in pairs({}) do if type(k) == 'string' and k:sub(1, 2) ~= '__' then {}.__data[k] = v end end",
                                metatable, metatable, metatable));

        auto metaMethod      = tableName + "MetaMeta";
        auto metaMethodTable = _state->create_named_table(metaMethod);

        // Create metatable metatable's `__index` fail assert to generate warning/error when trying to use missing variable.
        auto lassertMethod = [tableName](sol::table table, const std::string& key)
        {
            //ScriptAssertF(false, Fmt("`{}` has no member `{}`.", tableName, key));
        };

        metaMethodTable[sol::meta_method::index] = lassertMethod;
        _state->safe_script(Fmt("setmetatable({}, {})", metatable, metaMethod));

        // Make metatable's `__index` refer to itself to ensure requests to main table go through to metatable, and thus container's elements.
        _state->safe_script(Fmt("{}.__index = {}", metatable, metatable));

        _state->safe_script(metatable + ".__type = 'readonly'");

        // Disallow adding new elements to table.
        _state->safe_script(Fmt("{}.__newindex = function() error('{} is read-only') end", metatable, tableName));

        // Add `__pairs` metamethod to enable iteration with `pairs`.
        // Use clean data copy stored in `__data` field.
        // `debug.getmetatable` is used to bypass `__metatable` protection.
        _state->safe_script(Fmt("{}.__pairs = function(t) return pairs(debug.getmetatable(t).__data) end", metatable));

        // Protect metatable.
        _state->safe_script(Fmt("{}.__metatable = 'metatable is protected.'", metatable));

        auto table = _state->create_named_table(tableName);

        _state->safe_script(Fmt("setmetatable({}, {})", tableName, metatable));

        // Point initial metatable variable away from its contents for cleanliness.
        parent.set(tableName, table);

        _state->safe_script(Fmt("{} = nil", tableName));
        _state->safe_script(Fmt("{} = nil", metatable));
        _state->safe_script(Fmt("{} = nil", metaMethod));
    }*/
}
