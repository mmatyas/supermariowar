#pragma once

#ifndef GAME_H
#define GAME_H

#include "rng.h"

//----------------- game options all parts of the game need -----------
enum gs {GS_SPLASH, GS_MENU, GS_START_GAME, GS_START_WORLD, GS_END_GAME, GS_GAME, GS_QUIT};

class CGame
{
public:
    CGame(char *);
    ~CGame(void);

    // run all the game logic
    void Go();

	int ScreenWidth;
	int ScreenHeight;
	int MenuTransparency;
	CRandomNumberGenerator	*rng;

	// TODO: move these to a config class/struct
	int GetScreenWidth(int iSize) {
		switch (iSize) {
		case 0:
			return ScreenWidth;
		case 1:
			return ScreenWidth/2;
		case 2:
			return ScreenWidth/4;
		default:
			throw "invalid iSize for width";
		}
	}

	int GetScreenHeight(int iSize) {
		switch (iSize) {
		case 0:
			return ScreenHeight;
		case 1:
			return ScreenHeight/2;
		case 2:
			return ScreenHeight/4;
		default:
			throw "invalid iSize for height";
		}
	}
//private:

};

#endif // GAME_H
