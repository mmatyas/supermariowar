#ifndef SMW_GAMEOBJECT_BLOCK_MysteryMushroomTempPlayer_H
#define SMW_GAMEOBJECT_BLOCK_MysteryMushroomTempPlayer_H

class MysteryMushroomTempPlayer
{
	public:
    MysteryMushroomTempPlayer() {
			fUsed = false;
		}

		~MysteryMushroomTempPlayer() {}

		void SetPlayer(CPlayer * player, short iPowerup);
		void GetPlayer(CPlayer * player, short * iPowerup);

		bool fUsed;

	private:

		float fx, fy;
		float fOldX, fOldY;
		float velx, vely;

		bool bobomb;
		short powerup;

		short burnupstarttimer, burnuptimer;

		bool inair, onice, invincible;
		short invincibletimer;

		MovingPlatform * platform;

		short iCapeFrameX, iCapeFrameY, iCapeTimer, iCapeYOffset;
		short gamepowerup;

		short iOldPowerupX, iOldPowerupY;
};

#endif // SMW_GAMEOBJECT_BLOCK_MysteryMushroomTempPlayer_H
