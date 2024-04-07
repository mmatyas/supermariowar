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
// class yoshi (for egg mode)
//------------------------------------------------------------------------------
MO_Yoshi::MO_Yoshi(gfxSprite *nspr, short iColor) :
    IO_MovingObject(nspr, 0, 0, 2, 8, 52, 56, 0, 0, 0, iColor * 56, 56, 52)
{
    objectType = object_moving;
    movingObjectType = movingobject_yoshi;
    state = 1;

    color = iColor;

    placeYoshi();

    fObjectCollidesWithMap = false;
}

bool MO_Yoshi::collide(CPlayer * player)
{
    if (player->carriedItem && player->carriedItem->getMovingObjectType() == movingobject_egg) {
        CO_Egg * egg = (CO_Egg*)player->carriedItem;

        if (egg->color == color) {
            if (!game_values.gamemode->gameover) {
                player->Score().AdjustScore(1);
                game_values.gamemode->CheckWinner(player);
            }

            placeYoshi();

            egg->placeEgg();

            ifSoundOnPlay(rm->sfx_yoshi);
        }
    }

    return false;
}

void MO_Yoshi::update()
{
    animate();

    if (++timer > 2000)
        placeYoshi();
}

void MO_Yoshi::placeYoshi()
{
    timer = 0;

    for (short tries = 0; tries < 64; tries++) {
		ix = (short)RANDOM_INT(App::screenWidth - iw);
        iy = (short)RANDOM_INT(App::screenHeight - ih - TILESIZE);	//don't spawn too low

        short ixl = ix / TILESIZE;
        short ixr = (ix + iw) / TILESIZE;
        short iyt = iy / TILESIZE;
        short iyb = (iy + ih) / TILESIZE;

        int upperLeft = g_map->map(ixl, iyt);
        int upperRight = g_map->map(ixr, iyt);
        int lowerLeft = g_map->map(ixl, iyb);
        int lowerRight = g_map->map(ixr, iyb);

        if ((upperLeft & tile_flag_solid) == 0 && (upperRight & tile_flag_solid) == 0 &&
                (lowerLeft & tile_flag_solid) == 0 && (lowerRight & tile_flag_solid) == 0 &&
                !g_map->block(ixl, iyt) && !g_map->block(ixr, iyt) && !g_map->block(ixl, iyb) && !g_map->block(ixr, iyb)) {
            //spawn on ground, but not on spikes
            short iDeathY = (iy+ih)/TILESIZE;
            short iDeathX1 = ix/TILESIZE;
            short iDeathX2 = (ix+iw)/TILESIZE;

            while (iDeathY < MAPHEIGHT) {
                int ttLeftTile = g_map->map(iDeathX1, iDeathY);
                int ttRightTile = g_map->map(iDeathX2, iDeathY);

                if (((ttLeftTile & tile_flag_solid || ttLeftTile & tile_flag_solid_on_top) && (ttLeftTile & tile_flag_death_on_top) == 0) ||
                        ((ttRightTile & tile_flag_solid || ttRightTile & tile_flag_solid_on_top) && (ttRightTile & tile_flag_death_on_top) == 0) ||
                        g_map->block(iDeathX1, iDeathY) || g_map->block(iDeathX2, iDeathY)) {
                    short top = ((iDeathY << 5) - ih) / TILESIZE;

                    if (g_map->spawn(1, iDeathX1, top) && g_map->spawn(1, iDeathX2, top) &&
                            g_map->spawn(1, iDeathX1, iDeathY - 1) && g_map->spawn(1, iDeathX2, iDeathY - 1)) {
                        setXi(ix);
                        setYi((iDeathY << 5) - ih);
                        return;
                    }

                    break;
                } else if (ttLeftTile & tile_flag_death_on_top || ttRightTile & tile_flag_death_on_top) {
                    break;
                }

                iDeathY++;
            }
        }
    }

    ix = 320;
    iy = 240;
}

void MO_Yoshi::collide(IO_MovingObject * object)
{
    if (object->getMovingObjectType() == movingobject_egg) {
        CO_Egg * egg = (CO_Egg*)object;

        if (egg->color == color && egg->owner_throw) {
            CPlayer * player = egg->owner_throw;

            if (!game_values.gamemode->gameover) {
                player->Score().AdjustScore(1);
                game_values.gamemode->CheckWinner(player);
            }

            placeYoshi();
            egg->placeEgg();

            ifSoundOnPlay(rm->sfx_yoshi);
        }
    }
}

