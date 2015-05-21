#ifndef SMW_GAMEOBJECT_BLOCK_MO_SpinAttack_H
#define SMW_GAMEOBJECT_BLOCK_MO_SpinAttack_H

class MO_SpinAttack : public MO_AttackZone
{
	public:
		MO_SpinAttack(short playerID, short teamID, killstyle style, bool direction, short offsety);
		~MO_SpinAttack(){};

		void update();

		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

	private:

		bool fDirection;
		short iOffsetY;
};


#endif // SMW_GAMEOBJECT_BLOCK_MO_SpinAttack_H
