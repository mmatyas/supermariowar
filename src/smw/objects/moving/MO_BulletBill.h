#pragma once

#include "MovingObject.h"


class MO_BulletBill : public IO_MovingObject {
public:
    MO_BulletBill(gfxSprite* nspr, gfxSprite* nsprdead, Vec2s pos, float nspeed, short playerID, bool isspawned);

    void update() override;
    void draw() override;
    void draw(short iOffsetX, short iOffsetY);
    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject* object) override;

    bool hittop(CPlayer* player);
    bool hitother(CPlayer* player);

    void Die() override;
    void SetDirectionOffset();

private:
    gfxSprite* spr_dead;

    short iColorID;

    short iColorOffsetY;
    short iDirectionOffsetY;

    bool fIsSpawned;
    short iHiddenDirection;
    short iHiddenPlane;

    friend class MO_Podobo;
};
