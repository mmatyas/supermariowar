#ifndef SMW_GAMEOBJECT_BLOCK_OMO_RaceGoal_H
#define SMW_GAMEOBJECT_BLOCK_OMO_RaceGoal_H

class OMO_RaceGoal : public IO_OverMapObject
{
	public:
		OMO_RaceGoal(gfxSprite *nspr, short id);
		~OMO_RaceGoal(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void placeRaceGoal();
    void reset(short teamID) {
        tagged[teamID] = -1;
    }
    short isTagged(short teamID) {
        return tagged[teamID];
    }
    short getGoalID() {
        return goalID;
    }

	private:
		short tagged[4];
		short goalID;
		float angle;
		float anglechange;
		short anglechangetimer;
		float speed;
		short quantity;
		bool isfinishline;

	friend class CObjectContainer;
};

#endif // SMW_GAMEOBJECT_BLOCK_OMO_RaceGoal_H
