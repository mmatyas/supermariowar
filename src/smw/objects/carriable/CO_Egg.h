#ifndef SMW_GAMEOBJECT_CO_EGG_H
#define SMW_GAMEOBJECT_CO_EGG_H

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;

class CO_Egg : public MO_CarriedObject
{
	public:
		CO_Egg(gfxSprite *nspr, short iColor);
		~CO_Egg(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

		void placeEgg();
		void Drop() override;

		short getColor() { return color; }

	private:
		short relocatetimer;
		short explosiondrawframe, explosiondrawtimer;

		CPlayer * owner_throw;
		short owner_throw_timer;

		short sparkleanimationtimer;
		short sparkledrawframe;

		short color;

		short egganimationrates[6];

	friend class CPlayer;
	friend class MO_Yoshi;
	friend class CGM_Eggs;
};

#endif // SMW_GAMEOBJECT_CO_EGG_H
