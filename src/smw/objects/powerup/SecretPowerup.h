#ifndef SMW_GAMEOBJECT_BLOCK_PU_SecretPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_SecretPowerup_H

class PU_SecretPowerup : public MO_Powerup
{
	public:
		PU_SecretPowerup(gfxSprite * nspr, short x, short y, short type);
		~PU_SecretPowerup(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;
		void place();

	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
		short itemtype;
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_SecretPowerup_H
