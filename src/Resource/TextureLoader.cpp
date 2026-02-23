#include "Engine/Resource/TextureLoader.hpp"

#include "Engine/Renderer/GL.hpp"

#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace ow {

namespace {

void SkipComments(std::istream& in) {
    while (in >> std::ws && in.peek() == '#') {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

} // namespace

unsigned int TextureLoader::LoadPPM(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return 0;
    }

    std::string magic;
    file >> magic;
    if (magic != "P6") {
        return 0;
    }

    SkipComments(file);
    int width = 0;
    int height = 0;
    file >> width >> height;

    SkipComments(file);
    int maxValue = 0;
    file >> maxValue;

    if (width <= 0 || height <= 0 || maxValue != 255) {
        return 0;
    }

    file.get();

    std::vector<unsigned char> pixels(static_cast<std::size_t>(width * height * 3));
    file.read(reinterpret_cast<char*>(pixels.data()), static_cast<std::streamsize>(pixels.size()));
    if (!file) {
        return 0;
    }

    unsigned int tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

} // namespace ow
