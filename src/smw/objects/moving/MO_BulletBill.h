#pragma once

#include "MovingObject.h"


class MO_BulletBill : public IO_MovingObject {
public:
    MO_BulletBill(gfxSprite* nspr, gfxSprite* nsprdead, short x, short y, float nspeed, short playerID, bool isspawned);
    ~MO_BulletBill() {};

    void update();
    void draw();
    void draw(short iOffsetX, short iOffsetY);
    bool collide(CPlayer* player);
    void collide(IO_MovingObject* object);

    bool hittop(CPlayer* player);
    bool hitother(CPlayer* player);

    void Die();
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
