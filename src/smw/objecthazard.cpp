#include "objecthazard.h"

#include "Game.h"
#include "GameValues.h"
#include "map.h"
#include "ObjectContainer.h"
#include "objectgame.h"
#include "player.h"
#include "PlayerKillTypes.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "eyecandy.h"

#include <cstring>
#include <cmath>


extern short LookupTeamID(short id);
extern void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead);

extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;
extern CGame* smw;


void LoadMapHazards(bool fPreview)
{
    //Make sure we don't have any objects created before we create them from the map settings
    noncolcontainer.clean();
    objectcontainer[0].clean();
    objectcontainer[1].clean();
    objectcontainer[2].clean();

    //Create objects for all the map hazards
    for (short iMapHazard = 0; iMapHazard < g_map->iNumMapHazards; iMapHazard++) {
        MapHazard * hazard = &g_map->maphazards[iMapHazard];
        if (hazard->itype == 0) {
            for (short iFireball = 0; iFireball < hazard->iparam[0]; iFireball++)
                objectcontainer[1].add(new OMO_OrbitHazard(&rm->spr_hazard_fireball[fPreview ? 1 : 0], (hazard->ix << 4) + 16, (hazard->iy << 4) + 16, (float)(iFireball * 24), hazard->dparam[0], hazard->dparam[1], 4, 8, 18, 18, 0, 0, 0, hazard->dparam[0] < 0.0f ? 18 : 0, 18, 18));
        } else if (hazard->itype == 1) {
            float dSector = TWO_PI / hazard->iparam[0];
            for (short iRotoDisc = 0; iRotoDisc < hazard->iparam[0]; iRotoDisc++) {
                float dAngle = hazard->dparam[1] + iRotoDisc * dSector;
                if (dAngle > TWO_PI)
                    dAngle -= TWO_PI;

                objectcontainer[1].add(new OMO_OrbitHazard(&rm->spr_hazard_rotodisc[fPreview ? 1 : 0], (hazard->ix << 4) + 16, (hazard->iy << 4) + 16, hazard->dparam[2], hazard->dparam[0], dAngle, 21, 8, 32, 32, 0, 0, 0, 0, 32, 32));
            }
        } else if (hazard->itype == 2) {
            noncolcontainer.add(new IO_BulletBillCannon(hazard->ix << 4, hazard->iy << 4, hazard->iparam[0], hazard->dparam[0], fPreview));
        } else if (hazard->itype == 3) {
            objectcontainer[1].add(new IO_FlameCannon(hazard->ix << 4, hazard->iy << 4, hazard->iparam[0], hazard->iparam[1]));
        } else if (hazard->itype >= 4 && hazard->itype <= 7) {
            objectcontainer[1].add(new MO_PirhanaPlant(hazard->ix << 4, hazard->iy << 4, hazard->itype - 4, hazard->iparam[0], hazard->iparam[1], fPreview));
        }
    }
}


//------------------------------------------------------------------------------
// class OMO Orbit Hazard - component of the fireball string or rotodisc
//------------------------------------------------------------------------------
OMO_OrbitHazard::OMO_OrbitHazard(gfxSprite *nspr, short x, short y, float radius, float vel, float angle, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
    IO_OverMapObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    objectType = object_orbithazard;

    dRadius = radius;
    dVel = vel;
    dAngle = angle;

    dCenterX = (float)x;
    dCenterY = (float)y;

    CalculatePosition();
}

void OMO_OrbitHazard::update()
{
    animate();

    dAngle += dVel;

    if (dAngle < 0.0f)
        dAngle += TWO_PI;
    else if (dAngle >= TWO_PI)
        dAngle -= TWO_PI;

    CalculatePosition();
}

bool OMO_OrbitHazard::collide(CPlayer * player)
{
    if (!player->isInvincible() && !player->isShielded() && !player->shyguy) {
        return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
    }

    return false;
}

void OMO_OrbitHazard::CalculatePosition()
{
    setXf(dCenterX + dRadius * cos(dAngle) - (float)iw / 2.0f);
    setYf(dCenterY + dRadius * sin(dAngle) - (float)ih / 2.0f);
}


//------------------------------------------------------------------------------
// class OMO Straight Path Hazard - straight path fireball
//------------------------------------------------------------------------------
OMO_StraightPathHazard::OMO_StraightPathHazard(gfxSprite *nspr, short x, short y, float angle, float vel, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
    IO_OverMapObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    objectType = object_pathhazard;

    dVel = vel;
    dAngle = angle;

    velx = vel * cos(angle);
    vely = vel * sin(angle);
}

void OMO_StraightPathHazard::update()
{
    IO_OverMapObject::update();

    if (iy + ih < 0 || iy >= smw->ScreenHeight)
        dead = true;

    //Wrap hazard if it is off the edge of the screen
    if (ix < 0)
        ix += smw->ScreenWidth;
    else if (ix + iw >= smw->ScreenWidth)
        ix -= smw->ScreenWidth;
}

bool OMO_StraightPathHazard::collide(CPlayer * player)
{
    if (!player->isShielded()) {
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + (iw >> 2) - 16, iy + (ih >> 2) - 16, 3, 8));
        dead = true;

        if (!player->isInvincible() && !player->shyguy) {
            return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
        }
    }

    return false;
}


//------------------------------------------------------------------------------
// class bulletbill
//------------------------------------------------------------------------------
MO_BulletBill::MO_BulletBill(gfxSprite *nspr, gfxSprite *nsprdead, short x, short y, float nspeed, short playerID, bool isspawned) :
    IO_MovingObject(nspr, x, y, 4, 8, 30, 28, 1, 2)
{
    spr_dead = nsprdead;

    velx = nspeed;
    vely = 0.0f;

    movingObjectType = movingobject_bulletbill;
    state = 1;

    fIsSpawned = isspawned;

    ih = 32;
    iw = 32;

    inair = true;

    if (fIsSpawned) {
        iPlayerID = -1;
        iColorID = 0;
        iTeamID = -1;

        animationspeed = 0;

        if (velx < 0.0f) {
            iHiddenDirection = 1;
            iHiddenPlane = ix;
        } else {
            iHiddenDirection = 3;
            iHiddenPlane = ix + TILESIZE;
        }
    } else {
        if (velx < 0.0f)
            setXi(smw->ScreenWidth + iw);
        else
            setXi(-iw);

        iPlayerID = playerID;
        iColorID = game_values.colorids[iPlayerID];
        iTeamID = LookupTeamID(iPlayerID);
    }

    iColorOffsetY = 64 * iColorID;
    SetDirectionOffset();

    fObjectCollidesWithMap = false;
}

void MO_BulletBill::update()
{
    setXf(fx + velx);
    //setYf(fy + vely);

    animate();

    if ((velx < 0.0f && ix < -iw) || (velx > 0.0f && ix > smw->ScreenWidth))
        dead = true;
}

void MO_BulletBill::draw()
{
    if (fIsSpawned)
        spr->draw(ix, iy, drawframe, iColorOffsetY + iDirectionOffsetY, iw, ih, iHiddenDirection, iHiddenPlane);
    else
        spr->draw(ix, iy, drawframe, iColorOffsetY + iDirectionOffsetY, iw, ih);
}

//For preview drawing
void MO_BulletBill::draw(short iOffsetX, short iOffsetY)
{
    if (fIsSpawned)
        gfx_drawpreview(spr->getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, drawframe >> 1, (iColorOffsetY + iDirectionOffsetY) >> 1, iw >> 1, ih >> 1, iOffsetX, iOffsetY, 320, 240, false, iHiddenDirection, (iHiddenPlane >> 1) + iOffsetX);
    else
        gfx_drawpreview(spr->getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, drawframe >> 1, (iColorOffsetY + iDirectionOffsetY) >> 1, iw >> 1, ih >> 1, iOffsetX, iOffsetY, 320, 240, false);
}

bool MO_BulletBill::collide(CPlayer * player)
{
    if (dead)
        return false;

    //if the bullet bill is off the screen, don't wrap it to collide
    if ((ix < 0 && velx < 0.0f && player->ix > ix + iw && player->ix + PW < smw->ScreenWidth) ||
            (ix + iw >= smw->ScreenWidth && velx > 0.0f && player->ix + PW < ix && player->ix >= 0)) {
        return false;
    }

    if (player->isInvincible() || player->shyguy) {
        player->AddKillerAward(NULL, kill_style_bulletbill);
        ifSoundOnPlay(rm->sfx_kicksound);

        Die();
    } else {
        if (player->fOldY + PH <= iy && player->iy + PH >= iy)
            return hittop(player);
        else
            return hitother(player);
    }

    return false;
}


bool MO_BulletBill::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    player->AddKillerAward(NULL, kill_style_bulletbill);

    ifSoundOnPlay(rm->sfx_mip);

    Die();

    return false;
}

bool MO_BulletBill::hitother(CPlayer * player)
{
    if (player->isShielded() || player->globalID == iPlayerID)
        return false;

    if (game_values.teamcollision != 2 && iTeamID == player->teamID)
        return false;

    //Find the player that owns this bullet bill so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_bulletbill, false, false);

    return true;
}

void MO_BulletBill::collide(IO_MovingObject * object)
{
    removeifprojectile(object, true, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_bulletbill) {
        MO_BulletBill * bulletbill = (MO_BulletBill*) object;

        //Same team bullet bills don't kill each other
        if (bulletbill->iTeamID == iTeamID)
            return;

        bulletbill->dead = true;
        dead = true;

        short iOffsetX = 0;
        if (ix + iw < bulletbill->ix)
            iOffsetX = smw->ScreenWidth;
        else if (bulletbill->ix + bulletbill->iw < ix)
            iOffsetX = -smw->ScreenWidth;

        short iCenterX = ((ix + iOffsetX - bulletbill->ix) >> 1) + (bulletbill->ix + (bulletbill->iw >> 1));
        short iCenterY = ((iy - bulletbill->iy) >> 1) + (bulletbill->iy + (bulletbill->ih >> 1));

        objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, iCenterX - 96, iCenterY - 64, 2, 4, -1, -1, kill_style_bulletbill));
        ifSoundOnPlay(rm->sfx_bobombsound);
    } else if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_attackzone || type == movingobject_explosion) {
        //Don't kill things with shells that are sitting still
        if (type == movingobject_shell && object->state == 2)
            return;

        //Don't kill things with boxesx that aren't moving fast enough
        if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
            return;

        if (type != movingobject_explosion) {
            object->Die();
        }

        ifSoundOnPlay(rm->sfx_kicksound);
        Die();
    }
}

void MO_BulletBill::Die()
{
    dead = true;
    eyecandy[2].add(new EC_FallingObject(spr_dead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, velx > 0 ? 0 : 32, iColorID * 32, 32, 32));
}

void MO_BulletBill::SetDirectionOffset()
{
    iDirectionOffsetY = velx < 0 ? 0 : 32;
}


//------------------------------------------------------------------------------
// class IO_BulletBillCannon - gets update calls and shoots bullet bills based on timer
//------------------------------------------------------------------------------

IO_BulletBillCannon::IO_BulletBillCannon(short x, short y, short freq, float vel, bool preview) :
    CObject(NULL, x, y)
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

        objectcontainer[1].add(new MO_BulletBill(&rm->spr_hazard_bulletbill[fPreview ? 1 : 0], &rm->spr_hazard_bulletbilldead, ix + (dVel < 0.0f ? 32 : -32), iy, dVel, 0, true));
        ifSoundOnPlay(rm->sfx_bulletbillsound);
    }
}

void IO_BulletBillCannon::SetNewTimer()
{
    iTimer = iFreq + RANDOM_INT(iFreq);
}


//------------------------------------------------------------------------------
// class explosion (for bob-omb mode)
//------------------------------------------------------------------------------
MO_Explosion::MO_Explosion(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short playerid, short teamid, killstyle style) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed)
{
    state = 1;

    iPlayerID = playerid;
    iTeamID = teamid;
    timer = 0;
    movingObjectType = movingobject_explosion;
    iStyle = style;

    fObjectCollidesWithMap = false;
}

bool MO_Explosion::collide(CPlayer * player)
{
    if (player->globalID != iPlayerID && (game_values.teamcollision == 2 || iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded() && !player->shyguy) {
        //Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, death_style_jump, iStyle, false, false);
        return true;
    }

    return false;
}

void MO_Explosion::update()
{
    animate();

    //If this is the first frame, look for blocks to kill
    if (timer == 0) {
        short iTestY = iy;

        for (short iRow = 0; iRow < 5; iRow++) {
            short iTestX = ix;

            if (iTestX < 0)
                iTestX += smw->ScreenWidth;

            if (iTestY >= 0 && iTestY < smw->ScreenHeight) {
                short iTestRow = iTestY / TILESIZE;
                for (short iCol = 0; iCol < 7; iCol++) {
                    IO_Block * block = g_map->block(iTestX / TILESIZE, iTestRow);
                    if (block && block->getBlockType() == block_weaponbreakable) {
                        B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)block;
                        if (weaponbreakableblock->iType == 3) {
                            weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
                        }
                    }

                    iTestX += TILESIZE;

                    if (iTestX >= smw->ScreenWidth)
                        iTestX -= smw->ScreenWidth;
                }
            }


            iTestY += TILESIZE;

            if (iTestY >= smw->ScreenHeight)
                break;
        }
    }

    //RFC: why 48?
    if (++timer >= 48)
        dead = true;
}


//------------------------------------------------------------------------------
// class IO_FlameCannon - shoots a flame
//------------------------------------------------------------------------------
extern SDL_Rect g_rFlameRects[4][4];
IO_FlameCannon::IO_FlameCannon(short x, short y, short freq, short direction) :
    CObject(NULL, x, y)
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
    if (state == 0) { //No flame, waiting
        if (--iTimer <= 0) {
            iTimer = 0;
            iCycle = 0;
            iFrame = 0;

            state = 1;
            ifSoundOnPlay(rm->sfx_flamecannon);
        }
    } else if (state == 1 || state == 3) { //Start or end of flame but not deadly yet
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
    } else if (state == 2) { //Full flame
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
        SDL_Rect * rect = &g_rFlameRects[iDirection][iFrame];
        rm->spr_hazard_flame[0].draw(ix, iy, rect->x, rect->y, rect->w, rect->h);
    }
}

//For preview
void IO_FlameCannon::draw(short iOffsetX, short iOffsetY)
{
    if (state > 0) {
        SDL_Rect * rect = &g_rFlameRects[iDirection][iFrame];
        gfx_drawpreview(rm->spr_hazard_flame[1].getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, rect->x >> 1, rect->y >> 1, rect->w >> 1, rect->h >> 1, iOffsetX, iOffsetY, 320, 240, true);
    }
}

bool IO_FlameCannon::collide(CPlayer * player)
{
    if (state == 2 && !player->isInvincible() && !player->isShielded() && !player->shyguy)
        return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;

    return false;
}

void IO_FlameCannon::SetNewTimer()
{
    iTimer = iFreq + RANDOM_INT(iFreq);
}


//------------------------------------------------------------------------------
// class IO_PirhanaPlant - pirhana plant that appears on a certain frequency
//------------------------------------------------------------------------------
extern SDL_Rect g_rPirhanaRects[4][4][4];

MO_PirhanaPlant::MO_PirhanaPlant(short x, short y, short type, short freq, short direction, bool preview) :
    IO_MovingObject(NULL, x, y, 1, 0)
{
    iType = type;
    iDirection = direction;
    iFreq = freq;

    fPreview = preview;

    movingObjectType = movingobject_pirhanaplant;

    //iHiddenPlane = y;
    //iHiddenDirection = 2 - ((direction / 2) * 2);

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
    //iAnimationX = 0;

    iActionTimer = RANDOM_INT(8);
    iFrame = 0;

    fObjectCollidesWithMap = false;
}

void MO_PirhanaPlant::update()
{
    //Needed for collisions with player and kuribo's shoe to know
    //if the plant hit the player from the top or not
    fOldY = (float)iy;

    if (state == 0) { //waiting to appear
        if (--iTimer <= 0) {
            iTimer = 0;
            state = 1;
        }
    } else if (state == 1) { //appearing
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
    } else if (state == 2) { //extended
        if (++iTimer > 60) {
            iTimer = 0;
            state = 3;
        }
    } else if (state == 3) { //retreating
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


    if (iType == 1) { //face the plant towards the nearest player
        //Don't do this every frame, just once every 8 frames
        if (state > 0 && ++iActionTimer >= 8) {
            int distance_to_player = smw->ScreenWidth * 1000;
            short iDiffX = 1, iDiffY = 1;

            short iPlantX = ix + 16;
            short iPlantY = iy + (iDirection == 0 ? 16 : ih - 16);

            for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
                CPlayer * player = list_players[iPlayer];
                if (player->state != player_ready)
                    continue;

                //Calculate normal screen distance
                short tx = iPlantX - player->ix - PW;
                short ty = iPlantY - player->iy - PH;

                int distance_player_pow2 = tx*tx + ty*ty;

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
    } else if (iType == 2 || iType == 3) { //Animate if these are animated plants
        if (++iAnimationTimer >= 8) {
            iAnimationTimer = 0;

            if (++iFrame > 1)
                iFrame = 0;
        }
    }

    //Fire a fireball
    if (iType <= 1 && state == 2 && iTimer == 30) {
        objectcontainer[1].add(new OMO_StraightPathHazard(&rm->spr_hazard_fireball[fPreview ? 1 : 0], iDirection != 3 ? ix + 7 : ix + iw - 23, iDirection != 1 ? iy + 7 : iy + ih - 23, GetFireballAngle(), 3.0f, 4, 8, 18, 18, 0, 0, 0, iFrame <= 1 ? 18 : 0, 18, 18));
    }
}

void MO_PirhanaPlant::draw()
{
    if (state > 0) {
        SDL_Rect * rect = &g_rPirhanaRects[iType][iDirection][iFrame];
        if (iDirection == 0)
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x, rect->y, 32, collisionHeight);
        else if (iDirection == 1)
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x, rect->y + ih - collisionHeight, 32, collisionHeight);
        else if (iDirection == 2)
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x, rect->y, collisionWidth, 32);
        else
            rm->spr_hazard_pirhanaplant[0].draw(ix, iy, rect->x + iw - collisionWidth, rect->y, collisionWidth, 32);

        //SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
        //SDL_FillRect(blitdest, &r, 0xf000);
    }
}

//For preview drawing
void MO_PirhanaPlant::draw(short iOffsetX, short iOffsetY)
{
    if (state > 0) {
        SDL_Rect * rect = &g_rPirhanaRects[iType][iDirection][iFrame];
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

bool MO_PirhanaPlant::collide(CPlayer * player)
{
    if (state == 0)
        return false;

    bool fHitPlayerTop = fOldY + collisionHeight <= player->fOldY && iy + collisionHeight >= player->iy;

    if (player->isInvincible() || player->tanookisuit.isStatue() || (player->kuriboshoe.is_on() && !fHitPlayerTop)) {
        KillPlant();
    } else if (!player->isShielded() && !player->shyguy) {
        return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
    }

    return false;
}

void MO_PirhanaPlant::collide(IO_MovingObject * object)
{
    if (state == 0)
        return;

    removeifprojectile(object, true, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_attackzone || type == movingobject_explosion) {
        //Don't kill things with shells that are sitting still
        if (type == movingobject_shell && object->state == 2)
            return;

        //Don't kill things with boxesx that aren't moving fast enough
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

    //Face the green fireball plant in a random direction
    if (iType == 0) {
        //Only point flower towards directions that make sense
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
