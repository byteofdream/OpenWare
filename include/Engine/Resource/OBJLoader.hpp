#pragma once

// Resource module: lightweight OBJ parser for static meshes.

#include <memory>
#include <string>

namespace ow {

class Mesh;

class OBJLoader {
public:
    static std::shared_ptr<Mesh> Load(const std::string& path);
};

} // namespace ow
