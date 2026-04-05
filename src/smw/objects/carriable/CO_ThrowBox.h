#pragma once

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_ThrowBox : public MO_CarriedObject {
public:
    CO_ThrowBox(gfxSprite* nspr, Vec2s pos, short item);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

    bool KillPlayer(CPlayer* player);

    void Drop() override;
    void Kick() override;

    void collide(IO_MovingObject* object) override;
    void Die() override;
    void ShatterDie();

    void SideBounce(bool fRightSide) override;
    float BottomBounce() override;

    bool HasKillVelocity();

private:
    void DieHelper();

    short iItem;

    bool frozen;
    short frozentimer;
    short frozenanimationspeed;

    Spotlight* sSpotlight;

    friend class MO_Explosion;
    friend class MO_BulletBill;
    friend class MO_Goomba;
    friend class MO_CheepCheep;
    friend class MO_SpinAttack;
    friend class MO_AttackZone;

    friend void RunGame();
    friend void shakeScreen();
};
