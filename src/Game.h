#pragma once

#ifndef	__GAME_H
#define	__GAME_H

//----------------- game options all parts of the game need -----------
enum gs{GS_MENU, GS_START_GAME, GS_START_WORLD, GS_END_GAME, GS_GAME, GS_QUIT};

class CGame
{
	public:
		CGame(char *);
		~CGame(void);

		// run all the game logic
		void Go();
//private:

};

#endif // __GAME_H
