#pragma once

#include "MovingObject.h"


class MO_CheepCheep : public IO_MovingObject
{
	public:
		MO_CheepCheep(gfxSprite *nspr);
		~MO_CheepCheep(){};

		void draw() override;
		void update() override;
		bool collide(CPlayer * player) override;
		void collide(IO_MovingObject * object) override;
		void place();

		bool hittop(CPlayer * player);
		bool hitother(CPlayer * player);

		void Die() override;
		void ShatterDie();

	private:
		short iColorOffsetY;
		bool frozen;
};
