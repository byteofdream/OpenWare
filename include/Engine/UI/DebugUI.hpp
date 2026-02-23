#pragma once

// UI module: debug HUD and settings overlay with lightweight built-in bitmap text.

#include <string>

#include "Engine/UI/AboutUI.hpp"

union SDL_Event;

namespace ow {

struct RenderSettings {
    bool wireframe = false;
    bool vSync = true;
    int shadeSteps = 4;
    bool musicEnabled = true;
};

class DebugUI {
public:
    DebugUI() = default;
    ~DebugUI();

    bool Init();
    void Shutdown();

    void SetViewport(int width, int height);
    void Tick(float deltaTime);

    // Handles global debug/settings hotkeys.
    void HandleEvent(const SDL_Event& event, bool& settingsOpen, bool& running);

    // Draws overlay and hints. Should be called after 3D scene render.
    void Render(bool settingsOpen) const;

    const RenderSettings& Settings() const { return settings_; }

private:
    void AppendRect(float x, float y, float w, float h, float r, float g, float b, float a) const;
    void AppendText(float x, float y, float scale, const std::string& text, float r, float g, float b, float a) const;

    static const unsigned char* Glyph(char c);

    mutable unsigned int vao_ = 0;
    mutable unsigned int vbo_ = 0;
    unsigned int shaderProgram_ = 0;

    mutable float* vertices_ = nullptr;
    mutable int vertexCount_ = 0;
    mutable int vertexCapacity_ = 0;

    int viewportWidth_ = 1280;
    int viewportHeight_ = 720;

    float fps_ = 0.0f;
    float frameMs_ = 0.0f;
    bool showDebug_ = true;
    bool aboutOpen_ = false;

    RenderSettings settings_{};
    AboutUI aboutUi_{};
};

} // namespace ow
