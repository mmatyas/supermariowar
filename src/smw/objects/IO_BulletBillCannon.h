#pragma once

#include "object.h"


class IO_BulletBillCannon : public CObject
{
	public:
		IO_BulletBillCannon(short x, short y, short freq, float vel, bool preview);
		~IO_BulletBillCannon() {}

		void draw() {}
		void update();

    bool collide(CPlayer *) {
        return false;
    }
		void collide(IO_MovingObject *) {}

	private:
		void SetNewTimer();

		short iFreq, iTimer;
		float dVel;
		bool fPreview;
};


