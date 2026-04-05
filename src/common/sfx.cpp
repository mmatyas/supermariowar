#include "sfx.h"

#include "SDL.h"

#include <format>
#include <iostream>
#include <string>

namespace fs = std::filesystem;


bool fResumeMusic = true;
extern void SDLCALL musicfinished();


void MixDeleter::operator()(Mix_Chunk* ptr) const noexcept { Mix_FreeChunk(ptr); }
void MixDeleter::operator()(Mix_Music* ptr) const noexcept { Mix_FreeMusic(ptr); }


bool sfx_init()
{
    Mix_OpenAudio(44100, AUDIO_S16, 2, 2048);
    Mix_AllocateChannels(sfxSound::k_channels);

    Mix_ChannelFinished(&sfxSound::onChannelFinished);
    Mix_HookMusicFinished(&musicfinished);

#ifndef __EMSCRIPTEN__
    const SDL_version* link_version = Mix_Linked_Version();
    printf("[sfx] SDL_Mixer %d.%d.%d initialized.\n",
        link_version->major, link_version->minor, link_version->patch);
#else
    SDL_version ver_compiled;
    SDL_MIXER_VERSION(&ver_compiled);
    printf("[sfx] SDL_Mixer %d.%d.%d initialized.\n",
        ver_compiled.major, ver_compiled.minor, ver_compiled.patch);
#endif

    return true;
}

void sfx_close()
{
    Mix_CloseAudio();
}

void sfx_stopallsounds()
{
    Mix_HaltChannel(-1);
}

void sfx_setmusicvolume(int volume)
{
    Mix_VolumeMusic(volume);
}

void sfx_setsoundvolume(int volume)
{
    Mix_Volume(-1, volume);
}

bool sfx_canPlayAudio()
{
#ifdef __EMSCRIPTEN__  // emscripten has sound capabilities
    return true;
#else
    int frequency, channels;
    Uint16 format;
    return Mix_QuerySpec(&frequency, &format, &channels) != 0 /* error */;
#endif
}

sfxSound::sfxSound(const fs::path& path)
{
    const std::string path_str = path.generic_string();
    std::cout << "loading " << path_str << " ...";

    m_sfx = MixChunkPtr(Mix_LoadWAV(path_str.c_str()));
    if (!m_sfx)
        throw std::format("Failed to load {}: {}", path_str, Mix_GetError());

    std::cout << " done" << std::endl;
}

bool sfxSound::play()
{
    const Uint32 current_time = SDL_GetTicks();
    if (current_time - m_last_start_time < 40)
        return false;

    const int channel = Mix_PlayChannel(-1, m_sfx.get(), 0);
    if (channel < 0)
        return false;

    m_last_start_time = current_time;
    m_channels.set(channel);
    s_channels[channel] = this;
    return true;
}

void sfxSound::playLoop(int loops)
{
    const int channel = Mix_PlayChannel(-1, m_sfx.get(), loops);
    if (channel < 0)
        return;

    m_channels.set(channel);
}

void sfxSound::stop()
{
    for (size_t i = 0; i < m_channels.size(); i++) {
        if (m_channels.test(i)) {
            Mix_HaltChannel(i);
        }
    }
}

void sfxSound::onChannelFinished(int channel)
{
    sfxSound* const sfx = s_channels[channel];
    if (sfx) {
        sfx->m_channels.reset(channel);
    }
    s_channels[channel] = nullptr;
}


sfxMusic::sfxMusic(const fs::path& path)
{
    const std::string path_str = path.generic_string();
    std::cout << "loading " << path_str << " ...";

    m_music = MixMusicPtr(Mix_LoadMUS(path_str.c_str()));
    if (!m_music)
        throw std::format("Failed to load {}: {}", path_str, Mix_GetError());

    std::cout << " done" << std::endl;
}

void sfxMusic::play(bool fPlayonce, bool fResume)
{
    Mix_PlayMusic(m_music.get(), fPlayonce ? 0 : -1);
    fResumeMusic = fResume;
}

void sfxMusic::stop()
{
    Mix_HaltMusic();
}

void sfxMusic::togglePause()
{
    if (m_paused) {
        Mix_ResumeMusic();
    } else {
        Mix_PauseMusic();
    }
    m_paused = !m_paused;
}

bool sfxMusic::isPlaying() const
{
    return Mix_PlayingMusic();
}
