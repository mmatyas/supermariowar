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
    : CObject(NULL, pos)
    , m_freq(freq)
    , m_vel(vel)
    , m_preview(preview)
{
    objectType = object_bulletbillcannon;
    SetNewTimer();
}

void IO_BulletBillCannon::update()
{
    if (--m_timer <= 0) {
        SetNewTimer();

        const Vec2s pos(ix + (m_vel < 0.0f ? 32 : -32), iy);
        objectcontainer[1].add(new MO_BulletBill(&rm->spr_hazard_bulletbill[m_preview ? 1 : 0], &rm->spr_hazard_bulletbilldead, pos, m_vel, 0, true));
        ifSoundOnPlay(rm->sfx_bulletbillsound);
    }
}

void IO_BulletBillCannon::SetNewTimer()
{
    m_timer = m_freq + RANDOM_INT(m_freq);
}
