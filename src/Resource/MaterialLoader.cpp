#include "Engine/Resource/MaterialLoader.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Resource/TextureLoader.hpp"

namespace ow {

namespace {

std::string Trim(const std::string& value) {
    const auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char c) { return std::isspace(c) != 0; });
    const auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) { return std::isspace(c) != 0; }).base();
    if (begin >= end) {
        return {};
    }
    return std::string(begin, end);
}

bool ParseBool(const std::string& value) {
    const std::string v = Trim(value);
    return v == "1" || v == "true" || v == "TRUE" || v == "yes" || v == "on";
}

Vec3 ParseVec3(const std::string& value, const Vec3& fallback) {
    std::stringstream ss(value);
    Vec3 out = fallback;
    ss >> out.x >> out.y >> out.z;
    return ss.fail() ? fallback : out;
}

float ParseFloat(const std::string& value, float fallback) {
    std::stringstream ss(value);
    float out = fallback;
    ss >> out;
    return ss.fail() ? fallback : out;
}

std::string ResolveMaterialRelativePath(const std::string& materialPath, const std::string& valuePath) {
    namespace fs = std::filesystem;
    const fs::path raw(valuePath);
    if (raw.is_absolute() && fs::exists(raw)) {
        return raw.string();
    }

    const fs::path base = fs::path(materialPath).parent_path();

    // 1) Resolve relative to material file directory.
    const fs::path local = base / raw;
    if (fs::exists(local)) {
        return local.string();
    }

    // 2) Resolve from current working directory as-is.
    if (fs::exists(raw)) {
        return raw.string();
    }

    // 3) Walk parent directories and try to resolve project-like paths (e.g. assets/...).
    fs::path current = base;
    for (int i = 0; i < 8; ++i) {
        const fs::path candidate = current / raw;
        if (fs::exists(candidate)) {
            return candidate.string();
        }
        if (!current.has_parent_path()) {
            break;
        }
        current = current.parent_path();
    }

    return valuePath;
}

} // namespace

std::shared_ptr<Material> MaterialLoader::Load(const std::string& path, const std::shared_ptr<Shader>& shader) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return nullptr;
    }

    auto material = std::make_shared<Material>();
    material->shader = shader;

    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const std::string key = Trim(line.substr(0, separator));
        const std::string value = Trim(line.substr(separator + 1));

        if (key == "color") {
            material->color = ParseVec3(value, material->color);
        } else if (key == "roughness") {
            material->roughness = ParseFloat(value, material->roughness);
        } else if (key == "emissiveColor") {
            material->emissiveColor = ParseVec3(value, material->emissiveColor);
        } else if (key == "emissiveStrength") {
            material->emissiveStrength = ParseFloat(value, material->emissiveStrength);
        } else if (key == "useAlbedoTexture") {
            material->useAlbedoTexture = ParseBool(value);
        } else if (key == "useEmissiveTexture") {
            material->useEmissiveTexture = ParseBool(value);
        } else if (key == "albedoTexture") {
            material->albedoTextureId = TextureLoader::LoadPPM(ResolveMaterialRelativePath(path, value));
        } else if (key == "emissiveTexture") {
            material->emissiveTextureId = TextureLoader::LoadPPM(ResolveMaterialRelativePath(path, value));
        }
    }

    return material;
}

} // namespace ow
