#pragma once

#ifndef GAME_H
#define GAME_H

class CGame
{
public:
    CGame(const char *);
    ~CGame(void);

	int ScreenWidth;
	int ScreenHeight;
	int MenuTransparency;

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
