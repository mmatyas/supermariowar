#ifndef SMW_GAMEOBJECT_BLOCK_PU_LeafPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_LeafPowerup_H

class PU_LeafPowerup : public PU_FeatherPowerup
{
	public:
		PU_LeafPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_LeafPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_LeafPowerup_H
