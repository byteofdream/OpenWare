#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Renderer/GL.hpp"

#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Scene/Camera.hpp"
#include "Engine/Scene/Entity.hpp"
#include "Engine/Scene/Scene.hpp"

namespace ow {

void Renderer::Render(const Scene& scene, const Camera& camera, int width, int height, const RenderSettings& settings) const {
    if (height == 0) {
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, settings.wireframe ? GL_LINE : GL_FILL);

    glViewport(0, 0, width, height);
    glClearColor(0.72f, 0.78f, 0.86f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const Mat4 view = camera.ViewMatrix();
    const Mat4 projection = camera.ProjectionMatrix(aspect);

    for (const auto& entity : scene.entities) {
        if (!entity || !entity->mesh || !entity->material || !entity->material->shader) {
            continue;
        }

        auto& material = *entity->material;
        auto& shader = *material.shader;

        shader.Use();
        shader.SetMat4("uModel", entity->transform.Matrix());
        shader.SetMat4("uView", view);
        shader.SetMat4("uProjection", projection);
        shader.SetVec3("uLightDir", Normalize(scene.light.direction));
        shader.SetVec3("uLightColor", scene.light.color);
        shader.SetVec3("uViewPos", camera.position);
        shader.SetVec3("uColor", material.color);
        shader.SetVec3("uEmissiveColor", material.emissiveColor);
        shader.SetInt("uUseAlbedoTexture", material.useAlbedoTexture ? 1 : 0);
        shader.SetInt("uUseEmissiveTexture", material.useEmissiveTexture ? 1 : 0);
        shader.SetInt("uTextureAlbedo", 0);
        shader.SetInt("uTextureEmissive", 1);
        shader.SetFloat("uRoughness", material.roughness);
        shader.SetFloat("uEmissiveStrength", material.emissiveStrength);
        shader.SetInt("uShadeSteps", settings.shadeSteps);

        if (material.useAlbedoTexture && material.albedoTextureId != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.albedoTextureId);
        }

        if (material.useEmissiveTexture && material.emissiveTextureId != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.emissiveTextureId);
        }

        entity->mesh->Draw();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

} // namespace ow
