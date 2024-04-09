#pragma once

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;

class CO_Spring : public MO_CarriedObject
{
	public:
		CO_Spring(gfxSprite *nspr, short ix, short iy, bool fsuper);

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

		void place();

	protected:

		virtual void hittop(CPlayer * player);
		void hitother(CPlayer * player);

		bool fSuper;
		short iOffsetY;

	friend class CPlayer;
};


