#ifndef BLOCK_BASE_H
#define BLOCK_BASE_H

#include "ObjectBase.h"

class IO_Block : public CObject
{
	public:
		IO_Block(gfxSprite* nspr, Vec2s pos);

		virtual void draw();
		virtual void update();
		virtual void reset();

		//Returns true if we should continue to check for collisions in this direction
		virtual bool collide(CPlayer * player, short direction, bool useBehavior);
		virtual bool collide(IO_MovingObject * object, short direction);

    virtual bool isTransparent() {
        return false;
    }
    virtual bool isHidden() {
        return hidden;
    }

		virtual bool hittop(CPlayer * player, bool useBehavior);
		virtual bool hitbottom(CPlayer * player, bool useBehavior);
		virtual bool hitright(CPlayer * player, bool useBehavior);
		virtual bool hitleft(CPlayer * player, bool useBehavior);

		virtual bool hittop(IO_MovingObject * object);
		virtual bool hitbottom(IO_MovingObject * object);
		virtual bool hitright(IO_MovingObject * object);
		virtual bool hitleft(IO_MovingObject * object);

		virtual void triggerBehavior() {}

		void KillPlayersAndObjectsInsideBlock(short playerID);

	protected:
		void BounceMovingObject(IO_MovingObject * object);

		short iBumpPlayerID;
		short iBumpTeamID;

		float fposx, fposy;  //position to return to (for note and bumpable blocks)
		short iposx, iposy; //position of the block (doesn't move)

		bool hidden, ishiddentype;
		short iHiddenTimer;

		short col, row;

	friend class CPlayer;
};

#endif // BLOCK_BASE_H
