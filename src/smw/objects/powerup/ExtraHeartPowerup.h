#ifndef SMW_GAMEOBJECT_BLOCK_PU_ExtraHeartPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_ExtraHeartPowerup_H

class PU_ExtraHeartPowerup : public MO_Powerup
{
	public:
		PU_ExtraHeartPowerup(gfxSprite *nspr, short x, short y);
		~PU_ExtraHeartPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_ExtraHeartPowerup_H
