#pragma once

#ifdef SDL2_USE_MIXERX
#include "SDL_mixer_ext.h"
#else
#include "SDL_mixer.h"
#endif

#include <array>
#include <cassert>
#include <bitset>
#include <filesystem>


struct MixDeleter {
    void operator()(Mix_Chunk* ptr) const noexcept;
    void operator()(Mix_Music* ptr) const noexcept;
};
using MixChunkPtr = std::unique_ptr<Mix_Chunk, MixDeleter>;
using MixMusicPtr = std::unique_ptr<Mix_Music, MixDeleter>;


class Audio {
public:
    static Audio& get() {
        assert(s_instance);
        return *s_instance;
    }

    void clearAllEffects();
    void setMusicVolume(int volume);
    void setEffectVolume(int volume);

private:
    friend struct Systems;
    Audio();
    ~Audio();
    inline static Audio* s_instance = nullptr;
};


class sfxSound {
public:
    static constexpr int k_channels = 16;

    sfxSound() = default;
    sfxSound(const std::filesystem::path& path);

    bool play();
    void playLoop(int loops);
    void stop();

    bool isPlaying() const { return m_channels.any(); }

    static void onChannelFinished(int channel);

private:
    MixChunkPtr m_sfx;
    std::bitset<k_channels> m_channels;
    size_t m_last_start_time = 0;

    static inline std::array<sfxSound*, k_channels> s_channels {};
};


class sfxMusic {
public:
    sfxMusic() = default;
    sfxMusic(const std::filesystem::path& path);

    void play(bool fPlayonce, bool fResume);
    void stop();

    void togglePause();

    bool isPlaying() const;

private:
    MixMusicPtr m_music;
    bool m_paused = false;
};
