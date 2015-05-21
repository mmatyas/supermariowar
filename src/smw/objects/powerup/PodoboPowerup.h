#ifndef SMW_GAMEOBJECT_BLOCK_PU_PodoboPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_PodoboPowerup_H

class PU_PodoboPowerup : public MO_Powerup
{
	public:
		PU_PodoboPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_PodoboPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_PodoboPowerup_H
