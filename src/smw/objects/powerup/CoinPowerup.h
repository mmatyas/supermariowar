#ifndef SMW_GAMEOBJECT_BLOCK_PU_CoinPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_CoinPowerup_H

class PU_CoinPowerup : public MO_Powerup
{
	public:
		PU_CoinPowerup(gfxSprite *nspr, short x, short y, short color, short value);
		~PU_CoinPowerup(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

	protected:
		short iColorOffsetY;
		short iValue;

		short sparkleanimationtimer;
		short sparkledrawframe;
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_CoinPowerup_H
