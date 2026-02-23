#include "Engine/Audio/AudioSystem.hpp"

#ifdef OW_ENABLE_AUDIO
#include <SDL_mixer.h>
#endif

namespace ow {

#ifdef OW_ENABLE_AUDIO

namespace {

bool gInitialized = false;

} // namespace

bool AudioSystem::Init() {
    if (gInitialized) {
        return true;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        return false;
    }
    gInitialized = true;
    return true;
}

void AudioSystem::Shutdown() {
    if (!gInitialized) {
        return;
    }
    Mix_CloseAudio();
    gInitialized = false;
}

bool AudioSystem::IsAvailable() {
    return gInitialized;
}

AudioClip AudioSystem::LoadWav(const std::string& path) {
    AudioClip clip;
    if (!gInitialized) {
        return clip;
    }

    clip.native = Mix_LoadWAV(path.c_str());
    return clip;
}

void AudioSystem::PlaySfx(const AudioClip& clip, int loops) {
    if (!gInitialized || !clip.native) {
        return;
    }
    Mix_PlayChannel(-1, static_cast<Mix_Chunk*>(clip.native), loops);
}

void AudioSystem::FreeClip(AudioClip& clip) {
    if (!clip.native) {
        return;
    }
    Mix_FreeChunk(static_cast<Mix_Chunk*>(clip.native));
    clip.native = nullptr;
}

AudioMusic AudioSystem::LoadMusic(const std::string& path) {
    AudioMusic music;
    if (!gInitialized) {
        return music;
    }

    music.native = Mix_LoadMUS(path.c_str());
    return music;
}

void AudioSystem::PlayMusicLoop(const AudioMusic& music) {
    if (!gInitialized || !music.native) {
        return;
    }
    Mix_PlayMusic(static_cast<Mix_Music*>(music.native), -1);
}

void AudioSystem::StopMusic() {
    if (!gInitialized) {
        return;
    }
    Mix_HaltMusic();
}

void AudioSystem::FreeMusic(AudioMusic& music) {
    if (!music.native) {
        return;
    }
    Mix_FreeMusic(static_cast<Mix_Music*>(music.native));
    music.native = nullptr;
}

#else

bool AudioSystem::Init() {
    return false;
}

void AudioSystem::Shutdown() {}

bool AudioSystem::IsAvailable() {
    return false;
}

AudioClip AudioSystem::LoadWav(const std::string&) {
    return {};
}

void AudioSystem::PlaySfx(const AudioClip&, int) {}

void AudioSystem::FreeClip(AudioClip&) {}

AudioMusic AudioSystem::LoadMusic(const std::string&) {
    return {};
}

void AudioSystem::PlayMusicLoop(const AudioMusic&) {}

void AudioSystem::StopMusic() {}

void AudioSystem::FreeMusic(AudioMusic&) {}

#endif

} // namespace ow
