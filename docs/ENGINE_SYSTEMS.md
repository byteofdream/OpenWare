# OpenWare Engine Systems

This document summarizes the new gameplay and runtime systems added to OpenWare.

## 1. Gameplay Foundation

### Game State

- `include/Engine/Core/GameState.hpp`
- `ow::GameStateMachine` with states:
  - `Playing`
  - `Paused`

Usage in `src/main.cpp`:

- Press `P` to toggle pause.
- Physics and fixed-step simulation run only in `Playing` state.

### Fixed Timestep Loop

Main loop now uses:

- Variable frame delta for input/camera/UI.
- Fixed simulation step (`1 / 60`) for physics and scripted gameplay.
- Accumulator pattern to avoid unstable physics under frame jitter.

## 2. Material System (.mat)

### Material Data

- `include/Engine/Renderer/Material.hpp`
- Supports:
  - base color
  - roughness
  - emissive color/strength
  - albedo texture slot
  - emissive texture slot

### Material Loader

- `include/Engine/Resource/MaterialLoader.hpp`
- `src/Resource/MaterialLoader.cpp`

Format: key-value text files.

Example keys:

- `color = 0.9 0.8 0.7`
- `roughness = 0.6`
- `useAlbedoTexture = true`
- `albedoTexture = assets/checker.ppm`
- `emissiveColor = 0.1 0.05 0.0`
- `emissiveStrength = 0.3`

Demo material files:

- `assets/materials/ground.mat`
- `assets/materials/hero.mat`

## 3. Audio (SFX)

### Audio System

- `include/Engine/Audio/AudioSystem.hpp`
- `src/Audio/AudioSystem.cpp`

Backed by SDL2_mixer when available.

Behavior:

- If `SDL2_mixer` is found at configure time, audio is enabled (`OW_ENABLE_AUDIO`).
- If not found, stub implementation compiles and no audio plays.

Current usage:

- Space jump trigger attempts to play `assets/audio/jump.wav`.

## 4. Lua Scripting (Optional)

### Script System

- `include/Engine/Script/LuaScriptSystem.hpp`
- `src/Script/LuaScriptSystem.cpp`

Behavior:

- If Lua is found at configure time (`lua5.4` or `lua`), scripting is enabled (`OW_ENABLE_LUA`).
- Otherwise, stub implementation compiles and returns fallback values.

Demo script:

- `assets/scripts/game.lua`
- Exposes function:
  - `ComputeHeroImpulse(dt, timeSeconds)`

Engine calls this in fixed update and applies returned impulse to hero rigidbody.
