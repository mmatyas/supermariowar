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
// class powerup
//------------------------------------------------------------------------------
MO_Powerup::MO_Powerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    desty = fy - collisionHeight;
    movingObjectType = movingobject_powerup;
}

void MO_Powerup::draw()
{
    if (state == 0)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, (short)(ih - fy + desty));
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);
}


void MO_Powerup::update()
{
    //Have the powerup grow out of the powerup block
    if (state == 0) {
        setYf(fy - 2.0f);

        if (fy <= desty) {
            fy = desty;
            state = 1;
            vely = 1.0f;
        }
    } else { //Then have it obey the physics
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();
}

bool MO_Powerup::collide(CPlayer *)
{
    if (state > 0)
        dead = true;

    return false;
}

void MO_Powerup::nospawn(short y)
{
    state = 1;
    setYi(y);
    vely = -VELJUMP / 2.0;
}

