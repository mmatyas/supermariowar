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
// class CarriedObject - all objects players can carry inheirit from this class
//------------------------------------------------------------------------------
MO_CarriedObject::MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    init();
}

MO_CarriedObject::MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    init();
}

MO_CarriedObject::~MO_CarriedObject()
{
    if (owner)
        owner->carriedItem = NULL;
}

void MO_CarriedObject::init()
{
    owner = NULL;
    fSmoking = false;

    dKickX = 2.0f;
    dKickY = 4.0f;

    iOwnerRightOffset = HALFPW;
    iOwnerLeftOffset = HALFPW - 32;
    iOwnerUpOffset = 32;

    fCarriedByKuriboShoe = false;
}

void MO_CarriedObject::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}

void MO_CarriedObject::MoveToOwner()
{
    if (owner) {
        setXi(owner->ix + (owner->isFacingRight() ? iOwnerRightOffset : iOwnerLeftOffset));
        setYi(owner->iy + PH - iOwnerUpOffset + collisionOffsetY);
    }
}

void MO_CarriedObject::Drop()
{
    if (owner)
        owner->carriedItem = NULL;

    owner = NULL;

    collision_detection_checksides();
}

void MO_CarriedObject::Kick()
{
    if (owner) {
        velx = owner->velx + (owner->isFacingRight() ? dKickX : -dKickX);
        vely = -dKickY;
        ifSoundOnPlay(rm->sfx_kicksound);
    }

    Drop();
}

