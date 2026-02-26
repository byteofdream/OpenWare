#include "Engine/UI/DebugUI.hpp"

#include "Engine/Renderer/GL.hpp"

#include <SDL.h>

#include <cstdio>
#include <cstring>
#include <iostream>

namespace ow {

namespace {

struct Glyph5x7 {
    char c;
    unsigned char rows[7];
};

constexpr Glyph5x7 kGlyphs[] = {
    {' ', {0, 0, 0, 0, 0, 0, 0}},
    {'-', {0, 0, 0, 31, 0, 0, 0}},
    {'.', {0, 0, 0, 0, 0, 12, 12}},
    {':', {0, 12, 12, 0, 12, 12, 0}},
    {'|', {4, 4, 4, 4, 4, 4, 4}},
    {'0', {14, 17, 19, 21, 25, 17, 14}},
    {'1', {4, 12, 4, 4, 4, 4, 14}},
    {'2', {14, 17, 1, 2, 4, 8, 31}},
    {'3', {30, 1, 1, 14, 1, 1, 30}},
    {'4', {2, 6, 10, 18, 31, 2, 2}},
    {'5', {31, 16, 16, 30, 1, 1, 30}},
    {'6', {14, 16, 16, 30, 17, 17, 14}},
    {'7', {31, 1, 2, 4, 8, 8, 8}},
    {'8', {14, 17, 17, 14, 17, 17, 14}},
    {'9', {14, 17, 17, 15, 1, 1, 14}},
    {'A', {14, 17, 17, 31, 17, 17, 17}},
    {'B', {30, 17, 17, 30, 17, 17, 30}},
    {'C', {14, 17, 16, 16, 16, 17, 14}},
    {'D', {30, 17, 17, 17, 17, 17, 30}},
    {'E', {31, 16, 16, 30, 16, 16, 31}},
    {'F', {31, 16, 16, 30, 16, 16, 16}},
    {'G', {14, 17, 16, 16, 19, 17, 14}},
    {'H', {17, 17, 17, 31, 17, 17, 17}},
    {'I', {14, 4, 4, 4, 4, 4, 14}},
    {'K', {17, 18, 20, 24, 20, 18, 17}},
    {'L', {16, 16, 16, 16, 16, 16, 31}},
    {'M', {17, 27, 21, 21, 17, 17, 17}},
    {'N', {17, 25, 21, 19, 17, 17, 17}},
    {'O', {14, 17, 17, 17, 17, 17, 14}},
    {'P', {30, 17, 17, 30, 16, 16, 16}},
    {'Q', {14, 17, 17, 17, 21, 18, 13}},
    {'R', {30, 17, 17, 30, 20, 18, 17}},
    {'S', {14, 17, 16, 14, 1, 17, 14}},
    {'T', {31, 4, 4, 4, 4, 4, 4}},
    {'U', {17, 17, 17, 17, 17, 17, 14}},
    {'V', {17, 17, 17, 17, 17, 10, 4}},
    {'W', {17, 17, 17, 21, 21, 21, 10}},
    {'X', {17, 17, 10, 4, 10, 17, 17}},
    {'Y', {17, 17, 10, 4, 4, 4, 4}},
};

unsigned int CompileUiProgram() {
    const char* vertexSrc = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
out vec4 vColor;
void main() {
    vColor = aColor;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

    const char* fragmentSrc = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() {
    FragColor = vColor;
}
)";

    const unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, nullptr);
    glCompileShader(vs);

    int ok = GL_FALSE;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (ok != GL_TRUE) {
        char info[512]{};
        glGetShaderInfoLog(vs, sizeof(info), nullptr, info);
        std::cerr << "UI vertex shader compile failed: " << info << '\n';
        glDeleteShader(vs);
        return 0;
    }

    const unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (ok != GL_TRUE) {
        char info[512]{};
        glGetShaderInfoLog(fs, sizeof(info), nullptr, info);
        std::cerr << "UI fragment shader compile failed: " << info << '\n';
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    const unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (ok != GL_TRUE) {
        char info[512]{};
        glGetProgramInfoLog(program, sizeof(info), nullptr, info);
        std::cerr << "UI shader link failed: " << info << '\n';
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

} // namespace

DebugUI::~DebugUI() {
    Shutdown();
}

bool DebugUI::Init() {
    shaderProgram_ = CompileUiProgram();
    if (shaderProgram_ == 0) {
        return false;
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    vertexCapacity_ = 16384;
    vertices_ = new float[vertexCapacity_ * 6];
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertexCapacity_ * 6 * static_cast<int>(sizeof(float))), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, reinterpret_cast<void*>(sizeof(float) * 2));

    glBindVertexArray(0);
    return true;
}

void DebugUI::Shutdown() {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (shaderProgram_ != 0) {
        glDeleteProgram(shaderProgram_);
        shaderProgram_ = 0;
    }
    delete[] vertices_;
    vertices_ = nullptr;
    vertexCount_ = 0;
    vertexCapacity_ = 0;
}

void DebugUI::SetViewport(int width, int height) {
    viewportWidth_ = width > 0 ? width : 1;
    viewportHeight_ = height > 0 ? height : 1;
}

void DebugUI::Tick(float deltaTime) {
    if (deltaTime <= 0.000001f) {
        return;
    }

    frameMs_ = deltaTime * 1000.0f;
    const float instantFps = 1.0f / deltaTime;
    if (fps_ <= 0.0f) {
        fps_ = instantFps;
    } else {
        fps_ = fps_ * 0.9f + instantFps * 0.1f;
    }
}

void DebugUI::HandleEvent(const SDL_Event& event, bool& settingsOpen, bool& running) {
    if (event.type != SDL_KEYDOWN) {
        return;
    }

    const SDL_Scancode key = event.key.keysym.scancode;

    if (key == SDL_SCANCODE_F10) {
        running = false;
        return;
    }

    if (key == SDL_SCANCODE_F1) {
        showDebug_ = !showDebug_;
        return;
    }

    if (key == SDL_SCANCODE_F2) {
        aboutOpen_ = !aboutOpen_;
        return;
    }

    if (key == SDL_SCANCODE_ESCAPE) {
        settingsOpen = !settingsOpen;
        return;
    }

    if (!settingsOpen) {
        return;
    }

    if (key == SDL_SCANCODE_1) {
        settings_.wireframe = !settings_.wireframe;
    } else if (key == SDL_SCANCODE_2) {
        settings_.vSync = !settings_.vSync;
    } else if (key == SDL_SCANCODE_3) {
        if (settings_.shadeSteps == 2) {
            settings_.shadeSteps = 4;
        } else if (settings_.shadeSteps == 4) {
            settings_.shadeSteps = 8;
        } else {
            settings_.shadeSteps = 2;
        }
    } else if (key == SDL_SCANCODE_4) {
        settings_.musicEnabled = !settings_.musicEnabled;
    } else if (key == SDL_SCANCODE_5) {
        settings_.ps2Aesthetic = !settings_.ps2Aesthetic;
    } else if (key == SDL_SCANCODE_6) {
        if (settings_.ps2ColorLevels == 8) {
            settings_.ps2ColorLevels = 12;
        } else if (settings_.ps2ColorLevels == 12) {
            settings_.ps2ColorLevels = 20;
        } else {
            settings_.ps2ColorLevels = 8;
        }
    } else if (key == SDL_SCANCODE_7) {
        if (settings_.ps2Jitter < 0.1f) {
            settings_.ps2Jitter = 0.6f;
        } else if (settings_.ps2Jitter < 0.8f) {
            settings_.ps2Jitter = 1.1f;
        } else if (settings_.ps2Jitter < 1.3f) {
            settings_.ps2Jitter = 1.8f;
        } else {
            settings_.ps2Jitter = 0.0f;
        }
    } else if (key == SDL_SCANCODE_8) {
        if (settings_.ps2FogStrength < 0.3f) {
            settings_.ps2FogStrength = 0.82f;
        } else if (settings_.ps2FogStrength < 0.8f) {
            settings_.ps2FogStrength = 1.0f;
        } else {
            settings_.ps2FogStrength = 0.18f;
        }
    }
}

void DebugUI::AppendRect(float x, float y, float w, float h, float r, float g, float b, float a) const {
    if (vertexCount_ + 6 >= vertexCapacity_) {
        return;
    }

    const float l = (x / static_cast<float>(viewportWidth_)) * 2.0f - 1.0f;
    const float rPos = ((x + w) / static_cast<float>(viewportWidth_)) * 2.0f - 1.0f;
    const float t = 1.0f - (y / static_cast<float>(viewportHeight_)) * 2.0f;
    const float bPos = 1.0f - ((y + h) / static_cast<float>(viewportHeight_)) * 2.0f;

    auto push = [&](float px, float py) {
        const int idx = vertexCount_ * 6;
        vertices_[idx + 0] = px;
        vertices_[idx + 1] = py;
        vertices_[idx + 2] = r;
        vertices_[idx + 3] = g;
        vertices_[idx + 4] = b;
        vertices_[idx + 5] = a;
        ++vertexCount_;
    };

    push(l, t);
    push(rPos, t);
    push(rPos, bPos);

    push(l, t);
    push(rPos, bPos);
    push(l, bPos);
}

const unsigned char* DebugUI::Glyph(char c) {
    if (c >= 'a' && c <= 'z') {
        c = static_cast<char>(c - ('a' - 'A'));
    }

    for (const auto& glyph : kGlyphs) {
        if (glyph.c == c) {
            return glyph.rows;
        }
    }

    return kGlyphs[0].rows;
}

void DebugUI::AppendText(float x, float y, float scale, const std::string& text, float r, float g, float b, float a) const {
    const float pixel = scale;
    float cursor = x;

    for (char c : text) {
        const unsigned char* rows = Glyph(c);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                const bool on = (rows[row] & (1 << (4 - col))) != 0;
                if (!on) {
                    continue;
                }
                AppendRect(cursor + static_cast<float>(col) * pixel,
                           y + static_cast<float>(row) * pixel,
                           pixel,
                           pixel,
                           r, g, b, a);
            }
        }
        cursor += pixel * 6.0f;
    }
}

void DebugUI::Render(bool settingsOpen) const {
    vertexCount_ = 0;

    if (showDebug_) {
        AppendRect(12.0f, 12.0f, 390.0f, 92.0f, 0.05f, 0.08f, 0.12f, 0.72f);

        char line1[64]{};
        char line2[64]{};
        std::snprintf(line1, sizeof(line1), "FPS %.1f", fps_);
        std::snprintf(line2, sizeof(line2), "FRAME %.2f MS", frameMs_);
        AppendText(22.0f, 24.0f, 2.0f, line1, 0.92f, 0.96f, 1.0f, 1.0f);
        AppendText(22.0f, 48.0f, 2.0f, line2, 0.78f, 0.89f, 0.98f, 1.0f);
        AppendText(22.0f, 72.0f, 2.0f, "ESC SETTINGS | F2 ABOUT | F10 EXIT", 0.95f, 0.83f, 0.58f, 1.0f);
    }

    if (settingsOpen) {
        const float panelW = 560.0f;
        const float panelH = 330.0f;
        const float px = (static_cast<float>(viewportWidth_) - panelW) * 0.5f;
        const float py = (static_cast<float>(viewportHeight_) - panelH) * 0.5f;

        AppendRect(0.0f, 0.0f, static_cast<float>(viewportWidth_), static_cast<float>(viewportHeight_), 0.01f, 0.01f, 0.01f, 0.45f);
        AppendRect(px, py, panelW, panelH, 0.08f, 0.11f, 0.14f, 0.92f);

        AppendText(px + 24.0f, py + 22.0f, 3.0f, "SETTINGS", 1.0f, 0.94f, 0.78f, 1.0f);
        AppendText(px + 24.0f, py + 70.0f, 2.0f, settings_.wireframe ? "1 WIREFRAME ON" : "1 WIREFRAME OFF", 0.90f, 0.97f, 1.0f, 1.0f);
        AppendText(px + 24.0f, py + 98.0f, 2.0f, settings_.vSync ? "2 VSYNC ON" : "2 VSYNC OFF", 0.90f, 0.97f, 1.0f, 1.0f);

        char shadeLine[64]{};
        std::snprintf(shadeLine, sizeof(shadeLine), "3 SHADE STEPS %d", settings_.shadeSteps);
        AppendText(px + 24.0f, py + 126.0f, 2.0f, shadeLine, 0.90f, 0.97f, 1.0f, 1.0f);
        AppendText(px + 24.0f, py + 154.0f, 2.0f, settings_.musicEnabled ? "4 MUSIC ON" : "4 MUSIC OFF", 0.90f, 0.97f, 1.0f, 1.0f);

        AppendText(px + 24.0f, py + 182.0f, 2.0f, settings_.ps2Aesthetic ? "5 PS2 MODE ON" : "5 PS2 MODE OFF", 0.90f, 0.97f, 1.0f, 1.0f);

        char colorLine[64]{};
        std::snprintf(colorLine, sizeof(colorLine), "6 COLOR LEVELS %d", settings_.ps2ColorLevels);
        AppendText(px + 24.0f, py + 210.0f, 2.0f, colorLine, 0.90f, 0.97f, 1.0f, 1.0f);

        char jitterLine[64]{};
        std::snprintf(jitterLine, sizeof(jitterLine), "7 JITTER %.1f", settings_.ps2Jitter);
        AppendText(px + 24.0f, py + 238.0f, 2.0f, jitterLine, 0.90f, 0.97f, 1.0f, 1.0f);

        char fogLine[64]{};
        std::snprintf(fogLine, sizeof(fogLine), "8 FOG %.2f", settings_.ps2FogStrength);
        AppendText(px + 24.0f, py + 266.0f, 2.0f, fogLine, 0.90f, 0.97f, 1.0f, 1.0f);

        AppendText(px + 24.0f, py + 296.0f, 2.0f, "ESC CLOSE | F1 HIDE HUD", 0.96f, 0.84f, 0.61f, 1.0f);
    }

    if (aboutOpen_) {
        const float panelW = 860.0f;
        const float panelH = 360.0f;
        const float px = (static_cast<float>(viewportWidth_) - panelW) * 0.5f;
        const float py = (static_cast<float>(viewportHeight_) - panelH) * 0.5f;

        AppendRect(0.0f, 0.0f, static_cast<float>(viewportWidth_), static_cast<float>(viewportHeight_), 0.01f, 0.01f, 0.01f, 0.35f);
        AppendRect(px, py, panelW, panelH, 0.09f, 0.12f, 0.10f, 0.95f);
        AppendText(px + 24.0f, py + 22.0f, 3.0f, aboutUi_.Title(), 1.0f, 0.94f, 0.78f, 1.0f);

        float lineY = py + 84.0f;
        for (int i = 0; i < aboutUi_.LineCount(); ++i) {
            AppendText(px + 24.0f, lineY, 2.0f, aboutUi_.Line(i), 0.88f, 0.96f, 0.91f, 1.0f);
            lineY += 28.0f;
        }
        AppendText(px + 24.0f, py + panelH - 34.0f, 2.0f, "F2 CLOSE ABOUT", 0.96f, 0.84f, 0.61f, 1.0f);
    }

    if (vertexCount_ <= 0) {
        return;
    }

    // UI should always render on top of 3D geometry.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<long>(vertexCount_ * 6 * static_cast<int>(sizeof(float))), vertices_);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

} // namespace ow
