#include "MO_PirhanaPlant.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_ThrowBox.h"
#include "objects/overmap/WO_StraightPathHazard.h"

#include <cmath>

extern CEyecandyContainer eyecandy[3];
extern CObjectContainer objectcontainer[3];
extern CPlayer* list_players[4];
extern short list_players_cnt;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class IO_PirhanaPlant - pirhana plant that appears on a certain frequency
//------------------------------------------------------------------------------
extern SDL_Rect g_rPirhanaRects[4][4][4];

MO_PirhanaPlant::MO_PirhanaPlant(short x, short y, short type, short freq, short direction, bool preview)
    : IO_MovingObject(NULL, x, y, 1, 0)
{
    iType = type;
    iDirection = direction;
    iFreq = freq;

    fPreview = preview;

    movingObjectType = movingobject_pirhanaplant;

    // iHiddenPlane = y;
    // iHiddenDirection = 2 - ((direction / 2) * 2);

    state = 0;
    SetNewTimer();

    if (iDirection <= 1)
        iw = 32;
    else
        ih = 32;

    if (direction == 0)
        iy += 32;
    else if (direction == 2)
        ix += 32;

    /*
    if (iType == 0 || iType == 1)
    {
        iSrcX = iDirection * 128;
        iSrcY = iType * 48;
    }
    else if (iType == 2 || iType == 3)
    {
        iSrcX = iDirection * 64;
        iSrcY = iType * 48 + (iType == 3 ? 16 : 0);
    }
    */

    if (iDirection <= 1) {
        if (iType == 2)
            ih = 64;
        else
            ih = 48;
    } else {
        if (iType == 2)
            iw = 64;
        else
            iw = 48;
    }

    if (iDirection <= 1) {
        collisionHeight = 0;
        collisionWidth = 32;
    } else {
        collisionHeight = 32;
        collisionWidth = 0;
    }

    collisionOffsetX = 0;
    collisionOffsetY = 0;

    iAnimationTimer = 0;
    // iAnimationX = 0;

    iActionTimer = RANDOM_INT(8);
    iFrame = 0;

    fObjectCollidesWithMap = false;
}

void MO_PirhanaPlant::update()
{
    // Needed for collisions with player and kuribo's shoe to know
    // if the plant hit the player from the top or not
    fOldY = (float)iy;

    if (state == 0) {  // waiting to appear
        if (--iTimer <= 0) {
            iTimer = 0;
            state = 1;
        }
    } else if (state == 1) {  // appearing
        if (iDirection <= 1)
            collisionHeight += 2;
        else
            collisionWidth += 2;

        if (iDirection == 0)
            iy -= 2;
        else if (iDirection == 2)
            ix -= 2;

        if ((iDirection <= 1 && collisionHeight >= ih) || (iDirection >= 2 && collisionWidth >= iw))
            state = 2;
    } else if (state == 2) {  // extended
        if (++iTimer > 60) {
            iTimer = 0;
            state = 3;
        }
    } else if (state == 3) {  // retreating
        if (iDirection <= 1)
            collisionHeight -= 2;
        else
            collisionWidth -= 2;

        if (iDirection == 0)
            iy += 2;
        else if (iDirection == 2)
            ix += 2;

        if ((iDirection <= 1 && collisionHeight <= 0) || (iDirection >= 2 && collisionWidth <= 0)) {
            state = 0;
            SetNewTimer();
        }
    }


    if (iType == 1) {  // face the plant towards the nearest player
        // Don't do this every frame, just once every 8 frames
        if (state > 0 && ++iActionTimer >= 8) {
            int distance_to_player = App::screenWidth * 1000;
            short iDiffX = 1, iDiffY = 1;

            short iPlantX = ix + 16;
            short iPlantY = iy + (iDirection == 0 ? 16 : ih - 16);

            for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
                CPlayer* player = list_players[iPlayer];
                if (player->state != player_ready)
                    continue;

                // Calculate normal screen distance
                short tx = iPlantX - player->ix - PW;
                short ty = iPlantY - player->iy - PH;

                int distance_player_pow2 = tx * tx + ty * ty;

                if (distance_player_pow2 < distance_to_player) {
                    distance_to_player = distance_player_pow2;
                    iDiffX = tx;
                    iDiffY = ty;
                }
            }

            float dAngle = (float)atan2((double)iDiffX, (double)iDiffY);

            if (dAngle >= 0.0f && dAngle < HALF_PI)
                iFrame = 0;
            else if (dAngle >= HALF_PI && dAngle <= PI)
                iFrame = iDirection <= 1 ? 1 : 2;
            else if (dAngle >= -HALF_PI && dAngle < 0.0f)
                iFrame = iDirection <= 1 ? 2 : 1;
            else if (dAngle >= -PI && dAngle < -HALF_PI)
                iFrame = 3;
        }
    } else if (iType == 2 || iType == 3) {  // Animate if these are animated plants
        if (++iAnimationTimer >= 8) {
            iAnimationTimer = 0;

            if (++iFrame > 1)
                iFrame = 0;
        }
    }

    // Fire a fireball
    if (iType <= 1 && state == 2 && iTimer == 30) {
        objectcontainer[1].add(new OMO_StraightPathHazard(&rm->spr_hazard_fireball[fPreview ? 1 : 0], iDirection != 3 ? ix + 7 : ix + iw - 23, iDirection != 1 ? iy + 7 : iy + ih - 23, GetFireballAngle(), 3.0f, 4, 8, 18, 18, 0, 0, 0, iFrame <= 1 ? 18 : 0, 18, 18));
    }
}

void MO_PirhanaPlant::draw()
{
    if (state > 0) {
        SDL_Rect* rect = &g_rPirhanaRects[iType][iDirection][iFrame];
        if (iDirection == 0)
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x, rect->y, 32, collisionHeight);
        else if (iDirection == 1)
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x, rect->y + ih - collisionHeight, 32, collisionHeight);
        else if (iDirection == 2)
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x, rect->y, collisionWidth, 32);
        else
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x + iw - collisionWidth, rect->y, collisionWidth, 32);

        // SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
        // SDL_FillRect(blitdest, &r, 0xf000);
    }
}

// For preview drawing
void MO_PirhanaPlant::draw(short iOffsetX, short iOffsetY)
{
    if (state > 0) {
        SDL_Rect* rect = &g_rPirhanaRects[iType][iDirection][iFrame];
        if (iDirection == 0)
            gfx_drawpreview(rm->spr_hazard_pirhanaplant[1].getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, rect->x >> 1, rect->y >> 1, 16, collisionHeight >> 1, iOffsetX, iOffsetY, 320, 240, true);
        else if (iDirection == 1)
            gfx_drawpreview(rm->spr_hazard_pirhanaplant[1].getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, rect->x >> 1, (rect->y + ih - collisionHeight) >> 1, 16, collisionHeight >> 1, iOffsetX, iOffsetY, 320, 240, true);
        else if (iDirection == 2)
            gfx_drawpreview(rm->spr_hazard_pirhanaplant[1].getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, rect->x >> 1, rect->y >> 1, collisionWidth >> 1, 16, iOffsetX, iOffsetY, 320, 240, true);
        else
            gfx_drawpreview(rm->spr_hazard_pirhanaplant[1].getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, (rect->x + iw - collisionWidth) >> 1, rect->y >> 1, collisionWidth >> 1, 16, iOffsetX, iOffsetY, 320, 240, true);
    }
}

bool MO_PirhanaPlant::collide(CPlayer* player)
{
    if (state == 0)
        return false;

    bool fHitPlayerTop = fOldY + collisionHeight <= player->fOldY && iy + collisionHeight >= player->iy;

    if (player->isInvincible() || player->tanookisuit.isStatue() || (player->kuriboshoe.is_on() && !fHitPlayerTop)) {
        KillPlant();
    } else if (!player->isShielded() && !player->shyguy) {
        return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
    }

    return false;
}

void MO_PirhanaPlant::collide(IO_MovingObject* object)
{
    if (state == 0)
        return;

    removeifprojectile(object, true, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_attackzone || type == movingobject_explosion) {
        // Don't kill things with shells that are sitting still
        if (type == movingobject_shell && object->state == 2)
            return;

        // Don't kill things with boxesx that aren't moving fast enough
        if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
            return;

        if (type == movingobject_shell || type == movingobject_throwblock) {
            object->CheckAndDie();
        } else if (type == movingobject_attackzone || type == movingobject_throwbox) {
            object->Die();
        }

        KillPlant();
    }
}

void MO_PirhanaPlant::SetNewTimer()
{
    iTimer = iFreq + RANDOM_INT(iFreq);

    // Face the green fireball plant in a random direction
    if (iType == 0) {
        // Only point flower towards directions that make sense
        if ((ix >> 5) == 19)
            iFrame = RANDOM_INT(2);
        else if (ix == 0)
            iFrame = (RANDOM_INT(2)) + 2;
        else
            iFrame = RANDOM_INT(4);
    }
}

void MO_PirhanaPlant::KillPlant()
{
    if (state == 0)
        return;

    SetNewTimer();
    state = 0;

    ifSoundOnPlay(rm->sfx_kicksound);
    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + (iDirection == 2 ? 0 : collisionWidth - 32), iy + (iDirection == 0 ? 0 : collisionHeight - 32), 3, 4));

    if (iDirection == 0)
        iy += collisionHeight;
    else if (iDirection == 2)
        ix += collisionWidth;

    if (iDirection <= 1)
        collisionHeight = 0;
    else
        collisionWidth = 0;
}

float MO_PirhanaPlant::GetFireballAngle()
{
    if (iDirection <= 1) {
        if (iFrame == 0)
            return -2.7214f;
        else if (iFrame == 1)
            return 2.7214f;
        else if (iFrame == 2)
            return -0.4202f;
        else if (iFrame == 3)
            return 0.4202f;
    } else {

        if (iFrame == 0)
            return -1.9910f;
        else if (iFrame == 1)
            return -1.1506f;
        else if (iFrame == 2)
            return 1.9910f;
        else if (iFrame == 3)
            return 1.1506f;
    }

    return 0.0f;
}
