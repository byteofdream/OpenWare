# Integration Guide

## Where to place your game code

Primary entry point:

- `src/main.cpp`

Recommended structure:

1. Scene setup: spawn entities, set materials and rigidbodies.
2. Frame update: input, camera, UI.
3. Fixed update: gameplay simulation and physics.

## Add your own material

1. Create a `.mat` file in `assets/materials/`.
2. Load with `ow::MaterialLoader::Load(path, shader)`.
3. Assign to entity via `entity->material`.

## Add Lua gameplay logic

1. Write script in `assets/scripts/`.
2. Add numeric callback function (e.g. `ComputeHeroImpulse`).
3. Call `LuaScriptSystem::CallNumberFunction(...)` inside fixed loop.

## Add audio SFX

1. Put `.wav` files into `assets/audio/`.
2. Load with `ow::AudioSystem::LoadWav(...)`.
3. Trigger with `ow::AudioSystem::PlaySfx(...)`.

## Build note

Current expected build directory:

- `build_ninja`

Commands:

```bash
cmake -S . -B build_ninja -G Ninja
cmake --build build_ninja -j4
```
