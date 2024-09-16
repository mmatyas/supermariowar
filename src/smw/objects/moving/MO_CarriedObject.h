#pragma once

#include "MovingObject.h"


class gfxSprite;
class CPlayer;

class MO_CarriedObject : public IO_MovingObject {
public:
    MO_CarriedObject(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);
    MO_CarriedObject(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
    ~MO_CarriedObject();

    virtual void update() {}
    virtual void draw();
    virtual bool collide(CPlayer*) { return false; }

    virtual void MoveToOwner();

    virtual void Drop();
    virtual void Kick();

    bool HasOwner() { return owner != nullptr; }
    bool IsCarriedByKuriboShoe() { return fCarriedByKuriboShoe; }

protected:
    bool fCarriedByKuriboShoe;

    CPlayer* owner;
    bool fSmoking;

    float dKickX, dKickY;
    short iOwnerLeftOffset, iOwnerRightOffset, iOwnerUpOffset;

private:
    void init();

    friend class CPlayer;
};
