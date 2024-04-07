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

extern IO_MovingObject * createpowerup(short iType, short ix, short iy, bool side, bool spawn);

//------------------------------------------------------------------------------
// class throwable box - can be used as a shield, thrown at a player, or holds items
//------------------------------------------------------------------------------
CO_ThrowBox::CO_ThrowBox(gfxSprite * nspr, short x, short y, short item) :
    MO_CarriedObject(nspr, x, y, 4, 8, 30, 30, 1, 1)
{
    state = 1;
    ih = 32;
    iw = 32;

    movingObjectType = movingobject_throwbox;

    iPlayerID = -1;
    iTeamID = -1;

    iItem = item;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;

    frozen = false;
    frozentimer = 0;
    frozenanimationspeed = 8;

    sSpotlight = NULL;
}

bool CO_ThrowBox::collide(CPlayer * player)
{
    //Kill the player if it is moving

    if (frozen) {
        ShatterDie();
        return false;
    }

    if (HasKillVelocity() && player->globalID != iPlayerID)
        return KillPlayer(player);

    /*
    //Kill player when another player is holding the box
    else
    {
        if (owner && player != owner && (game_values.teamcollision == TeamCollisionStyle::On || player->teamID != owner->teamID))
    	{
    		iPlayerID = owner->globalID;
    		iTeamID = owner->teamID;
    		return KillPlayer(player);
    	}
    }*/

    //Otherwise allow them to pick this box up
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
        }
    }

    return false;
}


bool CO_ThrowBox::KillPlayer(CPlayer * player)
{
    if (player->isInvincible() || player->shyguy) {
        Die();
        return false;
    }

    if (player->isShielded())
        return false;

    Die();

    //Find the player that shot this shell so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::ThrowBlock, false, false);
    return true;
}

void CO_ThrowBox::collide(IO_MovingObject * object)
{
    if (object->isDead())
        return;

    removeifprojectile(object, false, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_throwbox) {
        CO_ThrowBox * box = (CO_ThrowBox*)object;
        if (frozen || box->frozen || HasKillVelocity() || box->HasKillVelocity()) {
            Die();
            box->Die();
        }
    } else if (type == movingobject_explosion || type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_superfireball || type == movingobject_sledgehammer) {
        Die();
    } else if (type == movingobject_iceblast) {
        velx = 0.0f;
        animationspeed = 0;

        frozen = true;
        frozentimer = 300;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
    }
}

void CO_ThrowBox::update()
{
    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            animationspeed = frozenanimationspeed;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
        }
    }

    fOldX = fx;
    fOldY = fy;

    if (owner) {
        MoveToOwner();
        inair = true;
    } else {
        applyfriction();
        collision_detection_map();
    }

    animate();

    if (game_values.spotlights && HasKillVelocity()) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

void CO_ThrowBox::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    }
}

void CO_ThrowBox::Drop()
{
    if (owner) {
        iPlayerID = owner->globalID;
        iTeamID = owner->teamID;
    }

    if (collision_detection_checksides())
        Die();
    else
        MO_CarriedObject::Drop();
}

void CO_ThrowBox::Kick()
{
    if (owner) {
        iPlayerID = owner->globalID;
        iTeamID = owner->teamID;
    }

    if (collision_detection_checksides())
        Die();
    else
        MO_CarriedObject::Kick();
}

void CO_ThrowBox::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 16, 16));

    DieHelper();
}

void CO_ThrowBox::ShatterDie()
{
    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;

    DieHelper();
}

void CO_ThrowBox::DieHelper()
{
    dead = true;
    ifSoundOnPlay(rm->sfx_breakblock);

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }

    //Check to see if we should spawn an item here
    if (iItem != NO_POWERUP) {
        createpowerup(iItem, ix, iy, velx < 0.0f, false);
    }
}

void CO_ThrowBox::SideBounce(bool fRightSide)
{
    if (dead)
        return;

    if (HasKillVelocity()) {
        if (frozen)
            ShatterDie();
        else
            Die();
    }
}

float CO_ThrowBox::BottomBounce()
{
    if (dead)
        return bounce;

    if (HasKillVelocity()) {
        if (frozen)
            ShatterDie();
        else
            Die();
    }

    return bounce;
}

bool CO_ThrowBox::HasKillVelocity()
{
    return velx < -0.01f || velx > 0.01f || vely < -0.01f || vely > 2.0f;
}

