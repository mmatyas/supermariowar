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
// class boomerang
//------------------------------------------------------------------------------
MO_Boomerang::MO_Boomerang(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() >> 3, 0, 0)
{
    //boomerangs sprites have both right and left sprites in them
    ih = ih >> 3;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    colorOffset = iColorID * 64;
    movingObjectType = movingobject_boomerang;

    state = 1;

    fMoveToRight = moveToRight;

    if (moveToRight)
        velx = 5.0f;
    else
        velx = -5.0f;

    vely = 0.0f;

    fFlipped = false;

    //Don't let boomerang start off the screen or it won't rebound correctly
    if (moveToRight && fx + iw >= App::screenWidth)
        setXf(fx - App::screenWidth);
    else if (!moveToRight && fx < 0.0f)
        setXf(fx + App::screenWidth);

    iStateTimer = 0;

    if (game_values.boomerangstyle == 3)
        iStyle = RANDOM_INT(3);
    else
        iStyle = game_values.boomerangstyle;

    fObjectCollidesWithMap = false;

    sSpotlight = NULL;
}

void MO_Boomerang::update()
{
    if (!rm->sfx_boomerang.isPlaying())
        ifSoundOnPlay(rm->sfx_boomerang);

    animate();

    //Detection collision with boomerang breakable blocks
    IO_Block * blocks[4];
    GetCollisionBlocks(blocks);
    for (short iBlock = 0; iBlock < 4; iBlock++) {
        if (blocks[iBlock] && blocks[iBlock]->getBlockType() == BlockType::WeaponBreakable) {
            B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
            if (weaponbreakableblock->iType == 4) {
                weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
                forcedead();
                return;
            }
        }
    }

    if (iStyle == 0) { //Flat style
        fOldX = fx;
        setXf(fx + velx);

        if (fMoveToRight && fx + iw >= App::screenWidth && fOldX + iw < App::screenWidth) {
            if (fFlipped) {
                forcedead();
                return;
            } else {
                setXf(App::screenWidth - iw);
                fFlipped = true;
                fMoveToRight = false;
                velx = -velx;
            }
        } else if (!fMoveToRight && fx < 0.0f && fOldX >= 0.0f) {
            if (fFlipped) {
                forcedead();
                return;
            } else {
                setXf(0.0f);
                fFlipped = true;
                fMoveToRight = true;
                velx = -velx;
            }
        }
    } else if (iStyle == 1) {
        //Attempting to emulate the SMB3 boomerang behavior
        iStateTimer++;

        fOldX = fx;
        setXf(fx + velx);

        if (fx < 0.0f)
            setXf(fx + App::screenWidth);
        else if (fx + iw >= App::screenWidth)
            setXf(fx - App::screenWidth);

        if (state == 1) {
            fOldY = fy;
            setYf(fy - 3.2f);

            if (iStateTimer >= 20) {
                iStateTimer = 0;
                state = 2;
            }
        } else if (state == 2) {
            if (iStateTimer >= 26) {
                iStateTimer = 0;
                state = 3;
            }
        } else if (state == 3) {
            fOldY = fy;
            setYf(fy + 1.0f);

            if (fMoveToRight) {
                //Add amount so that by time fy lowers by two tiles, we turn around the boomerang
                velx -= 0.15625f;

                if (velx <= -5.0f) {
                    velx = -5.0f;
                    state = 4;
                    fFlipped = true;
                }
            } else {
                velx += 0.15625f;

                if (velx >= 5.0f) {
                    velx = 5.0f;
                    state = 4;
                    fFlipped = true;
                }
            }

            iStateTimer = 0;
        } else if (state == 4) {
            if (iStateTimer >= 46) {
                if ((fMoveToRight && fx < 0.0f && fOldX >= 0.0f) ||
                        (!fMoveToRight && fx + iw >= App::screenWidth && fOldX + iw < App::screenWidth)) {
                    forcedead();
                    return;
                }
            }
        }
    } else if (iStyle == 2) { //Zelda style boomerang
        iStateTimer++;

        fOldX = fx;
        setXf(fx + velx);

        if (fx < 0.0f)
            setXf(fx + App::screenWidth);
        else if (fx + iw >= App::screenWidth)
            setXf(fx - App::screenWidth);

        if (iStateTimer > game_values.boomeranglife) {
            forcedead();
            return;
        }

        if (state == 1) {
            if (iStateTimer >= 64) {
                state = 2;
                fFlipped = true;

                /*
                CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

                //No wrap boomerang
                if (player)
                {
                    if ((player->ix < ix && velx > 0) || (player->ix > ix && velx < 0))
                		velx = -velx;
                }
                else
                {
                	velx = -velx;
                }
                */

                velx = -velx;  //Wrap Boomerang
            }
        } else if (state == 2) {
            fOldY = fy;
            setYf(fy + vely);

            //Follow the player zelda style
            CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

            if (player) {
                bool fWrap = false;
                if (abs(player->ix - ix) > 320)
                    fWrap = true;

                if ((player->ix < ix && !fWrap) || (player->ix > ix && fWrap))  //Wrap Boomerang
                    //if (player->ix < ix)  //No Wrap Boomerang
                {
                    velx -= 0.2f;

                    if (velx < -5.0f)
                        velx = -5.0f;
                } else {
                    velx += 0.2f;

                    if (velx > 5.0f)
                        velx = 5.0f;
                }

                if (player->iy < iy) {
                    vely -= 0.2f;

                    if (vely < -3.0f)
                        vely = -3.0f;
                } else {
                    vely += 0.2f;

                    if (vely > 3.0f)
                        vely = 3.0f;
                }
            } else {
                //Remove boomerang if player was removed from game
                forcedead();
                return;

                /*
                //Die at nearest edge if player was removed from game
                if (velx > 0)
                	velx = 5.0f;
                else
                	velx = -5.0f;

                if ((fx < 0.0f && fOldX >= 0.0f) ||
                	(fx + iw >= App::screenWidth && fOldX + iw < App::screenWidth))
                {
                	forcedead();
                	return;
                }
                */
            }
        }
    }

    if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

//Call to kill boomerang when it is not caught by player
void MO_Boomerang::forcedead()
{
    removeifprojectile(this, false, true);
    ifsoundonstop(rm->sfx_boomerang);

    if (game_values.boomeranglimit == 0)
        return;

    //Penalize player if they did not catch it
    CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

    if (player) {
        if (player->projectilelimit > 0)
            player->DecreaseProjectileLimit();
    }
}

bool MO_Boomerang::collide(CPlayer * player)
{
    if (iPlayerID != player->globalID && (game_values.teamcollision == TeamCollisionStyle::On|| iTeamID != player->teamID)) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                //Find the player that shot this boomerang so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Boomerang, false, false);
                return true;
            }
        }
    } else if (iPlayerID == player->globalID && fFlipped) {
        removeifprojectile(this, false, true);
    }

    return false;
}

void MO_Boomerang::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset + (fMoveToRight ? 0 : 32), iw, ih);
}

