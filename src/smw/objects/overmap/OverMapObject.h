#ifndef IO_OVERMAPOBJECT
#define IO_OVERMAPOBJECT

#include "ObjectBase.h"

class gfxSprite;
class CPlayer;

class IO_OverMapObject : public CObject {
public:
    IO_OverMapObject(gfxSprite* nspr, Vec2s pos, short iNumSpr,
        short aniSpeed, short iCollisionWidth = -1,
        short iCollisionHeight = -1, short iCollisionOffsetX = -1,
        short iCollisionOffsetY = -1, short iAnimationOffsetX = -1,
        short iAnimationOffsetY = -1, short iAnimationHeight = -1,
        short iAnimationWidth = -1);

    void draw() override;
    virtual void draw(short iOffsetX, short iOffsetY);
    void update() override;
    virtual void animate();

    bool collide(CPlayer*) override { return false; }
    void collide(IO_MovingObject*) override {}

protected:
    short iNumSprites;
    short drawframe;
    short animationtimer;
    short animationWidth;

    short animationOffsetX, animationOffsetY;
    short animationspeed;
};

#endif  // IO_OVERMAPOBJECT
