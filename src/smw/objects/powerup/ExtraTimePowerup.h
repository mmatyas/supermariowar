#ifndef SMW_GAMEOBJECT_BLOCK_PU_ExtraTimePowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_ExtraTimePowerup_H

class PU_ExtraTimePowerup : public MO_Powerup
{
	public:
		PU_ExtraTimePowerup(gfxSprite *nspr, short x, short y);
		~PU_ExtraTimePowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_ExtraTimePowerup_H
