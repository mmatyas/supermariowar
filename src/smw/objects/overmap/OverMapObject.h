#ifndef IO_OVERMAPOBJECT
#define IO_OVERMAPOBJECT

#include "ObjectBase.h"

class gfxSprite;
class CPlayer;

class IO_OverMapObject : public CObject
{
public:
	IO_OverMapObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1, short iAnimationOffsetX = -1, short iAnimationOffsetY = -1, short iAnimationHeight = -1, short iAnimationWidth = -1);
	virtual ~IO_OverMapObject(){};

	virtual void draw();
	virtual void draw(short iOffsetX, short iOffsetY);
	virtual void update();
	virtual void animate();

    virtual bool collide(CPlayer*) { return false; }
	virtual void collide(IO_MovingObject*) {}

protected:
	short iNumSprites;
	short drawframe;
	short animationtimer;
	short animationWidth;

	short animationOffsetX, animationOffsetY;
	short animationspeed;
};

#endif // IO_OVERMAPOBJECT
