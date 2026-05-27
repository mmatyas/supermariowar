#pragma once

#include <SDL3_mixer/SDL_mixer.h>

#include <array>
#include <bitset>
#include <filesystem>

struct MixDeleter {
    void operator()(MIX_Mixer* ptr) const noexcept;
    void operator()(MIX_Track* ptr) const noexcept;
    void operator()(MIX_Audio* ptr) const noexcept;
};
using MixDevicePtr = std::unique_ptr<MIX_Mixer, MixDeleter>;
using MixTrackPtr = std::unique_ptr<MIX_Track, MixDeleter>;
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

    static void onTrackFinished(void*, MIX_Track* rawptr);

private:
    MixAudioPtr m_audio;
    std::bitset<k_channels> m_channels;
    size_t m_last_start_time = 0;

    static inline std::array<std::pair<MIX_Track*, sfxSound*>, k_channels> s_channels {};
};


class sfxMusic {
public:
    sfxMusic() = default;
    sfxMusic(const std::filesystem::path& path);

    void play(bool fPlayonce, bool fResume);
    void stop();
    bool isPlaying() const;

    void togglePause();

    static inline bool fResumeMusic = true;

private:
    MixAudioPtr m_audio;
    bool m_paused = false;
};
