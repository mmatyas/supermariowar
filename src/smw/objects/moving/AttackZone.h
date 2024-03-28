#pragma once

#include "MovingObject.h"


class MO_AttackZone : public IO_MovingObject
{
	public:
		MO_AttackZone(short playerID, short teamID, short x, short y, short w, short h, short time, KillStyle style, bool dieoncollision);
		~MO_AttackZone(){};

		virtual void update();
		virtual void draw() {} //This is invisible

		virtual bool collide(CPlayer * player);
		virtual void collide(IO_MovingObject * object);

		void Die();

	protected:

		bool fDieOnCollision;

		short iTimer;
		KillStyle iStyle;

	friend class B_WeaponBreakableBlock;
};
