#include "Engine/UI/AboutUI.hpp"

namespace ow {

namespace {

constexpr const char* kAboutLines[] = {
    "OPENWARE ENGINE",
    "MINIMAL OPEN SOURCE 3D ENGINE",
    "C17 | OPENGL | SDL2",
    "RETRO LOW POLY RENDERWARE STYLE",
    "MODULES CORE RENDERER SCENE",
    "PHYSICS INPUT RESOURCE UI",
    "DESIGNED FOR EXTENSIONS",
    "PHYSICS AUDIO ANIMATION ECS",
};

} // namespace

const char* AboutUI::Title() const {
    return "ABOUT";
}

int AboutUI::LineCount() const {
    return static_cast<int>(sizeof(kAboutLines) / sizeof(kAboutLines[0]));
}

const char* AboutUI::Line(int index) const {
    if (index < 0 || index >= LineCount()) {
        return "";
    }
    return kAboutLines[index];
}

} // namespace ow
