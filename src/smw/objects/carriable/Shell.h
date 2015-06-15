#ifndef SMW_GAMEOBJECT_CO_SHELL_H
#define SMW_GAMEOBJECT_CO_SHELL_H

#include "objects/moving/CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_Shell : public MO_CarriedObject
{
	public:
		CO_Shell(short type, short x, short y, bool dieOnMovingPlayerCollision, bool dieOnHoldingPlayerCollision, bool dieOnFire, bool killBouncePlayer);
		~CO_Shell(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		bool HitTop(CPlayer * player);
		bool HitOther(CPlayer * player);

		void UsedAsStoredPowerup(CPlayer * player);

		bool KillPlayer(CPlayer * player);
		void Drop();
		void Kick();

		void collide(IO_MovingObject * object);
		void CheckAndDie();
		void Die();
		void ShatterDie();

		void SideBounce(bool fRightSide);
		void AddMovingKill(CPlayer * killer);

		bool IsThreat() {
			return state == 1 || state == 3;
		}

		void Flip();

		void nospawn(short y, bool fBounce);

	private:
		void Stop();

		short iShellType;

		short iIgnoreBounceTimer;
		short iDestY;

		bool fDieOnMovingPlayerCollision;
		bool fDieOnHoldingPlayerCollision;
		bool fDieOnFire;
		bool fKillBouncePlayer;

		short iDeathTime;
		short iNoOwnerKillTime;

		short iColorOffsetY;
		short iBounceCounter;
		short iKillCounter;

		bool fFlipped;
		short iFlippedOffset;

		bool frozen;
		short frozentimer;
		float frozenvelocity;
		short frozenanimationspeed;

		Spotlight * sSpotlight;

	friend class CPlayer;
	friend class MO_Explosion;
	friend class MO_BulletBill;
	friend class MO_Goomba;
	friend class MO_Koopa;
	friend class MO_BuzzyBeetle;
	friend class MO_Spiny;
	//friend class MO_SledgeBrother;
	friend class MO_CheepCheep;
	friend class MO_PirhanaPlant;

	friend class MO_AttackZone;
	friend class MO_SpinAttack;

	friend class B_WeaponBreakableBlock;

	friend void RunGame();
		friend void shakeScreen();
};

#endif // SMW_GAMEOBJECT_CO_SHELL_H
