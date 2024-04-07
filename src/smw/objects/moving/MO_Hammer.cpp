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
// class hammer
//------------------------------------------------------------------------------
MO_Hammer::MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short teamID, short iColorID, bool superHammer) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() >> 2, 0, 0)
{
    ih = ih >> 2;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
	//RFC
    colorOffset = iColorID * 28;
    movingObjectType = movingobject_hammer;

    state = 1;

    velx = fVelyX;
    vely = fVelyY;
    ttl = game_values.hammerttl;

    fSuper = superHammer;

    if (velx > 0.0f)
        drawframe = 0;
    else
        drawframe = animationWidth - iw;

    fObjectCollidesWithMap = false;

    sSpotlight = NULL;
}

void MO_Hammer::update()
{
    if (++animationtimer == animationspeed) {
        animationtimer = 0;

        if (velx > 0) {
            drawframe += iw;
            if (drawframe >= animationWidth)
                drawframe = 0;
        } else {
            drawframe -= iw;
            if (drawframe < 0)
                drawframe = animationWidth - iw;
        }
    }

    setXf(fx + velx);
    setYf(fy + vely);

    if (!fSuper)
        vely += GRAVITATION;

    if (ix < 0)
        setXi(ix + App::screenWidth);
    else if (ix > App::screenWidth - 1)
        setXi(ix - App::screenWidth);

    if (iy > App::screenHeight || --ttl <= 0 || (fSuper && iy < -ih)) {
        removeifprojectile(this, false, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }

    //Detection collision with hammer breakable blocks
    IO_Block * blocks[4];
    GetCollisionBlocks(blocks);
    for (short iBlock = 0; iBlock < 4; iBlock++) {
        if (blocks[iBlock] && blocks[iBlock]->getBlockType() == BlockType::WeaponBreakable) {
            B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
            if (weaponbreakableblock->iType == 5) {
                weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
                removeifprojectile(this, false, false);
                return;
            }
        }
    }
}

bool MO_Hammer::collide(CPlayer * player)
{
    if (iPlayerID != player->globalID && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->teamID)) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                //Find the player that shot this hammer so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Hammer, false, false);
                return true;
            }
        }
    }

    return false;
}

void MO_Hammer::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset, iw, ih);
}


