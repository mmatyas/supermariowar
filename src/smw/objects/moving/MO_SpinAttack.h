#pragma once

#include "MO_AttackZone.h"


class MO_SpinAttack : public MO_AttackZone {
public:
    MO_SpinAttack(short playerID, short teamID, KillStyle style, bool direction, short offsety);

    void update() override;

    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject* object) override;

private:
    bool fDirection;
    short iOffsetY;
};
