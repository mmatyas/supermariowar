#ifndef SMW_GAMEOBJECT_BLOCK_MO_Boomerang_H
#define SMW_GAMEOBJECT_BLOCK_MO_Boomerang_H

class MO_Boomerang : public IO_MovingObject
{
	public:
		MO_Boomerang(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_Boomerang(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short colorOffset;

	private:
		void forcedead();

		bool fMoveToRight;
		bool fFlipped;
		short iStateTimer;

		short iStyle;
		Spotlight * sSpotlight;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_Boomerang_H
