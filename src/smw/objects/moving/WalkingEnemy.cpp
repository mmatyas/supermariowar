#include "objectgame.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "movingplatform.h"
#include "ObjectContainer.h"
#include "objecthazard.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "gamemodes/BonusHouse.h"
#include "gamemodes/Chase.h"
#include "gamemodes/Race.h"
#include "gamemodes/Star.h"
#include "gamemodes/MiniBoss.h"
#include "gamemodes/MiniBoxes.h"
#include "gamemodes/MiniPipe.h"

#include <cstdlib> // abs()
#include <cmath>

extern short iKingOfTheHillZoneLimits[4][4];
extern void PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, short KillStyle, bool fForce, bool fKillCarriedItem);
extern void PlayerKilledPlayer(CPlayer * killer, CPlayer * killed, short deathstyle, short KillStyle, bool fForce, bool fKillCarriedItem);
extern bool SwapPlayers(short iUsingPlayerID);
extern short scorepowerupoffsets[3][3];

extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);
extern void CheckSecret(short id);
extern SpotlightManager spotlightManager;
extern CObjectContainer objectcontainer[3];

extern CGM_Pipe_MiniGame * pipegamemode;

extern CPlayer* list_players[4];
extern short score_cnt;

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class walking enemy (base class for goomba and koopa)
//------------------------------------------------------------------------------
MO_WalkingEnemy::MO_WalkingEnemy(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth, bool moveToRight, bool killOnWeakWeapon, bool bouncing, bool fallOffLedges) :
    IO_MovingObject(nspr, 0, 0, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    if (moveToRight)
        velx = 1.0f;
    else
        velx = -1.0f;

    movingObjectType = movingobject_none;

    fBouncing = bouncing;
    if (fBouncing)
        bounce = -VELENEMYBOUNCE;
    else
        bounce = GRAVITATION;

    spawnradius = 100.0f;
    spawnangle = (float)(RANDOM_INT(1000) * 0.00628f);
    inair = true;

    iSpawnIconOffset = 0;

    burnuptimer = 0;

    fKillOnWeakWeapon = killOnWeakWeapon;

    frozen = false;
    frozentimer = 0;
    frozenvelocity = velx;
    frozenanimationspeed = aniSpeed;

    fFallOffLedges = fallOffLedges;

    place();
}

void MO_WalkingEnemy::draw()
{
    if (state == 0) {
        short numeyecandy = 8;
        float addangle = TWO_PI / numeyecandy;
        float displayangle = spawnangle;

        for (short k = 0; k < numeyecandy; k++) {
            short spawnX = ix + (collisionWidth >> 1) - 8 + (short)(spawnradius * cos(displayangle));
            short spawnY = iy + (collisionHeight >> 1) - 8 + (short)(spawnradius * sin(displayangle));

            displayangle += addangle;

            rm->spr_awardsouls.draw(spawnX, spawnY, iSpawnIconOffset, 0, 16, 16);
        }
    } else {
        IO_MovingObject::draw();

        if (frozen) {
            rm->spr_iceblock.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
        }
    }
}

void MO_WalkingEnemy::update()
{
    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            velx = frozenvelocity;
            animationspeed = frozenanimationspeed;

            if (fBouncing)
                bounce = -VELENEMYBOUNCE;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 3, 8));
        }
    }

    if (state == 0) {
        spawnradius -= 2.0f;
        spawnangle += 0.05f;

        if (spawnradius < 10.0f)
            state = 1;
    } else {
        IO_MovingObject::update();
    }

    //Deal with terminal burnup velocity
    if (vely >= MAXVELY) {
        if (++burnuptimer > 20) {
            if (burnuptimer > 80)
                KillObjectMapHazard();
            else
                eyecandy[0].add(new EC_SingleAnimation(&rm->spr_burnup, ix + (collisionWidth >> 1) - 16, iy + (collisionHeight >> 1) - 16, 5, 4));
        }
    } else {
        burnuptimer = 0;
    }

    //If this enemy doesn't fall off of ledges, then take a look at the area in front of them
    //to determine if they need to turn around
    if (!inair && !fFallOffLedges) {
        short probeCenterX = ix + (collisionWidth >> 1);
        short probeFrontX = ix + (velx > 0.0f ? collisionWidth + 1 : -1);
        short probeY = iy + collisionHeight + 5;

        if (platform) {
            int iFrontTileType = platform->GetTileTypeFromCoord(probeFrontX, probeY);
            int iCenterTileType = platform->GetTileTypeFromCoord(probeCenterX, probeY);

            bool fFrontGap = iFrontTileType == tile_flag_nonsolid || iFrontTileType == tile_flag_super_death_top;
            bool fCenterGap = iCenterTileType == tile_flag_nonsolid || iCenterTileType == tile_flag_super_death_top;

            //If there is a hole or the type will kill the enemy, then turn around
            if (fFrontGap && fCenterGap) {
                velx = -velx;
            }
        } else {
            if (probeFrontX >= App::screenWidth) {
                probeFrontX -= App::screenWidth;
            } else if (probeFrontX < 0) {
                probeFrontX += App::screenWidth;
            }

            if (probeCenterX >= App::screenWidth) {
                probeCenterX -= App::screenWidth;
            } else if (probeCenterX < 0) {
                probeCenterX += App::screenWidth;
            }

            if (probeFrontX >= 0 && probeFrontX < App::screenWidth && probeCenterX >= 0 && probeCenterX < App::screenWidth && probeY >= 0 && probeY < App::screenHeight) {
                probeFrontX /= TILESIZE;
                probeCenterX /= TILESIZE;
                probeY /= TILESIZE;

                IO_Block * frontBlock = g_map->block(probeFrontX, probeY);
                IO_Block * centerBlock = g_map->block(probeCenterX, probeY);

                bool fFoundFrontBlock = frontBlock && !frontBlock->isTransparent() && !frontBlock->isHidden();
                bool fFoundCenterBlock = centerBlock && !centerBlock->isTransparent() && !centerBlock->isHidden();

                if (!fFoundFrontBlock && !fFoundCenterBlock) {
                    int frontTile = g_map->map(probeFrontX, probeY);
                    int centerTile = g_map->map(probeCenterX, probeY);

                    bool fFrontGap = (frontTile & tile_flag_super_death_top) || (!(frontTile & tile_flag_solid) && !(frontTile & tile_flag_solid_on_top));
                    bool fCenterGap = (centerTile & tile_flag_super_death_top) || (!(centerTile & tile_flag_solid) && !(centerTile & tile_flag_solid_on_top));

                    if (fFrontGap && fCenterGap) {
                        velx = -velx;
                    }
                }
            }
        }
    }
}


bool MO_WalkingEnemy::collide(CPlayer * player)
{
    if (state == 0)
        return false;

    if (player->isInvincible() || frozen) {
        player->AddKillerAward(NULL, killStyle);

        if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
            player->Score().AdjustScore(1);

        if (frozen) {
            ShatterDie();
        } else {
            ifSoundOnPlay(rm->sfx_kicksound);
            Die();
        }
    } else {
        if (player->fOldY + PH <= fOldY && player->iy + PH >= iy)
            return hittop(player);
        else
            return hitother(player);
    }

    return false;
}

bool MO_WalkingEnemy::hitother(CPlayer * player)
{
    if (player->isShielded())
        return false;

    return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
}

void MO_WalkingEnemy::collide(IO_MovingObject * object)
{
    if (state == 0)
        return;

    if (!object->isDead()) {
        removeifprojectile(object, false, false);

        MovingObjectType type = object->getMovingObjectType();

        if (((type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang) && (fKillOnWeakWeapon || frozen)) || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_bulletbill || type == movingobject_podobo || type == movingobject_attackzone || type == movingobject_explosion || type == movingobject_sledgehammer) {
            //Don't kill enemies with non-moving shells
            if (type == movingobject_shell && object->state == 2)
                return;

            //Don't kill enemies with slow or non-moving boxes
            if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
                return;

            if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover) {
                //Find the player that shot this fireball so we can attribute a kill
                CPlayer * killer = GetPlayerFromGlobalID(object->iPlayerID);

                if (killer) {
                    killer->AddKillerAward(NULL, killStyle);
                    killer->Score().AdjustScore(1);

                    if (type == movingobject_shell)
                        ((CO_Shell*)object)->AddMovingKill(killer);
                }
            }

            if (frozen) {
                ShatterDie();
            } else {
                ifSoundOnPlay(rm->sfx_kicksound);

                if (type == movingobject_attackzone)
                    DieAndDropShell(true, false);
                else
                    Die();
            }

            if (type == movingobject_shell || type == movingobject_throwblock) {
                object->CheckAndDie();
            } else if (type == movingobject_bulletbill || type == movingobject_attackzone || type == movingobject_throwbox) {
                object->Die();
            }
        } else if (type == movingobject_iceblast) {
            frozenvelocity = velx;
            velx = 0.0f;
            animationspeed = 0;

            bounce = GRAVITATION;

            frozen = true;
            frozentimer = 300;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 3, 8));
        }
    }
}


void MO_WalkingEnemy::place()
{
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
    fx = (float)ix;
    fy = (float)iy;
}

void MO_WalkingEnemy::ShatterDie()
{
    ifSoundOnPlay(rm->sfx_breakblock);
    dead = true;

    short iBrokenIceX = ix - collisionOffsetX + iw - 32, iBrokenIceY = iy - collisionOffsetY + ih - 32;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;
}


