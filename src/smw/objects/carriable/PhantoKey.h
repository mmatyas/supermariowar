#ifndef SMW_GAMEOBJECT_CO_PHANTOKEY_H
#define SMW_GAMEOBJECT_CO_PHANTOKEY_H

#include "objects/moving/CarriedObject.h"

class CPlayer;
class gfxSprite;

class CO_PhantoKey : public MO_CarriedObject
{
	public:
		CO_PhantoKey(gfxSprite *nspr);
		~CO_PhantoKey(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void placeKey();

	private:
		short relocatetimer;

		short sparkleanimationtimer;
		short sparkledrawframe;

	friend class CGM_Chase;
};


#endif // SMW_GAMEOBJECT_CO_PHANTOKEY_H
