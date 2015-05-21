#ifndef SMW_GAMEOBJECT_BLOCK_MO_AttackZone_H
#define SMW_GAMEOBJECT_BLOCK_MO_AttackZone_H

class MO_AttackZone : public IO_MovingObject
{
	public:
		MO_AttackZone(short playerID, short teamID, short x, short y, short w, short h, short time, killstyle style, bool dieoncollision);
		~MO_AttackZone(){};

		virtual void update();
		virtual void draw() {} //This is invisible

		virtual bool collide(CPlayer * player);
		virtual void collide(IO_MovingObject * object);

		void Die();

	protected:

		bool fDieOnCollision;

		short iTimer;
		killstyle iStyle;

	friend class B_WeaponBreakableBlock;
};


#endif // SMW_GAMEOBJECT_BLOCK_MO_AttackZone_H
