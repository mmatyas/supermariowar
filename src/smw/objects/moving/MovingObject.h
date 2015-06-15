#ifndef IO_MOVING_OBJECT
#define IO_MOVING_OBJECT

#include "ObjectBase.h"
#include "MovingObjectTypes.h"

class MovingPlatform;

class IO_MovingObject : public CObject
{
	public:
		IO_MovingObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1, short iAnimationOffsetX = -1, short iAnimationOffsetY = -1, short iAnimationHeight = -1, short iAnimationWidth = -1);
		virtual ~IO_MovingObject(){};

		virtual void draw();
		virtual void update();
		virtual void animate();

		virtual bool collide(CPlayer * player);

		void collide(IO_MovingObject *){}
	    MovingObjectType getMovingObjectType() {
	        return movingObjectType;
	    }
		void applyfriction();
		void collision_detection_map();
		bool collision_detection_checksides();
		void flipsidesifneeded();

		virtual void SideBounce(bool fRightSide) {}
    virtual float BottomBounce() {
        return bounce;
    }
		void KillObjectMapHazard(short playerID = -1);

    virtual void CheckAndDie() {
        dead = true;
    }
    virtual void Die() {
        dead = true;
    }

    bool CollidesWithMap() {
        return fObjectCollidesWithMap;
    }

		short iPlayerID;
		short iTeamID;

	protected:
		float fOldX, fOldY;
		float fPrecalculatedY;

		short iNumSprites;
		short drawframe;
		short animationtimer;

		float bounce;

		short animationspeed;
		short animationWidth;
		short animationOffsetX;
		short animationOffsetY;

		bool inair;
		bool onice;

		MovingObjectType movingObjectType;

		//Pointer to the platform the object is riding
		MovingPlatform * platform;
		short iHorizontalPlatformCollision;
		short iVerticalPlatformCollision;

		bool fObjectDiesOnSuperDeathTiles;
		bool fObjectCollidesWithMap;

	friend class IO_Block;
	friend class B_PowerupBlock;
	friend class B_BreakableBlock;
	friend class B_NoteBlock;
	friend class B_DonutBlock;
	friend class B_FlipBlock;
	friend class B_BounceBlock;
	friend class MO_Explosion;
	friend class B_OnOffSwitchBlock;
	friend class B_SwitchBlock;
	friend class B_WeaponBreakableBlock;

	friend class MO_BulletBill;
	friend class MO_WalkingEnemy;
	friend class MO_Goomba;
	friend class MO_CheepCheep;
	friend class MO_PirhanaPlant;

	friend class MovingPlatform;

	friend void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead);
	friend void RunGame();
		friend void shakeScreen();
		friend void handleP2ObjCollisions();
};

#endif // IO_MOVING_OBJECT
