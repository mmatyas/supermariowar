#ifndef SMW_GAMEOBJECT_MO_COIN_H
#define SMW_GAMEOBJECT_MO_COIN_H

#include "MovingObject.h"

class gfxSprite;
class CPlayer;

class MO_Coin : public IO_MovingObject
{
	public:
		MO_Coin(gfxSprite *nspr, float velx, float vely, short ix, short iy, short color, short team, short type, short uncollectabletime, bool placecoin);
		~MO_Coin() {};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCoin();

	private:
		short timer;
		short sparkleanimationtimer;
		short sparkledrawframe;

		short iType;
		short iTeam;
		short iUncollectableTime;
};

#endif // SMW_GAMEOBJECT_MO_COIN_H
