#pragma once

#include "IO_Block.h"
#include "GlobalConstants.h"

class gfxSprite;
class CPlayer;
class IO_MovingObject;


class B_PowerupBlock : public IO_Block {
public:
    B_PowerupBlock(gfxSprite* nspr, short x, short y,
        short iNumSpr, short aniSpeed, bool fHidden, short * piSettings);
    virtual ~B_PowerupBlock();

    void draw() override;
    void update() override;
    void reset() override;

    bool collide(CPlayer* player, short direction, bool useBehavior) override;
    bool collide(IO_MovingObject* object, short direction) override;

    bool hittop(CPlayer* player, bool useBehavior) override;
    bool hitbottom(CPlayer* player, bool useBehavior) override;

    bool hittop(IO_MovingObject* object) override;
    bool hitright(IO_MovingObject* object) override;
    bool hitleft(IO_MovingObject* object) override;

    void triggerBehavior() override;
    virtual short SelectPowerup();

protected:
    short iCountWeight;

    short timer;
    bool side;
    short iNumSprites;
    short animationSpeed;
    short drawFrame;
    short animationTimer;
    short animationWidth;

    short settings[NUM_POWERUPS];

friend class CPlayer;
};
