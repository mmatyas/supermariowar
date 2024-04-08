#include "WO_KingOfTheHillZone.h"

#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

extern short iKingOfTheHillZoneLimits[4][4];

extern CPlayer* list_players[4];
extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class KingOfTheHillArea (for King of the Hill mode)
//------------------------------------------------------------------------------
OMO_KingOfTheHillZone::OMO_KingOfTheHillZone(gfxSprite* nspr)
    : IO_OverMapObject(nspr, 0, 0, 5, 0)
{
    size = game_values.gamemodesettings.kingofthehill.areasize;

    if (size < 2)
        game_values.gamemodesettings.kingofthehill.areasize = size = 2;

    iw = TILESIZE * size;
    collisionWidth = iw;
    ih = TILESIZE * size;
    collisionHeight = ih;

    objectType = object_kingofthehill_area;
    state = 1;

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        playersTouching[iPlayer] = NULL;
        playersTouchingCount[iPlayer] = 0;
    }

    placeArea();
}

bool OMO_KingOfTheHillZone::collide(CPlayer* player)
{
    if (!player->IsTanookiStatue()) {
        playersTouching[player->getTeamID()] = player;
        playersTouchingCount[player->getTeamID()]++;
        totalTouchingPlayers++;
    }
    return false;
}

void OMO_KingOfTheHillZone::draw()
{
    for (short iRow = 0; iRow < size; iRow++) {
        short iYPiece = TILESIZE;
        if (iRow == 0)
            iYPiece = 0;
        if (iRow == size - 1)
            iYPiece = TILESIZE * 2;

        for (short iCol = 0; iCol < size; iCol++) {
            short iXPiece = TILESIZE;
            if (iCol == 0)
                iXPiece = 0;
            if (iCol == size - 1)
                iXPiece = TILESIZE * 2;

            short iColX = ix + (iCol << 5);
            short iRowX = iy + (iRow << 5);

            if (multiplier > 1)
                rm->spr_awardkillsinrow.draw(iColX + 8, iRowX + 8, (multiplier - 1) << 4, colorID << 4, 16, 16);

            spr->draw(iColX, iRowX, iXPiece + frame, iYPiece, TILESIZE, TILESIZE);
        }
    }
}

void OMO_KingOfTheHillZone::update()
{
    short iMax = 0;
    short iMaxTeam = -1;

    for (short iTeam = 0; iTeam < 4; iTeam++) {
        if (playersTouchingCount[iTeam] > iMax) {
            iMax = playersTouchingCount[iTeam];
            iMaxTeam = iTeam;
        }
    }

    if ((iMax << 1) > totalTouchingPlayers) {  // If the max touching player team is greater than the rest of the touching players
        colorID = playersTouching[iMaxTeam]->getColorID();
        iPlayerID = playersTouching[iMaxTeam]->localID;
        frame = ((colorID + 1) << 5) * 3;
    } else {
        colorID = -1;
        iPlayerID = -1;
        frame = 0;
    }

    if (iPlayerID != -1 && !game_values.gamemode->gameover) {
        scoretimer += (iMax << 1) - totalTouchingPlayers;  // Speed of point accumulation is proportional to how many players are in zone

        if (scoretimer >= game_values.pointspeed) {
            scoretimer = 0;
            list_players[iPlayerID]->Score().AdjustScore(multiplier);
            game_values.gamemode->CheckWinner(list_players[iPlayerID]);

            if (game_values.gamemodesettings.kingofthehill.maxmultiplier > 1 && ++multipliertimer >= 5) {
                multipliertimer = 0;

                if (multiplier < game_values.gamemodesettings.kingofthehill.maxmultiplier)
                    multiplier++;
            }
        }
    } else {
        multiplier = 1;
        multipliertimer = 0;
    }

    if (game_values.gamemodesettings.kingofthehill.relocationfrequency > 0) {
        if (++relocatetimer >= game_values.gamemodesettings.kingofthehill.relocationfrequency) {
            relocatetimer = 0;
            placeArea();
        }
    }

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        playersTouching[iPlayer] = NULL;
        playersTouchingCount[iPlayer] = 0;
    }

    totalTouchingPlayers = 0;
}

void OMO_KingOfTheHillZone::placeArea()
{
    relocatetimer = 0;
    colorID = -1;
    iPlayerID = -1;
    frame = 0;

    multiplier = 1;
    multipliertimer = 0;

    short x;
    short y;

    for (short iLoop = 0; iLoop < 64; iLoop++) {
        x = (short)RANDOM_INT(MAPWIDTH - size + 1);
        y = (short)RANDOM_INT(MAPHEIGHT - size);

        // First move the zone down so it is sitting on atleast 1 solid tile
        short iFindY = y + size;
        short iOldFindY = iFindY;
        bool fTryAgain = false;
        bool fDone = false;

        while (!fDone) {
            for (short iCol = 0; iCol < size; iCol++) {
                int type = g_map->map(x + iCol, iFindY);
                if (((type & tile_flag_solid_on_top || type & tile_flag_solid) && (type & tile_flag_death_on_top) == 0) || g_map->block(x + iCol, iFindY)) {
                    fDone = true;
                    break;
                }
            }

            if (fDone)
                break;

            if (++iFindY >= MAPHEIGHT)
                iFindY = size;

            if (iFindY == iOldFindY) {  // If we didn't find solid ground in that loop, look for a new place for the zone
                fTryAgain = true;
                break;
            }
        }

        if (fTryAgain)
            continue;

        y = iFindY - size;

        // Now verify that the area is not completely covered with solid tiles
        short iCountSolidTiles = 0;
        for (short iRow = 0; iRow < size; iRow++) {
            for (short iCol = 0; iCol < size; iCol++) {
                // If there is a solid tile inside the zone
                if ((g_map->map(x + iCol, y + iRow) & tile_flag_solid) || !g_map->spawn(1, x + iCol, y + iRow) || g_map->block(x + iCol, y + iRow)) {
                    iCountSolidTiles++;

                    // Be more picky in the first few loops, but allow solid tiles to be in
                    if ((iLoop < 16 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][0]) || (iLoop < 32 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][1]) || (iLoop < 48 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][2]) || (iLoop < 63 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][3])) {
                        fTryAgain = true;
                        break;
                    }
                }
            }

            if (fTryAgain)
                break;
        }

        if (fTryAgain)
            continue;

        // Verify zone is not in a platform
        if (g_map->IsInPlatformNoSpawnZone(x << 5, y << 5, size << 5, size << 5))
            continue;

        break;
    }

    ix = x << 5;
    iy = y << 5;
}

void OMO_KingOfTheHillZone::reset()
{
    iPlayerID = -1;
    colorID = -1;
    scoretimer = 0;
    frame = 0;
}
