#pragma once

#include "OverMapObject.h"


class OMO_Phanto : public IO_OverMapObject
{
	public:
		OMO_Phanto(gfxSprite *nspr, short x, short y, float velx, float vely, short type);
		~OMO_Phanto(){};

		void update() override;
		bool collide(CPlayer * player) override;

    short GetType() {
        return iType;
    }

	private:
		short iType;

		float dMaxSpeedX, dMaxSpeedY;
		float dReactionSpeed, dSpeedRatio;
		short iSpeedTimer;
};
