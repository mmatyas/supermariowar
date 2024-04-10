#pragma once

#include "object.h"


class IO_FlameCannon : public CObject
{
	public:
		IO_FlameCannon(short x, short y, short freq, short direction);
		~IO_FlameCannon() {}

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		bool collide(CPlayer * player);
		void collide(IO_MovingObject *) {}

	private:
		void SetNewTimer();

		short iFreq, iTimer, iCycle;
		short iFrame;

		short iDirection;

	friend class CPlayerAI;
};


