#ifndef SMW_GAMEOBJECT_CO_BOMB_H
#define SMW_GAMEOBJECT_CO_BOMB_H

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_Bomb : public MO_CarriedObject
{
	public:
		CO_Bomb(gfxSprite *nspr, short x, short y, float fVelX, float fVelY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, short timetolive);
		~CO_Bomb(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

		void place();

		void Die() override;

	protected:

		short iColorOffsetY;
		short ttl;

		Spotlight * sSpotlight;

	friend class CPlayer;
};


#endif // SMW_GAMEOBJECT_CO_BOMB_H
