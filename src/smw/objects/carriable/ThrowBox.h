#ifndef SMW_GAMEOBJECT_CO_THROWBOX_H
#define SMW_GAMEOBJECT_CO_THROWBOX_H

#include "objects/moving/CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_ThrowBox : public MO_CarriedObject
{
	public:
		CO_ThrowBox(gfxSprite * nspr, short x, short y, short item);
		~CO_ThrowBox(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		bool KillPlayer(CPlayer * player);

		void Drop();
		void Kick();

		void collide(IO_MovingObject * object);
		void Die();
		void ShatterDie();

		void SideBounce(bool fRightSide);
		float BottomBounce();

		bool HasKillVelocity();

	private:
		void DieHelper();

		short iItem;

		bool frozen;
		short frozentimer;
		short frozenanimationspeed;

		Spotlight * sSpotlight;

	friend class CPlayer;
	friend class MO_Explosion;
	friend class MO_BulletBill;
	friend class MO_Goomba;
	friend class MO_CheepCheep;
	friend class MO_SpinAttack;
	friend class MO_AttackZone;

	friend class CO_ThrowBlock;
	friend class CO_Shell;
	friend class B_WeaponBreakableBlock;

	friend void RunGame();
		friend void shakeScreen();
};

#endif // SMW_GAMEOBJECT_CO_THROWBOX_H
