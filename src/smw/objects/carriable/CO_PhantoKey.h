#pragma once

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;

class CO_PhantoKey : public MO_CarriedObject
{
	public:
		CO_PhantoKey(gfxSprite *nspr);

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

		void placeKey();

	private:
		short relocatetimer;

		short sparkleanimationtimer;
		short sparkledrawframe;

	friend class CGM_Chase;
};



