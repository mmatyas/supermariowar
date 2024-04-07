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
// class phanto key (for chase mode)
//------------------------------------------------------------------------------
CO_PhantoKey::CO_PhantoKey(gfxSprite *nspr) :
    MO_CarriedObject(nspr, 0, 0, 1, 0, 30, 30, 1, 1, 0, 0, 32, 32)
{
    state = 1;
    movingObjectType = movingobject_phantokey;

    iOwnerRightOffset = 12;
    iOwnerLeftOffset = -20;
    iOwnerUpOffset = 32;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    fCarriedByKuriboShoe = true;

    placeKey();
}

bool CO_PhantoKey::collide(CPlayer * player)
{
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
        }
    }

    return false;
}

void CO_PhantoKey::update()
{
    if (owner) {
        MoveToOwner();
        relocatetimer = 0;
    } else if (++relocatetimer > 1000) {
        placeKey();
    } else {
        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }
}

void CO_PhantoKey::draw()
{
    MO_CarriedObject::draw();

    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void CO_PhantoKey::placeKey()
{
    relocatetimer = 0;

    short x = 0, y = 0;
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) && iAttempts-- > 0);

    setXi(x);
    setYi(y);

    vely = GRAVITATION;
    velx = 0.0f;

    Drop();
}
