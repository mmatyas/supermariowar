#pragma once

#include "MovingObject.h"


class CO_Flag;

class MO_FlagBase : public IO_MovingObject {
public:
    MO_FlagBase(gfxSprite* nspr, short iTeamID, short iColorID);

    void draw() override;
    void update() override;
    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject* object) override;
    void placeFlagBase(bool fInit);
    void scoreFlag(CO_Flag* flag, CPlayer* player);
    void setFlag(CO_Flag* flag)
    {
        homeflag = flag;
    }

    short GetTeamID() const
    {
        return teamID;
    }

private:
    short teamID;
    short iGraphicOffsetX;
    float angle;
    float anglechange;
    short anglechangetimer;
    float speed;
    CO_Flag* homeflag;

    short timer;

    friend class CO_Flag;
    friend class CPlayer;
};
