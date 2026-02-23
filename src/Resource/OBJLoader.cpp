#include "Engine/Resource/OBJLoader.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Engine/Core/Math.hpp"
#include "Engine/Renderer/Mesh.hpp"

namespace ow {

namespace {

struct IndexTriplet {
    int pos = 0;
    int uv = 0;
    int normal = 0;
};

IndexTriplet ParseFaceVertex(const std::string& token) {
    IndexTriplet result{};
    std::stringstream ss(token);
    std::string part;

    if (std::getline(ss, part, '/')) {
        result.pos = part.empty() ? 0 : std::stoi(part);
    }
    if (std::getline(ss, part, '/')) {
        result.uv = part.empty() ? 0 : std::stoi(part);
    }
    if (std::getline(ss, part, '/')) {
        result.normal = part.empty() ? 0 : std::stoi(part);
    }

    return result;
}

std::string BuildKey(const IndexTriplet& idx) {
    return std::to_string(idx.pos) + "/" + std::to_string(idx.uv) + "/" + std::to_string(idx.normal);
}

} // namespace

std::shared_ptr<Mesh> OBJLoader::Load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return nullptr;
    }

    std::vector<Vec3> positions;
    std::vector<Vec2> uvs;
    std::vector<Vec3> normals;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<std::string, unsigned int> vertexMap;

    positions.reserve(1024);
    uvs.reserve(1024);
    normals.reserve(1024);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string head;
        ss >> head;

        if (head == "v") {
            Vec3 p{};
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        } else if (head == "vt") {
            Vec2 uv{};
            ss >> uv.x >> uv.y;
            uvs.push_back(uv);
        } else if (head == "vn") {
            Vec3 n{};
            ss >> n.x >> n.y >> n.z;
            normals.push_back(Normalize(n));
        } else if (head == "f") {
            std::vector<std::string> faceTokens;
            std::string token;
            while (ss >> token) {
                faceTokens.push_back(token);
            }

            if (faceTokens.size() < 3) {
                continue;
            }

            // Triangulate n-gons as a fan to keep loader simple.
            for (std::size_t i = 1; i + 1 < faceTokens.size(); ++i) {
                const std::string tri[3] = {faceTokens[0], faceTokens[i], faceTokens[i + 1]};

                for (const auto& triToken : tri) {
                    const IndexTriplet idx = ParseFaceVertex(triToken);
                    const std::string key = BuildKey(idx);

                    const auto it = vertexMap.find(key);
                    if (it != vertexMap.end()) {
                        indices.push_back(it->second);
                        continue;
                    }

                    Vertex v{};
                    if (idx.pos > 0 && static_cast<std::size_t>(idx.pos) <= positions.size()) {
                        v.position = positions[static_cast<std::size_t>(idx.pos - 1)];
                    }
                    if (idx.uv > 0 && static_cast<std::size_t>(idx.uv) <= uvs.size()) {
                        v.uv = uvs[static_cast<std::size_t>(idx.uv - 1)];
                    }
                    if (idx.normal > 0 && static_cast<std::size_t>(idx.normal) <= normals.size()) {
                        v.normal = normals[static_cast<std::size_t>(idx.normal - 1)];
                    } else {
                        v.normal = Vec3{0.0f, 1.0f, 0.0f};
                    }

                    const unsigned int newIndex = static_cast<unsigned int>(vertices.size());
                    vertices.push_back(v);
                    vertexMap.emplace(key, newIndex);
                    indices.push_back(newIndex);
                }
            }
        }
    }

    if (vertices.empty() || indices.empty()) {
        return nullptr;
    }

    return std::make_shared<Mesh>(vertices, indices);
}

} // namespace ow
