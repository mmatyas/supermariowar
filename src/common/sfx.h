#pragma once

#ifdef SDL2_USE_MIXERX
#include "SDL_mixer_ext.h"
#else
#include "SDL_mixer.h"
#endif

#include <string>


bool sfx_init();
void sfx_close();
void sfx_stopallsounds();
void sfx_setmusicvolume(int volume);
void sfx_setsoundvolume(int volume);
bool sfx_canPlayAudio();


class sfxSound {
public:
    ~sfxSound();

    bool init(const std::string& filename);

    int play();
    int playLoop(int iLoop);
    void stop();

    void togglePause();
    void resetPause() {
        paused = false;
    }

    void reset();
    void clearChannel();

    bool isReady() const { return ready; }
    bool isPlaying() const;

private:
    Mix_Chunk* sfx = nullptr;
    int channel = -1;
    bool paused = false;
    bool ready = false;
    int starttime = 0;
    short instances = 0;
};


class sfxMusic {
public:
    ~sfxMusic();

    bool load(const std::string& filename);

    void play(bool fPlayonce, bool fResume);
    void stop();

    void togglePause();

    void reset();
    bool isPlaying() const;
    bool isReady() const { return ready; }

private:
    Mix_Music* music = nullptr;
    bool paused = false;
    bool ready = false;
};
