#pragma once

#include "MovingObject.h"
#include "PlayerKillStyles.h"


class MO_AttackZone : public IO_MovingObject
{
	public:
		MO_AttackZone(short playerID, short teamID, short x, short y, short w, short h, short time, KillStyle style, bool dieoncollision);

		void update() override;
		void draw() override {} //This is invisible

		bool collide(CPlayer * player) override;
		void collide(IO_MovingObject * object) override;

		void Die() override;

	protected:

		bool fDieOnCollision;

		short iTimer;
		KillStyle iStyle;

	friend class B_WeaponBreakableBlock;
};
