#pragma once

#include "MovingObject.h"
#include "PlayerKillStyles.h"


class MO_Explosion : public IO_MovingObject {
public:
    MO_Explosion(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short id, short iTeamID, KillStyle style);

    void update() override;
    bool collide(CPlayer* player) override;

private:
    short timer;
    KillStyle iStyle;
};
