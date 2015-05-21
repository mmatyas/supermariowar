#ifndef SMW_GAMEOBJECT_BLOCK_OMO_Area_H
#define SMW_GAMEOBJECT_BLOCK_OMO_Area_H

class OMO_Area : public IO_OverMapObject
{
	public:
		OMO_Area(gfxSprite *nspr, short numAreas);
		~OMO_Area(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void placeArea();
		void reset();
    short getColorID() {
        return colorID;
    }
		void setOwner(CPlayer * player);

	private:
		short iPlayerID;
		short iTeamID;

		short colorID;
		short scoretimer;
		short frame;
		short relocatetimer;
		short numareas;
		short totalTouchingPlayers;
		CPlayer * touchingPlayer;

	friend class CObjectContainer;
};

#endif // SMW_GAMEOBJECT_BLOCK_OMO_Area_H
