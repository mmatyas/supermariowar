#ifndef SMW_GAMEOBJECT_BLOCK_MO_Fireball_H
#define SMW_GAMEOBJECT_BLOCK_MO_Fireball_H

class MO_Fireball : public IO_MovingObject
{
	public:
		MO_Fireball(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_Fireball(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short colorOffset;

	private:
		short ttl;
		Spotlight * sSpotlight;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_Fireball_H
