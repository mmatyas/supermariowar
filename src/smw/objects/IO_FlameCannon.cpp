#include "IO_FlameCannon.h"

#include "GameValues.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

extern SDL_Rect g_rFlameRects[4][4];
extern CResourceManager* rm;


//------------------------------------------------------------------------------
// class IO_FlameCannon - shoots a flame
//------------------------------------------------------------------------------
IO_FlameCannon::IO_FlameCannon(short x, short y, short freq, short direction)
    : CObject(NULL, x, y)
{
    iFreq = freq;
    state = 0;
    SetNewTimer();

    objectType = object_flamecannon;

    iw = g_rFlameRects[direction][0].w;
    ih = g_rFlameRects[direction][0].h;

    collisionHeight = ih;
    collisionWidth = iw;
    collisionOffsetX = 0;
    collisionOffsetY = 0;

    iDirection = direction;

    iFrame = 0;

    if (iDirection == 1) {
        ix -= 64;
    } else if (iDirection == 2) {
        iy -= 64;
    }
}

void IO_FlameCannon::update()
{
    if (state == 0) {  // No flame, waiting
        if (--iTimer <= 0) {
            iTimer = 0;
            iCycle = 0;
            iFrame = 0;

            state = 1;
            ifSoundOnPlay(rm->sfx_flamecannon);
        }
    } else if (state == 1 || state == 3) {  // Start or end of flame but not deadly yet
        if (++iTimer >= 4) {
            iTimer = 0;

            if (++iFrame > 1) {
                iFrame = 0;

                if (++iCycle >= 4) {
                    iFrame = 2;
                    iCycle = 0;

                    if (state == 1) {
                        state = 2;
                    } else {
                        state = 0;
                        SetNewTimer();
                    }
                }
            }
        }
    } else if (state == 2) {  // Full flame
        if (++iTimer >= 4) {
            iTimer = 0;

            if (++iFrame > 3) {
                iFrame = 2;

                if (++iCycle >= 8) {
                    state = 3;
                    iFrame = 0;
                    iCycle = 0;
                }
            }
        }
    }
}

void IO_FlameCannon::draw()
{
    if (state > 0) {
        const SDL_Rect* rect = &g_rFlameRects[iDirection][iFrame];
        rm->spr_hazard_flame[0].draw(ix, iy, rect->x, rect->y, rect->w, rect->h);
    }
}

// For preview
void IO_FlameCannon::draw(short iOffsetX, short iOffsetY)
{
    if (state > 0) {
        const SDL_Rect* rect = &g_rFlameRects[iDirection][iFrame];
        gfx_drawpreview(rm->spr_hazard_flame[1].getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, rect->x >> 1, rect->y >> 1, rect->w >> 1, rect->h >> 1, iOffsetX, iOffsetY, 320, 240, true);
    }
}

bool IO_FlameCannon::collide(CPlayer* player)
{
    if (state == 2 && !player->isInvincible() && !player->isShielded() && !player->shyguy)
        return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;

    return false;
}

void IO_FlameCannon::SetNewTimer()
{
    iTimer = iFreq + RANDOM_INT(iFreq);
}
