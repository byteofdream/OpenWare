#pragma once

// Resource module: loads simple key=value material files (.mat).

#include <memory>
#include <string>

namespace ow {

class Material;
class Shader;

class MaterialLoader {
public:
    static std::shared_ptr<Material> Load(const std::string& path, const std::shared_ptr<Shader>& shader);
};

} // namespace ow
