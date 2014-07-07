#ifndef OBJECTHAZARD_H
#define OBJECTHAZARD_H

#include "gfx.h"
#include "object.h"
#include "PlayerKillStyles.h"

class CPlayer;
class IO_MovingObject;

class OMO_OrbitHazard : public IO_OverMapObject
{
	public:
		OMO_OrbitHazard(gfxSprite *nspr, short x, short y, float radius, float vel, float angle, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);
		~OMO_OrbitHazard() {}

		void update();

		bool collide(CPlayer *);

	private:
		void CalculatePosition();

		float dAngle, dVel, dRadius;
		float dCenterX, dCenterY;
};

class OMO_StraightPathHazard : public IO_OverMapObject
{
	public:
		OMO_StraightPathHazard(gfxSprite *nspr, short x, short y, float angle, float vel, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);
		~OMO_StraightPathHazard() {}

		void update();

		bool collide(CPlayer *);

	private:

		float dAngle, dVel;
};

class MO_BulletBill : public IO_MovingObject
{
	public:
		MO_BulletBill(gfxSprite *nspr, gfxSprite *nsprdead, short x, short y, float nspeed, short playerID, bool isspawned);
		~MO_BulletBill(){};

		void update();
		void draw();
		void draw(short iOffsetX, short iOffsetY);
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

		bool hittop(CPlayer * player);
		bool hitother(CPlayer * player);

		void Die();
		void SetDirectionOffset();

	private:
		gfxSprite * spr_dead;

		short iColorID;

		short iColorOffsetY;
		short iDirectionOffsetY;

		bool fIsSpawned;
		short iHiddenDirection;
		short iHiddenPlane;

	friend class MO_Podobo;
};

class IO_BulletBillCannon : public CObject
{
	public:
		IO_BulletBillCannon(short x, short y, short freq, float vel, bool preview);
		~IO_BulletBillCannon() {}

		void draw() {}
		void update();

    bool collide(CPlayer *) {
        return false;
    }
		void collide(IO_MovingObject *) {}

	private:
		void SetNewTimer();

		short iFreq, iTimer;
		float dVel;
		bool fPreview;
};


class MO_Explosion : public IO_MovingObject
{
	public:
		MO_Explosion(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short id, short iTeamID, killstyle style);
		~MO_Explosion(){};

		void update();
		bool collide(CPlayer * player);

	private:

		short timer;
		killstyle iStyle;
};


class IO_FlameCannon : public CObject
{
	public:
		IO_FlameCannon(short x, short y, short freq, short direction);
		~IO_FlameCannon() {}

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		bool collide(CPlayer * player);
		void collide(IO_MovingObject *) {}

	private:
		void SetNewTimer();

		short iFreq, iTimer, iCycle;
		short iFrame;

		short iDirection;

	friend class CPlayerAI;
};


class MO_PirhanaPlant : public IO_MovingObject
{
	public:
		MO_PirhanaPlant(short x, short y, short type, short freq, short direction, bool preview);
		~MO_PirhanaPlant() {}

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		bool collide(CPlayer * player);
		void collide(IO_MovingObject *);

		void KillPlant();

	private:
		void SetNewTimer();

		float GetFireballAngle();

		short iType, iDirection;
		short iFreq, iTimer;
		//short iHiddenPlane, iHiddenDirection;
		//short iSrcX, iSrcY;
		short iAnimationTimer;
		short iFrame;
		short iActionTimer;

		bool fPreview;

	friend class CPlayerAI;
};

#endif // OBJECTHAZARD_H
