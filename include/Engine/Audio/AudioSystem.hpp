#pragma once

// Audio module: simple SFX playback wrapper (SDL2_mixer, optional).

#include <string>

namespace ow {

class AudioClip {
public:
    void* native = nullptr;
};

class AudioMusic {
public:
    void* native = nullptr;
};

class AudioSystem {
public:
    static bool Init();
    static void Shutdown();

    static bool IsAvailable();
    static AudioClip LoadWav(const std::string& path);
    static void PlaySfx(const AudioClip& clip, int loops = 0);
    static void FreeClip(AudioClip& clip);

    static AudioMusic LoadMusic(const std::string& path);
    static void PlayMusicLoop(const AudioMusic& music);
    static void StopMusic();
    static void FreeMusic(AudioMusic& music);
};

} // namespace ow
