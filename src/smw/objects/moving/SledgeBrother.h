#ifndef SMW_GAMEOBJECT_BLOCK_MO_SledgeBrother_H
#define SMW_GAMEOBJECT_BLOCK_MO_SledgeBrother_H

class MO_SledgeBrother : public IO_MovingObject
{
	public:
		MO_SledgeBrother(gfxSprite *nspr, short platformY, short type);
		virtual ~MO_SledgeBrother(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

		bool hit(CPlayer * player);

		void Die();
		void Damage(short playerID);

	protected:

		void SetLastAction(short type);

		short iType;

		short iActionState;
		short iDestLocationX[5];

		void randomaction();
		void move();
		void throwprojectile();
		void taunt();
		void turn();
		void jump();
		void wait(short min, short max);

		short location;

		short throwing_timer;

		short hit_timer;
		short hit_movement_timer;
		short hit_offset_y;

		short leg_offset_x;
		short leg_movement_timer;

		short arm_offset_x;
		short arm_movement_timer;

		short taunt_timer;
		short wait_timer;

		short hit_points;
		bool face_right;

		short iDestX;

		short iPlatformY;
		short need_action[6];

		short last_action;
		short last_action_count;
};


#endif // SMW_GAMEOBJECT_BLOCK_MO_SledgeBrother_H
