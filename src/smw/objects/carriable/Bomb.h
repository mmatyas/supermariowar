#ifndef SMW_GAMEOBJECT_CO_BOMB_H
#define SMW_GAMEOBJECT_CO_BOMB_H

#include "objects/moving/CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_Bomb : public MO_CarriedObject
{
	public:
		CO_Bomb(gfxSprite *nspr, short x, short y, float fVelX, float fVelY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, short timetolive);
		~CO_Bomb(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void place();

		void Die();

	protected:

		short iColorOffsetY;
		short ttl;

		Spotlight * sSpotlight;

	friend class CPlayer;
};


#endif // SMW_GAMEOBJECT_CO_BOMB_H
