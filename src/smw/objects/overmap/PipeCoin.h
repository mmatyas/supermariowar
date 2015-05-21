#ifndef SMW_GAMEOBJECT_BLOCK_OMO_PipeCoin_H
#define SMW_GAMEOBJECT_BLOCK_OMO_PipeCoin_H

class OMO_PipeCoin: public IO_OverMapObject
{
	public:
		OMO_PipeCoin(gfxSprite *nspr, float velx, float vely, short ix, short iy, short teamid, short colorid, short uncollectabletime);
		~OMO_PipeCoin(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

    short GetColor() {
        return iColorID;
    }
    short GetTeam() {
        return iTeamID;
    }

	private:
		short iTeamID, iColorID;

		short sparkleanimationtimer;
		short sparkledrawframe;

		short iUncollectableTime;
};

#endif // SMW_GAMEOBJECT_BLOCK_OMO_PipeCoin_H
