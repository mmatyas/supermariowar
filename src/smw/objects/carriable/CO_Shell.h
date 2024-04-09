#pragma once

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;
class Spotlight;

class CO_Shell : public MO_CarriedObject {
public:
    CO_Shell(short type, short x, short y, bool dieOnMovingPlayerCollision, bool dieOnHoldingPlayerCollision, bool dieOnFire, bool killBouncePlayer);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

    bool HitTop(CPlayer* player);
    bool HitOther(CPlayer* player);

    void UsedAsStoredPowerup(CPlayer* player);

    bool KillPlayer(CPlayer* player);
    void Drop() override;
    void Kick() override;

    void collide(IO_MovingObject* object) override;
    void CheckAndDie() override;
    void Die() override;
    void ShatterDie();

    void SideBounce(bool fRightSide) override;
    void AddMovingKill(CPlayer* killer);

    bool IsThreat() const
    {
        return state == 1 || state == 3;
    }

    void Flip();

    void nospawn(short y, bool fBounce);

private:
    void Stop();

    short iShellType;

    short iIgnoreBounceTimer;
    short iDestY;

    bool fDieOnMovingPlayerCollision;
    bool fDieOnHoldingPlayerCollision;
    bool fDieOnFire;
    bool fKillBouncePlayer;

    short iDeathTime;
    short iNoOwnerKillTime;

    short iColorOffsetY;
    short iBounceCounter;
    short iKillCounter;

    bool fFlipped;
    short iFlippedOffset;

    bool frozen;
    short frozentimer;
    float frozenvelocity;
    short frozenanimationspeed;

    Spotlight* sSpotlight;

friend class MO_AttackZone;
friend class MO_SpinAttack;

friend void RunGame();
friend void shakeScreen();
};
