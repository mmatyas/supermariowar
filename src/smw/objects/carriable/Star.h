#ifndef SMW_GAMEOBJECT_BLOCK_CO_Star_H
#define SMW_GAMEOBJECT_BLOCK_CO_Star_H

class CO_Star : public MO_CarriedObject
{
	public:
		CO_Star(gfxSprite *nspr, short type, short id);
		~CO_Star(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void placeStar();

    short getType() {
        return iType;
    }
    void setPlayerColor(short iColor) {
        iOffsetY = 64 + (iColor << 5);
    }

	private:
		short timer;
		short iType, iOffsetY;
		short sparkleanimationtimer;
		short sparkledrawframe;
		short iID;

	friend class CPlayer;
	friend class CGM_Star;
};

//Declaring class for use in MO_FlagBase (some compilers complain if this isn't here)
#endif // SMW_GAMEOBJECT_BLOCK_CO_Star_H
