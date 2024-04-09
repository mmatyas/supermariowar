#pragma once

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_ThrowBlock : public MO_CarriedObject {
public:
    CO_ThrowBlock(gfxSprite* nspr, short x, short y, short type);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

    bool HitTop(CPlayer* player);
    bool HitOther(CPlayer* player);

    bool KillPlayer(CPlayer* player);
    void Drop() override;
    void Kick() override;

    void collide(IO_MovingObject* object) override;
    void CheckAndDie() override;
    void Die() override;
    void ShatterDie();

    void SideBounce(bool fRightSide) override;

private:
    void DieHelper();

    short iType;

    short iDeathTime;
    bool fDieOnBounce;
    bool fDieOnPlayerCollision;
    short iBounceCounter;
    short iNoOwnerKillTime;

    bool frozen;
    short frozentimer;
    float frozenvelocity;
    short frozenanimationspeed;

    Spotlight* sSpotlight;

friend class MO_SpinAttack;
friend class MO_AttackZone;
friend class B_ThrowBlock;

friend void RunGame();
friend void shakeScreen();
};
