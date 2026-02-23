#pragma once

// Script module: optional Lua runner for gameplay callbacks.

#include <string>

namespace ow {

class LuaScriptSystem {
public:
    bool Init();
    void Shutdown();

    bool IsAvailable() const { return available_; }
    bool LoadScript(const std::string& path);

    // Calls Lua function(dt, timeSeconds) and returns numeric result.
    float CallNumberFunction(const char* functionName, float dt, float timeSeconds, float fallback = 0.0f);

private:
    bool available_ = false;
    void* luaState_ = nullptr;
};

} // namespace ow
