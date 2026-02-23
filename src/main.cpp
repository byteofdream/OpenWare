// OpenWare minimal RenderWare-style demo.
// Creates a scene with low-poly cubes, a free camera and directional lighting.

#include <SDL.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/GameState.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Physics/Rigidbody.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Renderer/GL.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Resource/MaterialLoader.hpp"
#include "Engine/Resource/OBJLoader.hpp"
#include "Engine/Scene/Camera.hpp"
#include "Engine/Scene/Entity.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Engine/Script/LuaScriptSystem.hpp"
#include "Engine/UI/DebugUI.hpp"

namespace {

const char* kVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormal;
out vec2 vUV;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vUV = aUV;
    gl_Position = uProjection * uView * worldPos;
}
)";

const char* kFragmentShader = R"(
#version 330 core
in vec3 vNormal;
in vec2 vUV;

uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uColor;
uniform vec3 uEmissiveColor;

uniform sampler2D uTextureAlbedo;
uniform sampler2D uTextureEmissive;

uniform int uUseAlbedoTexture;
uniform int uUseEmissiveTexture;
uniform int uShadeSteps;
uniform float uRoughness;
uniform float uEmissiveStrength;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(vNormal);
    float ndl = max(dot(normal, -normalize(uLightDir)), 0.0);

    float roughness = uRoughness;
    float emissiveStrength = uEmissiveStrength;

    float ambient = mix(0.25, 0.42, clamp(roughness, 0.0, 1.0));
    float diffuse = (1.0 - roughness * 0.35) * ndl;

    vec3 base = uColor;
    if (uUseAlbedoTexture == 1) {
        base *= texture(uTextureAlbedo, vUV).rgb;
    }

    float lightBand = ambient + diffuse;
    float steps = max(float(uShadeSteps), 1.0);
    lightBand = floor(lightBand * steps) / steps;

    vec3 emissive = uEmissiveColor * emissiveStrength;
    if (uUseEmissiveTexture == 1) {
        emissive *= texture(uTextureEmissive, vUV).rgb;
    }

    vec3 lit = base * lightBand * uLightColor + emissive;
    FragColor = vec4(lit, 1.0);
}
)";

std::shared_ptr<ow::Material> CreateFallbackMaterial(
    const std::shared_ptr<ow::Shader>& shader,
    const ow::Vec3& color,
    float roughness,
    const ow::Vec3& emissive,
    float emissiveStrength) {
    auto material = std::make_shared<ow::Material>();
    material->shader = shader;
    material->color = color;
    material->roughness = roughness;
    material->emissiveColor = emissive;
    material->emissiveStrength = emissiveStrength;
    return material;
}

std::string ResolveAssetPath(const std::string& relativePath) {
    namespace fs = std::filesystem;
    const fs::path direct(relativePath);
    if (fs::exists(direct)) {
        return direct.string();
    }

    const fs::path parent = fs::path("..") / relativePath;
    if (fs::exists(parent)) {
        return parent.string();
    }

    return relativePath;
}

} // namespace

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
        return EXIT_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow(
        "OpenWare - Retro RenderWare Style",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create GL context: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);

    ow::Input::Init();
    ow::Input::SetRelativeMouseMode(true);

    auto shader = std::make_shared<ow::Shader>();
    if (!shader->Compile(kVertexShader, kFragmentShader)) {
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    const bool audioAvailable = ow::AudioSystem::Init();
    ow::AudioClip jumpSfx{};
    ow::AudioMusic bgm{};
    if (audioAvailable) {
        jumpSfx = ow::AudioSystem::LoadWav(ResolveAssetPath("assets/audio/jump.wav"));
        bgm = ow::AudioSystem::LoadMusic(ResolveAssetPath("assets/audio/music.wav"));
        if (bgm.native) {
            ow::AudioSystem::PlayMusicLoop(bgm);
        }
    }

    ow::LuaScriptSystem scriptSystem;
    const bool scriptingAvailable = scriptSystem.Init();
    if (scriptingAvailable) {
        scriptSystem.LoadScript(ResolveAssetPath("assets/scripts/game.lua"));
    }

    auto cubeMesh = ow::Mesh::CreateCube(0.5f);
    auto objMesh = ow::OBJLoader::Load(ResolveAssetPath("assets/cube.obj"));
    if (!objMesh) {
        objMesh = cubeMesh;
    }

    auto matColor = ow::MaterialLoader::Load(ResolveAssetPath("assets/materials/ground.mat"), shader);
    if (!matColor) {
        matColor = CreateFallbackMaterial(shader, ow::Vec3{0.76f, 0.62f, 0.44f}, 0.85f, ow::Vec3{0.0f, 0.0f, 0.0f}, 0.0f);
    }

    auto matTextured = ow::MaterialLoader::Load(ResolveAssetPath("assets/materials/hero.mat"), shader);
    if (!matTextured) {
        matTextured = CreateFallbackMaterial(shader, ow::Vec3{0.95f, 0.90f, 0.76f}, 0.55f, ow::Vec3{0.08f, 0.06f, 0.02f}, 0.3f);
    }

    // --- GAME CODE AREA: Scene setup (spawn your game objects here) ---
    ow::Scene scene;
    scene.light.direction = ow::Vec3{-0.35f, -1.0f, -0.25f};
    scene.light.color = ow::Vec3{1.0f, 0.94f, 0.86f};

    for (int z = -2; z <= 2; ++z) {
        for (int x = -2; x <= 2; ++x) {
            auto e = std::make_shared<ow::Entity>("Cube");
            e->mesh = cubeMesh;
            e->material = ((x + z) % 2 == 0) ? matColor : matTextured;
            e->transform.position = ow::Vec3{static_cast<float>(x) * 1.5f, 0.0f, static_cast<float>(z) * 1.5f};
            e->transform.scale = ow::Vec3{1.0f, 1.0f, 1.0f};
            e->colliderRadius = 0.75f;
            e->rigidbody = std::make_shared<ow::Rigidbody>();
            e->rigidbody->isStatic = true;
            e->rigidbody->useGravity = false;
            scene.AddEntity(e);
        }
    }

    auto hero = std::make_shared<ow::Entity>("HeroOBJ");
    hero->mesh = objMesh;
    hero->material = matTextured;
    hero->transform.position = ow::Vec3{0.0f, 3.2f, 0.0f};
    hero->transform.scale = ow::Vec3{1.2f, 1.2f, 1.2f};
    hero->colliderRadius = 0.9f;
    hero->rigidbody = std::make_shared<ow::Rigidbody>();
    hero->rigidbody->SetMass(1.25f);
    hero->rigidbody->restitution = 0.15f;
    hero->rigidbody->linearDamping = 0.995f;
    scene.AddEntity(hero);

    ow::Camera camera;
    ow::Renderer renderer;
    ow::DebugUI debugUi;
    if (!debugUi.Init()) {
        std::cerr << "Failed to initialize debug UI\n";
        scriptSystem.Shutdown();
        ow::AudioSystem::Shutdown();
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    ow::GameStateMachine gameState;

    bool running = true;
    bool settingsOpen = false;
    bool prevSettingsOpen = false;
    bool appliedVsync = true;
    bool musicPlaying = bgm.native != nullptr;

    const float fixedDeltaTime = 1.0f / 60.0f;
    float accumulator = 0.0f;
    float simulationTime = 0.0f;
    float lastTime = static_cast<float>(SDL_GetTicks()) * 0.001f;

    while (running) {
        // --- GAME CODE AREA: Per-frame update (rules, AI, gameplay logic) ---
        const float now = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float frameDelta = now - lastTime;
        lastTime = now;

        if (frameDelta > 0.25f) {
            frameDelta = 0.25f;
        }

        ow::Input::BeginFrame();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ow::Input::HandleEvent(event);
            debugUi.HandleEvent(event, settingsOpen, running);
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_P) {
                gameState.TogglePause();
            }
        }

        if (settingsOpen != prevSettingsOpen) {
            ow::Input::SetRelativeMouseMode(!settingsOpen);
            prevSettingsOpen = settingsOpen;
        }

        const float moveSpeed = 4.0f * frameDelta;
        if (!settingsOpen && gameState.IsPlaying()) {
            if (ow::Input::KeyDown(SDL_SCANCODE_W)) camera.position += camera.Front() * moveSpeed;
            if (ow::Input::KeyDown(SDL_SCANCODE_S)) camera.position += camera.Front() * -moveSpeed;
            if (ow::Input::KeyDown(SDL_SCANCODE_A)) camera.position += camera.Right() * -moveSpeed;
            if (ow::Input::KeyDown(SDL_SCANCODE_D)) camera.position += camera.Right() * moveSpeed;
            if (ow::Input::KeyDown(SDL_SCANCODE_Q)) camera.position += ow::Vec3{0.0f, -moveSpeed, 0.0f};
            if (ow::Input::KeyDown(SDL_SCANCODE_E)) camera.position += ow::Vec3{0.0f, moveSpeed, 0.0f};

            if (ow::Input::KeyDown(SDL_SCANCODE_SPACE) && hero->rigidbody) {
                hero->rigidbody->AddImpulse(ow::Vec3{0.0f, 8.0f, 0.0f});
                ow::AudioSystem::PlaySfx(jumpSfx);
            }

            camera.ProcessMouse(ow::Input::MouseDeltaX(), ow::Input::MouseDeltaY());
        }

        if (gameState.IsPlaying()) {
            accumulator += frameDelta;
            while (accumulator >= fixedDeltaTime) {
                const float scriptedImpulse = scriptSystem.CallNumberFunction("ComputeHeroImpulse", fixedDeltaTime, simulationTime, 0.0f);
                if (hero->rigidbody && scriptedImpulse != 0.0f) {
                    hero->rigidbody->AddImpulse(ow::Vec3{0.0f, scriptedImpulse, 0.0f});
                }

                ow::PhysicsSystem::Step(scene, fixedDeltaTime, 2);
                accumulator -= fixedDeltaTime;
                simulationTime += fixedDeltaTime;
            }
        }

        debugUi.Tick(frameDelta);

        if (scene.entities.size() > 1) {
            const bool collides = ow::PhysicsSystem::CheckSphereCollision(*scene.entities[0], *hero);
            std::string title = collides ? "OpenWare - Collision: YES" : "OpenWare - Collision: NO";
            if (gameState.IsPaused()) {
                title += " [PAUSED]";
            }
            SDL_SetWindowTitle(window, title.c_str());
        }

        int width = 0;
        int height = 0;
        SDL_GL_GetDrawableSize(window, &width, &height);
        debugUi.SetViewport(width, height);

        const ow::RenderSettings& settings = debugUi.Settings();
        if (settings.vSync != appliedVsync) {
            SDL_GL_SetSwapInterval(settings.vSync ? 1 : 0);
            appliedVsync = settings.vSync;
        }

        if (audioAvailable && bgm.native) {
            if (settings.musicEnabled && !musicPlaying) {
                ow::AudioSystem::PlayMusicLoop(bgm);
                musicPlaying = true;
            } else if (!settings.musicEnabled && musicPlaying) {
                ow::AudioSystem::StopMusic();
                musicPlaying = false;
            }
        }

        renderer.Render(scene, camera, width, height, settings);
        debugUi.Render(settingsOpen);

        SDL_GL_SwapWindow(window);
    }

    auto releaseMaterialTextures = [](const std::shared_ptr<ow::Material>& material) {
        if (!material) {
            return;
        }

        std::unordered_set<unsigned int> textures;
        if (material->albedoTextureId != 0) {
            textures.insert(material->albedoTextureId);
        }
        if (material->emissiveTextureId != 0) {
            textures.insert(material->emissiveTextureId);
        }

        for (unsigned int tex : textures) {
            glDeleteTextures(1, &tex);
        }
    };

    releaseMaterialTextures(matColor);
    releaseMaterialTextures(matTextured);

    ow::AudioSystem::FreeClip(jumpSfx);
    ow::AudioSystem::StopMusic();
    ow::AudioSystem::FreeMusic(bgm);
    ow::AudioSystem::Shutdown();
    scriptSystem.Shutdown();

    debugUi.Shutdown();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
