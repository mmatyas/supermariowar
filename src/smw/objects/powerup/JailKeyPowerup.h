#ifndef SMW_GAMEOBJECT_BLOCK_PU_JailKeyPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_JailKeyPowerup_H

class PU_JailKeyPowerup : public MO_Powerup
{
	public:
		PU_JailKeyPowerup(gfxSprite *nspr, short x, short y);
		~PU_JailKeyPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_JailKeyPowerup_H
