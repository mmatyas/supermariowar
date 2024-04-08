#pragma once

#include "OverMapObject.h"


class OMO_RaceGoal : public IO_OverMapObject
{
	public:
		OMO_RaceGoal(gfxSprite *nspr, short id);

		void draw() override;
		void update() override;
		bool collide(CPlayer * player) override;
		void placeRaceGoal();
    void reset(short teamID) {
        tagged[teamID] = -1;
    }
    short isTagged(short teamID) const {
        return tagged[teamID];
    }
    short getGoalID() const {
        return goalID;
    }

	private:
		const short goalID;
		short tagged[4];
		float angle;
		float anglechange;
		short anglechangetimer;
		float speed;
		short quantity;
		bool isfinishline;

	friend class CObjectContainer;
};
