#pragma once

// Input module: wraps SDL keyboard and mouse state for camera control.

union SDL_Event;

namespace ow {

class Input {
public:
    static void Init();
    static void BeginFrame();
    static void HandleEvent(const SDL_Event& event);

    static bool KeyDown(int scancode);
    static float MouseDeltaX();
    static float MouseDeltaY();
    static void SetRelativeMouseMode(bool enabled);

private:
    static float mouseDeltaX_;
    static float mouseDeltaY_;
};

} // namespace ow
