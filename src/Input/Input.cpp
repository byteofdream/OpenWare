#include "Engine/Input/Input.hpp"

#include <SDL.h>

namespace ow {

float Input::mouseDeltaX_ = 0.0f;
float Input::mouseDeltaY_ = 0.0f;

void Input::Init() {
    mouseDeltaX_ = 0.0f;
    mouseDeltaY_ = 0.0f;
}

void Input::BeginFrame() {
    mouseDeltaX_ = 0.0f;
    mouseDeltaY_ = 0.0f;
}

void Input::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mouseDeltaX_ += static_cast<float>(event.motion.xrel);
        mouseDeltaY_ += static_cast<float>(-event.motion.yrel);
    }
}

bool Input::KeyDown(int scancode) {
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    return state && state[scancode] != 0;
}

float Input::MouseDeltaX() {
    return mouseDeltaX_;
}

float Input::MouseDeltaY() {
    return mouseDeltaY_;
}

void Input::SetRelativeMouseMode(bool enabled) {
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

} // namespace ow
