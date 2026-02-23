#include "Engine/Script/LuaScriptSystem.hpp"

#ifdef OW_ENABLE_LUA
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#endif

namespace ow {

bool LuaScriptSystem::Init() {
#ifdef OW_ENABLE_LUA
    lua_State* L = luaL_newstate();
    if (!L) {
        return false;
    }
    luaL_openlibs(L);
    luaState_ = L;
    available_ = true;
    return true;
#else
    available_ = false;
    luaState_ = nullptr;
    return false;
#endif
}

void LuaScriptSystem::Shutdown() {
#ifdef OW_ENABLE_LUA
    if (luaState_) {
        lua_close(static_cast<lua_State*>(luaState_));
    }
#endif
    luaState_ = nullptr;
    available_ = false;
}

bool LuaScriptSystem::LoadScript(const std::string& path) {
#ifdef OW_ENABLE_LUA
    if (!available_ || !luaState_) {
        return false;
    }
    return luaL_dofile(static_cast<lua_State*>(luaState_), path.c_str()) == LUA_OK;
#else
    (void)path;
    return false;
#endif
}

float LuaScriptSystem::CallNumberFunction(const char* functionName, float dt, float timeSeconds, float fallback) {
#ifdef OW_ENABLE_LUA
    if (!available_ || !luaState_) {
        return fallback;
    }

    lua_State* L = static_cast<lua_State*>(luaState_);
    lua_getglobal(L, functionName);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return fallback;
    }

    lua_pushnumber(L, static_cast<lua_Number>(dt));
    lua_pushnumber(L, static_cast<lua_Number>(timeSeconds));

    if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
        lua_pop(L, 1);
        return fallback;
    }

    if (!lua_isnumber(L, -1)) {
        lua_pop(L, 1);
        return fallback;
    }

    const float result = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);
    return result;
#else
    (void)functionName;
    (void)dt;
    (void)timeSeconds;
    return fallback;
#endif
}

} // namespace ow
