#include "MO_CarriedObject.h"

#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class CarriedObject - all objects players can carry inheirit from this class
//------------------------------------------------------------------------------
MO_CarriedObject::MO_CarriedObject(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : IO_MovingObject(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    init();
}

MO_CarriedObject::MO_CarriedObject(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth)
    : IO_MovingObject(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    init();
}

MO_CarriedObject::~MO_CarriedObject()
{
    if (owner)
        owner->carriedItem = NULL;
}

void MO_CarriedObject::init()
{
    owner = NULL;
    fSmoking = false;

    dKickX = 2.0f;
    dKickY = 4.0f;

    iOwnerRightOffset = HALFPW;
    iOwnerLeftOffset = HALFPW - 32;
    iOwnerUpOffset = 32;

    fCarriedByKuriboShoe = false;
}

void MO_CarriedObject::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}

void MO_CarriedObject::MoveToOwner()
{
    if (owner) {
        setXi(owner->ix + (owner->isFacingRight() ? iOwnerRightOffset : iOwnerLeftOffset));
        setYi(owner->iy + PH - iOwnerUpOffset + collisionOffsetY);
    }
}

void MO_CarriedObject::Drop()
{
    if (owner)
        owner->carriedItem = NULL;

    owner = NULL;

    collision_detection_checksides();
}

void MO_CarriedObject::Kick()
{
    if (owner) {
        velx = owner->velx + (owner->isFacingRight() ? dKickX : -dKickX);
        vely = -dKickY;
        ifSoundOnPlay(rm->sfx_kicksound);
    }

    Drop();
}
