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
// class fireball
//------------------------------------------------------------------------------
MO_Fireball::MO_Fireball(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() >> 2, (short)nspr->getHeight() >> 3, 0, 0)
{
    if (moveToRight)
        velx = 5.0f;
    else
        velx = -5.0f;

    //fireball sprites have both right and left sprites in them
    ih = ih >> 3;

    bounce = -FIREBALLBOUNCE;

    iPlayerID = iGlobalID;
    iTeamID = teamID;

    colorOffset = iColorID * 36;
    movingObjectType = movingobject_fireball;

    state = 1;

    ttl = game_values.fireballttl;

    sSpotlight = NULL;
}

void MO_Fireball::update()
{
    IO_MovingObject::update();

    if (--ttl <= 0) {
        removeifprojectile(this, true, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

bool MO_Fireball::collide(CPlayer * player)
{
    if (iPlayerID != player->getGlobalID() && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->getTeamID())) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                //Find the player that shot this fireball so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Fireball, false, false);
                return true;
            }
        }
    }

    return false;
}

void MO_Fireball::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, (velx > 0 ? 0 : 18) + colorOffset, iw, ih);
}


