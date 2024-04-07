#ifndef SMW_GAMEOBJECT_BLOCK_MO_SpinAttack_H
#define SMW_GAMEOBJECT_BLOCK_MO_SpinAttack_H

class MO_SpinAttack : public MO_AttackZone
{
	public:
		MO_SpinAttack(short playerID, short teamID, KillStyle style, bool direction, short offsety);
		~MO_SpinAttack(){};

		void update() override;

		bool collide(CPlayer * player) override;
		void collide(IO_MovingObject * object) override;

	private:

		bool fDirection;
		short iOffsetY;
};


#endif // SMW_GAMEOBJECT_BLOCK_MO_SpinAttack_H
