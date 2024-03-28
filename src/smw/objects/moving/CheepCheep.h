#pragma once

#include "MovingObject.h"


class MO_CheepCheep : public IO_MovingObject
{
	public:
		MO_CheepCheep(gfxSprite *nspr);
		~MO_CheepCheep(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void place();

		bool hittop(CPlayer * player);
		bool hitother(CPlayer * player);

		void Die();
		void ShatterDie();

	private:
		short iColorOffsetY;
		bool frozen;
};
