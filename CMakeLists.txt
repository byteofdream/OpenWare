cmake_minimum_required(VERSION 3.16)
project(OpenWareEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(OW_BUILD_API "Build C API library in api/c" ON)

# Prefer legacy libGL on Linux to avoid hard dependency on GLX import libs.
set(OpenGL_GL_PREFERENCE LEGACY)
# If build dir was moved across distros, stale cached OpenGL paths can break linking.
foreach(_gl_cache_var
    OPENGL_gl_LIBRARY
    OPENGL_opengl_LIBRARY
    OPENGL_glx_LIBRARY
    OPENGL_egl_LIBRARY
    OPENGL_gles2_LIBRARY
    OPENGL_gles3_LIBRARY
    OPENGL_glu_LIBRARY
)
    if(DEFINED ${_gl_cache_var} AND NOT EXISTS "${${_gl_cache_var}}")
        unset(${_gl_cache_var} CACHE)
    endif()
endforeach()

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_MIXER QUIET SDL2_mixer)
pkg_check_modules(LUA54 QUIET lua5.4)
if(NOT LUA54_FOUND)
    pkg_check_modules(LUA54 QUIET lua)
endif()
if(NOT LUA54_FOUND)
    find_path(LUA_INCLUDE_DIR
        NAMES lua.h
        PATH_SUFFIXES lua5.4 lua54 lua)
    find_library(LUA_LIBRARY
        NAMES lua5.4 lua54 lua)
    if(LUA_INCLUDE_DIR AND LUA_LIBRARY)
        set(LUA54_FOUND 1)
        set(LUA54_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
        set(LUA54_LIBRARIES ${LUA_LIBRARY})
    endif()
endif()

add_executable(OpenWareEngine
    src/main.cpp
    src/Renderer/Shader.cpp
    src/Renderer/Mesh.cpp
    src/Renderer/Material.cpp
    src/Renderer/Renderer.cpp
    src/Scene/Camera.cpp
    src/Scene/Entity.cpp
    src/Scene/Scene.cpp
    src/Physics/PhysicsSystem.cpp
    src/Audio/AudioSystem.cpp
    src/Script/LuaScriptSystem.cpp
    src/Input/Input.cpp
    src/Resource/OBJLoader.cpp
    src/Resource/TextureLoader.cpp
    src/Resource/MaterialLoader.cpp
    src/UI/AboutUI.cpp
    src/UI/DebugUI.cpp
)

target_include_directories(OpenWareEngine PRIVATE include ${SDL2_INCLUDE_DIRS})
target_link_libraries(OpenWareEngine PRIVATE OpenGL::GL ${SDL2_LIBRARIES})
target_compile_options(OpenWareEngine PRIVATE ${SDL2_CFLAGS_OTHER})

if(SDL2_MIXER_FOUND)
    target_compile_definitions(OpenWareEngine PRIVATE OW_ENABLE_AUDIO=1)
    target_include_directories(OpenWareEngine PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
    target_link_libraries(OpenWareEngine PRIVATE ${SDL2_MIXER_LIBRARIES})
    target_compile_options(OpenWareEngine PRIVATE ${SDL2_MIXER_CFLAGS_OTHER})
endif()

if(LUA54_FOUND)
    target_compile_definitions(OpenWareEngine PRIVATE OW_ENABLE_LUA=1)
    target_include_directories(OpenWareEngine PRIVATE ${LUA54_INCLUDE_DIRS})
    target_link_libraries(OpenWareEngine PRIVATE ${LUA54_LIBRARIES})
    target_compile_options(OpenWareEngine PRIVATE ${LUA54_CFLAGS_OTHER})
endif()

if(MSVC)
    target_compile_options(OpenWareEngine PRIVATE /W4)
else()
    target_compile_options(OpenWareEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(OW_BUILD_API)
    add_subdirectory(api/c)
endif()
