#pragma once

// Core gameplay module: lightweight game state machine for play/pause flow.

namespace ow {

enum class GameState {
    Playing,
    Paused,
};

class GameStateMachine {
public:
    void SetState(GameState next) { state_ = next; }
    void TogglePause() {
        state_ = (state_ == GameState::Playing) ? GameState::Paused : GameState::Playing;
    }

    bool IsPlaying() const { return state_ == GameState::Playing; }
    bool IsPaused() const { return state_ == GameState::Paused; }
    GameState State() const { return state_; }

private:
    GameState state_ = GameState::Playing;
};

} // namespace ow
