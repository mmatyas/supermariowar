#include "IO_BulletBillCannon.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/moving/MO_BulletBill.h"

extern CObjectContainer objectcontainer[3];
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class IO_BulletBillCannon - gets update calls and shoots bullet bills based on timer
//------------------------------------------------------------------------------

IO_BulletBillCannon::IO_BulletBillCannon(Vec2s pos, short freq, float vel, bool preview)
    : CObject(NULL, pos.x, pos.y)
{
    iFreq = freq;
    dVel = vel;
    fPreview = preview;

    objectType = object_bulletbillcannon;

    SetNewTimer();
}

void IO_BulletBillCannon::update()
{
    if (--iTimer <= 0) {
        SetNewTimer();

        objectcontainer[1].add(new MO_BulletBill(&rm->spr_hazard_bulletbill[fPreview ? 1 : 0], &rm->spr_hazard_bulletbilldead, {ix + (dVel < 0.0f ? 32 : -32), iy}, dVel, 0, true));
        ifSoundOnPlay(rm->sfx_bulletbillsound);
    }
}

void IO_BulletBillCannon::SetNewTimer()
{
    iTimer = iFreq + RANDOM_INT(iFreq);
}
