#pragma once

#include "MovingObject.h"


class MO_PirhanaPlant : public IO_MovingObject {
public:
    MO_PirhanaPlant(Vec2s pos, short type, short freq, short direction, bool preview);
    ~MO_PirhanaPlant() {}

    void draw();
    void draw(short iOffsetX, short iOffsetY);
    void update();

    bool collide(CPlayer* player);
    void collide(IO_MovingObject*);

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
