#include "CO_KuriboShoe.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CEyecandyContainer eyecandy[3];
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class kuribo's shoe
//------------------------------------------------------------------------------
CO_KuriboShoe::CO_KuriboShoe(gfxSprite* nspr, short iX, short iY, bool sticky)
    : CO_Spring(nspr, iX, iY + 15, false)
{
    iw = 32;
    ih = 32;

    collisionOffsetY = 15;
    collisionHeight = 16;

    animationOffsetX = sticky ? 64 : 0;

    movingObjectType = movingobject_carried;

    fSticky = sticky;
}

void CO_KuriboShoe::hittop(CPlayer* player)
{
    if (!player->kuriboshoe.is_on() && player->tanookisuit.notStatue()) {
        dead = true;
        player->SetKuriboShoe(fSticky ? STICKY : NORMAL);
        ifSoundOnPlay(rm->sfx_transform);
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->ix + HALFPW - 16, player->iy + HALFPH - 16, 3, 8));
    }
}
