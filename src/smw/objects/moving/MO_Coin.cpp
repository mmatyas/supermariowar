#include "MO_Coin.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"

extern CObjectContainer objectcontainer[3];
extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class coin (for coin mode)
//------------------------------------------------------------------------------
MO_Coin::MO_Coin(gfxSprite* nspr, Vec2f vel, Vec2s pos, short color, short team, short type, short uncollectabletime, bool placecoin)
    : IO_MovingObject(nspr, pos, 4, 8, 30, 30, 1, 1, 0, color << 5, 32, 32)
{
    state = 1;
    objectType = object_moving;
    movingObjectType = movingobject_coin;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    iType = type;
    iTeam = team;

    iUncollectableTime = uncollectabletime;
    velx = vel.x;
    vely = vel.y;

    timer = 0;
    if (placecoin) {
        placeCoin();
    }

    if (iType == 0) {
        fObjectCollidesWithMap = false;
    } else {
        collision_detection_checksides();
    }
}

bool MO_Coin::collide(CPlayer* player)
{
    if (iUncollectableTime > 0 || (iType == 1 && (!game_values.gamemodesettings.greed.owncoins && iTeam == player->getTeamID())))
        return false;

    if (!game_values.gamemode->gameover) {
        if (iType == 2)
            player->Score().subscore[0]++;
        else
            player->Score().AdjustScore(1);

        game_values.gamemode->CheckWinner(player);
    }

    eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_coinsparkle, ix, iy, 7, 4);

    ifSoundOnPlay(rm->sfx_coin);

    if (iType == 0)
        placeCoin();
    else
        dead = true;

    return false;
}

void MO_Coin::update()
{
    if (iType != 1) {
        animate();

        if (++sparkleanimationtimer >= 4) {
            sparkleanimationtimer = 0;
            sparkledrawframe += 32;
            if (sparkledrawframe >= App::screenHeight)
                sparkledrawframe = 0;
        }

        if (++timer > 1000)
            placeCoin();
    }

    if (iType != 0) {
        applyfriction();
        IO_MovingObject::update();

        iUncollectableTime--;

        if (iType == 1 && iUncollectableTime < -game_values.gamemodesettings.greed.coinlife) {
            eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_fireballexplosion, ix, iy, 3, 8);
            dead = true;
        }
    }
}

void MO_Coin::draw()
{
    IO_MovingObject::draw();

    // Draw sparkles
    if (iType != 1)
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_Coin::placeCoin()
{
    timer = 0;

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) || objectcontainer[1].getClosestMovingObject(x, y, movingobject_coin) < 150.0f)
        && iAttempts-- > 0)
        ;

    setXi(x);
    setYi(y);
}
