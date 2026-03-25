#pragma once

#ifdef SDL2_USE_MIXERX
#include "SDL_mixer_ext.h"
#else
#include <SDL3_mixer/SDL_mixer.h>
#endif

#include <array>
#include <bitset>
#include <filesystem>

struct MixDeleter {
    void operator()(MIX_Audio* ptr) const noexcept;
};
using MixAudioPtr = std::unique_ptr<MIX_Audio, MixDeleter>;


bool sfx_init();
void sfx_close();
void sfx_stopallsounds();
void sfx_setmusicvolume(int volume);
void sfx_setsoundvolume(int volume);
bool sfx_canPlayAudio();


class sfxSound {
public:
    static constexpr int k_channels = 16;

    sfxSound() = default;
    sfxSound(const std::filesystem::path& path);

    bool play();
    void playLoop(int iLoop);
    void stop();

    bool isPlaying() const { return m_channels.any(); }

    static void onChannelFinished(int channel);

private:
    MixAudioPtr m_sfx;
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
    MixAudioPtr m_music;
    bool m_paused = false;
};
