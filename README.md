# OpenWare (Minimal RenderWare-Style Engine)

OpenWare is a minimal open-source 3D engine prototype in a retro low-poly style inspired by RenderWare.

## Features

- C++17 modular architecture
- OpenGL rendering with SDL2 window/input backend
- Core math (`Vec2`, `Vec3`, `Mat4`, `Transform`)
- Gameplay foundation:
  - game state machine (`Playing` / `Paused`)
  - fixed timestep simulation loop (`60 Hz`)
- Scene system (`Entity`, `Scene`, `Camera`, `DirectionalLight`)
- Renderer system (`Shader`, `Mesh`, `Material`, `Renderer`)
- Physics module with rigid bodies, gravity, impulses, and sphere collision response
- Resource loading:
  - OBJ mesh loader (`.obj`)
  - PPM texture loader (`.ppm`)
  - material loader (`.mat`)
- Optional Lua scripting callbacks (enabled when Lua is installed)
- Optional audio SFX via SDL2_mixer (enabled when installed)
- Debug UI overlay:
  - FPS and frame time
  - Settings menu on `Esc`

## Project Structure

- `include/Engine/Core` - math and transform
- `include/Engine/Renderer` - rendering interfaces and OpenGL glue
- `include/Engine/Scene` - scene, entity, camera, light
- `include/Engine/Physics` - simple collision system
- `include/Engine/Audio` - audio wrapper (SDL2_mixer)
- `include/Engine/Script` - Lua scripting wrapper
- `include/Engine/Input` - SDL2 input wrapper
- `include/Engine/Resource` - OBJ/PPM loaders
- `include/Engine/UI` - debug HUD/settings UI
- `src/...` - module implementations
- `api/` - C shared library API and Python HTTP API examples
- `assets/` - demo resources (`cube.obj`, `checker.ppm`, `.mat`, scripts)
- `docs/` - engine system and integration docs

## Dependencies (Fedora)

```bash
sudo dnf install gcc-c++ cmake ninja-build SDL2-devel mesa-libGL-devel pkgconf-pkg-config
```

Optional packages:

```bash
sudo dnf install SDL2_mixer-devel lua-devel
```

## Build (Ninja)

All builds are expected to use the `build_ninja` directory.

```bash
cmake -S . -B build_ninja -G Ninja
cmake --build build_ninja -j4
```

## Run

```bash
./build_ninja/OpenWareEngine
```

## API Examples

The project also includes API examples in `api/`:

- C API in `api/c` (shared library `openware_api`)
- Python HTTP API in `api/python` (uses C API through `ctypes`)

Run the Python API server:

```bash
python3 api/python/server.py
```

## Where To Write Your Game Code

If you are building your own game on top of this engine, start here:

- `src/main.cpp` - main game entry point and loop.

Recommended places inside `src/main.cpp`:

- Scene setup block: create your entities, materials, lights, and initial transforms.
- Per-frame update block (inside `while (running)`): implement gameplay logic, AI, custom controls, and game rules.

Typical workflow:

- Keep engine modules inside `include/Engine` and `src/...` reusable.
- Put game-specific logic in `src/main.cpp` first.
- When your game grows, move game code into your own files (for example `src/Game/...`) and call them from `main.cpp`.

## Controls

### Camera

- `W/A/S/D` - move
- `Q/E` - down/up
- Mouse - look around
- `Space` - apply upward impulse to the dynamic test object
- `P` - toggle `Playing` / `Paused`

### Debug / UI

- `Esc` - open/close settings menu
- `F1` - show/hide debug HUD
- `F2` - open/close About panel
- `F10` - exit

### Settings Menu (when open)

- `1` - toggle wireframe
- `2` - toggle VSync
- `3` - cycle shading steps (`2/4/8`) for stylized low-poly lighting
- `4` - toggle background music on/off (if `assets/audio/music.wav` exists)
- `5` - toggle PS2 aesthetic mode
- `6` - cycle PS2 color quantization levels (`8/12/20`)
- `7` - cycle vertex jitter amount
- `8` - cycle fog strength

## Notes

- The engine is intentionally minimal and designed to be extended with animation, audio, richer physics, ECS, and tooling.
- In headless environments, runtime may fail with `No available video device` even though compilation succeeds.
- Detailed docs are in `docs/README.md`.
