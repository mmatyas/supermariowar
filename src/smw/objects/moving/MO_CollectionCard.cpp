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
// class collection card (for card collection mode)
//------------------------------------------------------------------------------
MO_CollectionCard::MO_CollectionCard(gfxSprite *nspr, short iType, short iValue, short iUncollectableTime, float dvelx, float dvely, short iX, short iY) :
    IO_MovingObject(nspr, iX, iY, 6, 8, -1, -1, -1, -1, 0, 0, 32, 32)
{
    state = 1;
    objectType = object_moving;
    movingObjectType = movingobject_collectioncard;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    type = iType;
    value = iValue;

    uncollectabletime = iUncollectableTime;
    velx = dvelx;
    vely = dvely;

    if (iType == 0) {
        placeCard();
        fObjectCollidesWithMap = false;
    } else {
        collision_detection_checksides();
        animationOffsetY = (value + 1) << 5;
    }
}

bool MO_CollectionCard::collide(CPlayer * player)
{
    //If it is not collectable, return
    if ((type == 1 && uncollectabletime > 0) || state != 1)
        return false;

    ifSoundOnPlay(rm->sfx_areatag);

    //Add this card to the team's score
    if (player->Score().subscore[0] < 3) {
        player->Score().subscore[1] |= value << (player->Score().subscore[0] << 1);
        player->Score().subscore[0]++;
    } else {
        player->Score().subscore[1] &= ~48; //Clear previous card in 3rd slot
        player->Score().subscore[1] |= value << 4; //Set card to newly collected one in 3rd slot
    }

    player->Score().subscore[2] = 0;

    if (type == 1) {
        dead = true;
    } else {
        state = 2;
        animationspeed = 4;
        animationtimer = 0;
        animationOffsetY = animationOffsetY = (value + 1) << 5; // FIXME
        drawframe = 96;
    }

    timer = 0;

    return false;
}

void MO_CollectionCard::update()
{
    if (type == 1 || state < 3)
        animate();

    //Handle flipping over a card to reveal it's value
    if (state == 2 && drawframe == 0) {
        state = 3;
        timer = 0;
    } else if (state == 3) {
        if (++timer > 200) {
            dead = true;
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
        }
    }


    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }

    if (type == 0) {
        if (++timer > 1500)
            placeCard();
    } else {
        applyfriction();
        IO_MovingObject::update();

        if (--uncollectabletime < -game_values.gamemodesettings.collection.cardlife) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
            dead = true;
        }
    }
}

void MO_CollectionCard::draw()
{
    IO_MovingObject::draw();

    //Draw sparkles
    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_CollectionCard::placeCard()
{
    timer = 0;

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[1].getClosestMovingObject(x, y, movingobject_collectioncard) <= 150.0f)
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);
}


