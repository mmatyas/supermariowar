#ifndef SMW_GAMEOBJECT_BLOCK_MO_Hammer_H
#define SMW_GAMEOBJECT_BLOCK_MO_Hammer_H

class MO_Hammer : public IO_MovingObject
{
	public:
		MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);
		~MO_Hammer(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short colorOffset;

	private:
		short ttl;
		bool fSuper;
		Spotlight * sSpotlight;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_Hammer_H
