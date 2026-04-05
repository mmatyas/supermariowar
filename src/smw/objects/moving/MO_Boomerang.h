#pragma once

#include "MovingObject.h"

class Spotlight;
enum class BoomerangStyle : unsigned char;


class MO_Boomerang : public IO_MovingObject {
public:
    MO_Boomerang(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);

    void update() override;
    bool collide(CPlayer* player) override;
    void draw() override;

    short colorOffset;

private:
    void forcedead();

    bool fMoveToRight;
    bool fFlipped;
    short iStateTimer;

    BoomerangStyle iStyle;
    Spotlight* sSpotlight;
};
