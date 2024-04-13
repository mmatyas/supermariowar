#include "WO_Area.h"

#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"

extern CObjectContainer objectcontainer[3];
extern CPlayer* list_players[4];
extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class area (for Domination mode)
//------------------------------------------------------------------------------
OMO_Area::OMO_Area(gfxSprite* nspr, short iNumAreas)
    : IO_OverMapObject(nspr, 1280, 960, 5, 0)
    , numareas(iNumAreas)
{
    iw = (short)spr->getWidth() / 5;
    collisionWidth = iw;

    objectType = object_area;

    placeArea();
}

bool OMO_Area::collide(CPlayer* player)
{
    if (player->tanookisuit.notStatue() && !player->isdead()) {
        totalTouchingPlayers++;

        if (totalTouchingPlayers == 1) {
            touchingPlayer = player;
        } else {
            touchingPlayer = NULL;
            reset();
        }
    }

    return false;
}

void OMO_Area::draw()
{
    spr->draw(ix, iy, frame, 0, iw, ih);
}

void OMO_Area::update()
{
    if (touchingPlayer)
        setOwner(touchingPlayer);

    if (iPlayerID != -1 && !game_values.gamemode->gameover) {
        if (++scoretimer >= (game_values.pointspeed << 1)) {
            scoretimer = 0;
            list_players[iPlayerID]->Score().AdjustScore(1);
            game_values.gamemode->CheckWinner(list_players[iPlayerID]);
        }
    }

    if (game_values.gamemodesettings.domination.relocationfrequency > 0) {
        if (++relocatetimer >= game_values.gamemodesettings.domination.relocationfrequency) {
            relocatetimer = 0;
            placeArea();
        }
    }

    totalTouchingPlayers = 0;
    touchingPlayer = NULL;
}

void OMO_Area::placeArea()
{
    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) || objectcontainer[0].getClosestObject(x, y, object_area) <= (200.0f - ((numareas - 3) * 25.0f)))
        && iAttempts-- > 0)
        ;

    setXi(x);
    setYi(y);
}

void OMO_Area::reset()
{
    iPlayerID = -1;
    iTeamID = -1;

    colorID = -1;
    scoretimer = 0;
    frame = 0;
}

void OMO_Area::setOwner(CPlayer* player)
{
    if (colorID != player->colorID) {
        iPlayerID = player->localID;
        iTeamID = player->teamID;
        colorID = player->colorID;

        frame = (colorID + 1) * iw;
        ifSoundOnPlay(rm->sfx_areatag);
    }
}
