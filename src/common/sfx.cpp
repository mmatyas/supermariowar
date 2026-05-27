#include "sfx.h"

#include <SDL3/SDL.h>

#include <cassert>
#include <algorithm>
#include <format>
#include <iostream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

extern void SDLCALL musicfinished(void*, MIX_Track* rawptr);


namespace {
std::unique_ptr<class AudioSystem> s_audiosys;

constexpr auto TAG_SFX = "sfx";


class AudioSystem {
public:
    AudioSystem();

    MixAudioPtr openAudio(const std::filesystem::path& path);

    std::pair<MIX_Track*, size_t> playAsEffect(const MixAudioPtr& audio, long repeats = 0);
    void playAsMusic(const MixAudioPtr& audio, long repeats = -1);
    bool isMusicPlaying() const;

    void stopMusicTrack() const;
    void stopEffectTrack(size_t idx) const;

    void clearAllEffects() const;
    void setMusicVolume(float volume) const;
    void setMusicPaused(bool paused) const;
    void setEffectVolume(float volume) const;

private:
    struct SdlMixer {
        SdlMixer();
        ~SdlMixer();
    };

    SdlMixer m_mixer;
    MixDevicePtr m_device;
    MixTrackPtr m_music_track;
    std::array<MixTrackPtr, sfxSound::k_channels> m_sfx_tracks;
    size_t m_next_free_sfxtrack_idx = 0;

    void playAudio(const MixAudioPtr& audio, const MixTrackPtr& track, long repeats);
    void stopAudioTrack(const MixTrackPtr& track) const;
};


AudioSystem::SdlMixer::SdlMixer()
{
    if (!MIX_Init())
        throw std::format("Could not initialize SDL_mixer: {}", SDL_GetError());

    const auto sdlmix_version = MIX_Version();
    printf("[init] SDL_Mixer %d.%d.%d loaded.\n",
        SDL_VERSIONNUM_MAJOR(sdlmix_version),
        SDL_VERSIONNUM_MINOR(sdlmix_version),
        SDL_VERSIONNUM_MICRO(sdlmix_version));
}

AudioSystem::SdlMixer::~SdlMixer()
{
    MIX_Quit();
}


AudioSystem::AudioSystem()
{
    const SDL_AudioSpec audio_spec {
        .format = SDL_AudioFormat::SDL_AUDIO_S16,
        .channels = 2,
        .freq = 44100,
    };
    m_device = MixDevicePtr(MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec));
    if (!m_device)
        throw std::format("Could not open default audio device: {}", SDL_GetError());

    MIX_Mixer* m_device_raw = m_device.get();
    std::generate(m_sfx_tracks.begin(), m_sfx_tracks.end(), [m_device_raw]{
        auto track = MixTrackPtr(MIX_CreateTrack(m_device_raw));
        if (!track) {
            throw std::format("Could not create audio effect channel: {}", SDL_GetError());
        }
        if (!MIX_TagTrack(track.get(), TAG_SFX)) {
            throw std::format("Couldn't tag mixer track: {}", SDL_GetError());
        }
        MIX_SetTrackStoppedCallback(track.get(), &sfxSound::onTrackFinished, nullptr);
        return track;
    });

    m_music_track = MixTrackPtr(MIX_CreateTrack(m_device.get()));
    if (!m_music_track) {
        throw std::format("Could not create music channel: {}", SDL_GetError());
    }
    MIX_SetTrackStoppedCallback(m_music_track.get(), &musicfinished, nullptr);
}


MixAudioPtr AudioSystem::openAudio(const fs::path& path)
{
    const std::string path_str = path.generic_string();
    std::cout << "loading " << path_str << " ...";

    auto audio = MixAudioPtr(MIX_LoadAudio(m_device.get(), path_str.c_str(), false));
    if (!audio)
        throw std::format("Failed to load {}: {}", path_str, SDL_GetError());

    std::cout << " done" << std::endl;
    return audio;
}


std::pair<MIX_Track*, size_t> AudioSystem::playAsEffect(const MixAudioPtr& audio, long repeats)
{
    const size_t track_idx = m_next_free_sfxtrack_idx;
    m_next_free_sfxtrack_idx = (m_next_free_sfxtrack_idx + 1) % m_sfx_tracks.size();

    playAudio(audio, m_sfx_tracks[track_idx], repeats);

    return std::make_pair(m_sfx_tracks[track_idx].get(), track_idx);
}

void AudioSystem::playAsMusic(const MixAudioPtr& audio, long repeats)
{
    playAudio(audio, m_music_track, repeats);
}

void AudioSystem::playAudio(const MixAudioPtr& audio, const MixTrackPtr& track, long repeats)
{
    const SDL_PropertiesID options = SDL_CreateProperties();
    if (!options)
        throw std::format("Failed to create audio options: {}", SDL_GetError());

    SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, repeats);

    MIX_SetTrackAudio(track.get(), audio.get());
    MIX_PlayTrack(track.get(), options);

    SDL_DestroyProperties(options);
}

bool AudioSystem::isMusicPlaying() const {
    return MIX_TrackPlaying(m_music_track.get());
}


void AudioSystem::stopMusicTrack() const {
    stopAudioTrack(m_music_track);
}

void AudioSystem::stopEffectTrack(size_t idx) const {
    stopAudioTrack(m_sfx_tracks.at(idx));
}

void AudioSystem::stopAudioTrack(const MixTrackPtr& track) const {
    MIX_StopTrack(track.get(), 0);
}


void AudioSystem::setMusicPaused(bool paused) const {
    (paused ? MIX_PauseTrack : MIX_ResumeTrack)(m_music_track.get());
}

void AudioSystem::clearAllEffects() const {
    MIX_StopTag(m_device.get(), TAG_SFX, 0);
}

void AudioSystem::setMusicVolume(float volume) const {
    MIX_SetTrackGain(m_music_track.get(), volume);
}

void AudioSystem::setEffectVolume(float volume) const {
    MIX_SetTagGain(m_device.get(), TAG_SFX, volume);
}
} // namespace


void MixDeleter::operator()(MIX_Mixer* ptr) const noexcept { if (ptr) MIX_DestroyMixer(ptr); }
void MixDeleter::operator()(MIX_Track* ptr) const noexcept { if (ptr) MIX_DestroyTrack(ptr); }
void MixDeleter::operator()(MIX_Audio* ptr) const noexcept { if (ptr) MIX_DestroyAudio(ptr); }


bool sfx_init() {
    assert(!s_audiosys);
    s_audiosys = std::make_unique<AudioSystem>();
    return true;
}

void sfx_close() {
    s_audiosys.reset();
}

void sfx_stopallsounds() {
    s_audiosys->clearAllEffects();
}

void sfx_setmusicvolume(int volume) {
    s_audiosys->setMusicVolume(volume / 128.f);
}

void sfx_setsoundvolume(int volume) {
    s_audiosys->setEffectVolume(volume / 128.f);
}

bool sfx_canPlayAudio() {
    return true;
}

sfxSound::sfxSound(const fs::path& path)
    : m_audio(s_audiosys->openAudio(path))
{}

bool sfxSound::play()
{
    const Uint32 current_time = SDL_GetTicks();
    if (current_time - m_last_start_time < 40)
        return false;

    const auto [track_ptr, track_idx] = s_audiosys->playAsEffect(m_audio);

    m_last_start_time = current_time;
    m_channels.set(track_idx);
    s_channels[track_idx] = {track_ptr, this};
    return true;
}

void sfxSound::playLoop(int loops)
{
    const auto [track_ptr, track_idx] = s_audiosys->playAsEffect(m_audio, loops);
    m_channels.set(track_idx);
}

void sfxSound::stop()
{
    for (size_t i = 0; i < m_channels.size(); i++) {
        if (m_channels.test(i)) {
            s_audiosys->stopEffectTrack(i);
        }
    }
}

void sfxSound::onTrackFinished(void*, MIX_Track* rawptr)
{
    auto it = std::find_if(s_channels.begin(), s_channels.end(), [rawptr](auto& pair) {
        return pair.first == rawptr;
    });
    if (it != s_channels.end() && it->second)  {
        const size_t index = std::distance(s_channels.begin(), it);
        it->second->m_channels.reset(index);
        it->second = nullptr;
    }
}


sfxMusic::sfxMusic(const fs::path& path)
    : m_audio(s_audiosys->openAudio(path))
{}

void sfxMusic::play(bool fPlayonce, bool fResume) {
    s_audiosys->playAsMusic(m_audio, fPlayonce ? 0 : -1);
    fResumeMusic = fResume;
}

void sfxMusic::stop() {
    s_audiosys->stopMusicTrack();
}

void sfxMusic::togglePause() {
    m_paused = !m_paused;
    s_audiosys->setMusicPaused(m_paused);
}

bool sfxMusic::isPlaying() const {
    return s_audiosys->isMusicPlaying();
}
