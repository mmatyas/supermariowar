#pragma once

#include "SDL_mixer.h"

#include <array>
#include <bitset>
#include <filesystem>

struct MixDeleter {
    void operator()(Mix_Chunk* ptr) const noexcept;
    void operator()(Mix_Music* ptr) const noexcept;
};
using MixChunkPtr = std::unique_ptr<Mix_Chunk, MixDeleter>;
using MixMusicPtr = std::unique_ptr<Mix_Music, MixDeleter>;


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
