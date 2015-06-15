#ifndef SMW_GAMEOBJECT_CO_THROWBLOCK_H
#define SMW_GAMEOBJECT_CO_THROWBLOCK_H

#include "objects/moving/CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_ThrowBlock : public MO_CarriedObject
{
	public:
		CO_ThrowBlock(gfxSprite * nspr, short x, short y, short type);
		~CO_ThrowBlock(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		bool HitTop(CPlayer * player);
		bool HitOther(CPlayer * player);

		bool KillPlayer(CPlayer * player);
		void Drop();
		void Kick();

		void collide(IO_MovingObject * object);
		void CheckAndDie();
		void Die();
		void ShatterDie();

		void SideBounce(bool fRightSide);

	private:
		void DieHelper();

		short iType;

		short iDeathTime;
		bool fDieOnBounce;
		bool fDieOnPlayerCollision;
		short iBounceCounter;
		short iNoOwnerKillTime;

		bool frozen;
		short frozentimer;
		float frozenvelocity;
		short frozenanimationspeed;

		Spotlight * sSpotlight;

	friend class CPlayer;
	friend class MO_Explosion;
	friend class MO_BulletBill;
	friend class MO_Goomba;
	//friend class MO_SledgeBrother;
	friend class MO_CheepCheep;
	friend class MO_SpinAttack;
	friend class MO_AttackZone;

	friend class CO_Shell;

	friend class B_ThrowBlock;
	friend class B_WeaponBreakableBlock;

	friend void RunGame();
		friend void shakeScreen();
};

#endif // SMW_GAMEOBJECT_CO_THROWBLOCK_H
