#pragma once

#include "MovingObject.h"


class MO_PirhanaPlant : public IO_MovingObject {
public:
    MO_PirhanaPlant(Vec2s pos, short type, short freq, short direction, bool preview);

    void draw() override;
    void draw(short iOffsetX, short iOffsetY);
    void update() override;

    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject*) override;

    void KillPlant();

private:
    void SetNewTimer();

    float GetFireballAngle();

    short iType, iDirection;
    short iFreq, iTimer;
    // short iHiddenPlane, iHiddenDirection;
    // short iSrcX, iSrcY;
    short iAnimationTimer;
    short iFrame;
    short iActionTimer;

    bool fPreview;

    friend class CPlayerAI;
};
