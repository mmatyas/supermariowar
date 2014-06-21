#ifndef INPUT_H
#define INPUT_H

#include "SDL.h"

#include <stdint.h>

//This set of classes takes SDL input events, sees if a
//player configured button is pressed, and sets the output
//key structure if that button is down/pressed.
//This way you just need to call Update(SDL_Event) once a frame
//and then to use all the player input, just call
//CPlayerInput.outputControls.{key}.{presstype} to get the key state you want

#define NUM_KEYS 8

#ifdef USE_SDL2
    #define SDL_KEYTYPE SDL_Keycode
#else
    #define SDL_KEYTYPE short
#endif

struct CKeyState {
	bool fDown;
	bool fPressed;
};

struct CInputControl {
    union {
        struct {
			SDL_KEYTYPE menu_up, menu_down, menu_left, menu_right, menu_select, menu_cancel, menu_random, menu_scrollfast;
		};
        struct {
			SDL_KEYTYPE game_left, game_right, game_jump, game_down, game_turbo, game_powerup, game_start, game_cancel;
		};
		SDL_KEYTYPE keys[NUM_KEYS];
	};
};

struct COutputControl {
    union {
        struct {
			CKeyState menu_up, menu_down, menu_left, menu_right, menu_select, menu_cancel, menu_random, menu_scrollfast;
		};
        struct {
			CKeyState game_left, game_right, game_jump, game_down, game_turbo, game_powerup, game_start, game_cancel;
		};
		CKeyState keys[NUM_KEYS];
	};

	bool operator == (const COutputControl& control) const {
		for (uint8_t k = 0; k < NUM_KEYS; k++)
			if (this->keys[k].fDown != control.keys[k].fDown
				|| this->keys[k].fPressed != control.keys[k].fPressed)
					return false;
		return true;
	}

	bool operator != (const COutputControl& control) const {
		return !(*this == control);
	}
};

struct CInputPlayerControl {
	short iDevice;
	CInputControl inputGameControls[2]; //0 == game controls, 1 == menu controls
};

class CPlayerInput
{
	public:

		CPlayerInput();
		~CPlayerInput(){}

		//Called before
		void CheckIfMouseUsed();

		//Clears key pressed states
		void ClearPressedKeys(short iGameState);
		void ResetKeys();
		//Updates what commands the user is sending
		void Update(SDL_Event event, short iGameState);

		//Points to the input controls in the global inputConfiguration class
		CInputPlayerControl * inputControls[4];

		//Use these structures to see what input has been pressed
		COutputControl outputControls[4];

		SDL_KEYTYPE iPressedKey;

	private:
		bool fUsingMouse;
};

#endif // INPUT_H
