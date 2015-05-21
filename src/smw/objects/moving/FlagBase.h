#ifndef SMW_GAMEOBJECT_BLOCK_MO_FlagBase_H
#define SMW_GAMEOBJECT_BLOCK_MO_FlagBase_H

class CO_Flag;

class MO_FlagBase : public IO_MovingObject
{
	public:
		MO_FlagBase(gfxSprite *nspr, short iTeamID, short iColorID);
		~MO_FlagBase(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void placeFlagBase(bool fInit);
		void scoreFlag(CO_Flag * flag, CPlayer * player);
    void setFlag(CO_Flag * flag) {
        homeflag = flag;
    }

    short GetTeamID() {
        return teamID;
    }

	private:
		short teamID;
		short iGraphicOffsetX;
		float angle;
		float anglechange;
		short anglechangetimer;
		float speed;
		CO_Flag * homeflag;

		short timer;

	friend class CO_Flag;
	friend class CPlayer;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_FlagBase_H
