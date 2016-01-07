#ifndef SFX_H
#define SFX_H

#include "SDL_mixer.h"
#include <string>

#define NUM_SOUND_CHANNELS 16

bool sfx_init();
void sfx_close();
void sfx_stopallsounds();
void sfx_setmusicvolume(int volume);
void sfx_setsoundvolume(int volume);

class sfxSound
{
	public:
		sfxSound();
		~sfxSound();

		bool init(const std::string& filename);

		int play();
		int playloop(int iLoop);
		void stop();
		void sfx_pause();

    void resetpause() {
        paused = false;
    }

		void reset();
    bool isready() {
        return ready;
    }
		int isPlaying();

		void clearchannel();

	private:
		Mix_Chunk *sfx;
		int channel;
		bool paused;
		bool ready;
		int starttime;
		short instances;
};

class sfxMusic
{
	public:
		sfxMusic();
		~sfxMusic();

		bool load(const std::string& filename);

		void play(bool fPlayonce, bool fResume);
		void stop();
		void sfx_pause();

    void resetpause() {
        paused = false;
    }

		void reset();
    bool isready() {
        return ready;
    }
		int isplaying();

	private:
		Mix_Music *music;
		bool paused;
		bool ready;
};

#endif // SFX_H
