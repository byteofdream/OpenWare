#pragma once

// Resource module: tiny PPM texture loader that uploads OpenGL 2D textures.

#include <string>

namespace ow {

class TextureLoader {
public:
    static unsigned int LoadPPM(const std::string& path);
};

} // namespace ow
