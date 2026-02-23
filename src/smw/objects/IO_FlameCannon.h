#pragma once

#include "ObjectBase.h"


class IO_FlameCannon : public CObject {
public:
    IO_FlameCannon(Vec2s pos, short freq, short direction);

    void draw() override;
    void draw(short iOffsetX, short iOffsetY);
    void update() override;

    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject*) override {};

private:
    void SetNewTimer();

    short iFreq, iTimer, iCycle;
    short iFrame = 0;
    short iDirection;

    friend class CPlayerAI;
};
