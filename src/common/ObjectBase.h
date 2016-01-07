#ifndef OBJECT_BASE_H
#define OBJECT_BASE_H

#include "gfx/gfxSprite.h"

class CPlayer;
class IO_Block;
class IO_MovingObject;

enum ObjectType {
    object_none = 0,
    object_block = 1,
    object_moving = 2,
    object_overmap = 3,
    object_area = 4,
    object_frenzycard = 5,
    object_race_goal = 6,
    object_thwomp = 7,
    object_kingofthehill_area = 8,
    object_bowserfire = 9,
    object_orbithazard = 10,
    object_bulletbillcannon = 11,
    object_flamecannon = 12,
    object_pathhazard = 13,
    object_pipe_coin = 14,
    object_pipe_bonus = 15,
    object_phanto = 16
};

float CapFallingVelocity(float vel);
float CapSideVelocity(float vel);

//object base class
class CObject
{
	public:

		CObject(gfxSprite *nspr, short x, short y);
		virtual ~CObject(){};

		virtual void draw(){};
		virtual void update() = 0;
		virtual bool collide(CPlayer *){return false;}
		virtual void collide(IO_MovingObject *){}

		virtual ObjectType getObjectType(){return objectType;}

		void setXf(float xf){fx = xf; ix = (short)fx;};
 		void setXi(short xi){ix = xi; fx = (float)ix;};
		void setYf(float yf){fy = yf; if (fy < 0.0f) iy = (short)(fy - 1.0f); else iy = (short)fy;};
		void setYi(short yi){iy = yi; fy = (float)iy;};

		int iNetworkID;
		short ix, iy;

		short collisionWidth;
		short collisionHeight;
		short collisionOffsetX;
		short collisionOffsetY;

    short GetState() {
        return state;
    }
    bool isDead() {
        return dead;
    }

    bool GetWrap() {
        if (spr) return spr->GetWrap();
        return true;
    }

		void GetCollisionBlocks(IO_Block * blocks[4]);

		//virtual short writeNetworkUpdate(char * pData);
		//virtual void readNetworkUpdate(short size, char * pData);

	protected:
		ObjectType objectType;

		short iw, ih;
		float fx, fy;
		float velx, vely;

		gfxSprite *spr;
		short state;
		bool dead;

		short index;

	friend class CObjectContainer;
	friend class CPlayer;
	friend void RunGame();
		friend void shakeScreen();
		friend void handleP2ObjCollisions();
	friend class CO_Shell;
	friend class B_BreakableBlock;
	friend class B_WeaponBreakableBlock;
};

#endif // OBJECT_BASE_H
