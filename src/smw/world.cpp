#include "world.h"

#include "GameMode.h"
#include "GameValues.h"
#include "gfx.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "FileList.h"
#include "MapList.h"

#include <cstdio>
#include <cstdlib> // atoi()
#include <cstring>

#include <map>
#include <queue>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif

void ResetTourStops();
TourStop * ParseTourStopLine(char * buffer, int32_t iVersion[4], bool fIsWorld);
void WriteTourStopLine(TourStop * ts, char * buffer, bool fIsWorld);

WorldMap g_worldmap;

extern std::string stripPathAndExtension(const std::string &path);

extern int32_t g_iVersion[];
extern bool VersionIsEqual(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);
extern bool VersionIsEqualOrBefore(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);
extern bool VersionIsEqualOrAfter(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

extern CGameMode * gamemodes[GAMEMODE_LAST];

extern SDL_Surface* blitdest;

extern CGameValues game_values;
extern CResourceManager* rm;

extern MapList *maplist;
extern SkinList *skinlist;
extern WorldList *worldlist;

using std::queue;
using std::map;


/**********************************
* WorldMovingObject
**********************************/

WorldMovingObject::WorldMovingObject()
    : iDrawSprite(0)
    , iDrawDirection(0)
    , iTileSize(0)
{
    SetPosition(0, 0);
}

WorldMovingObject::~WorldMovingObject()
{}

void WorldMovingObject::Init(short iCol, short iRow, short iSprite, short iInitialDirection, short tilesize)
{
    iTileSize = tilesize;
    iTileSheet = tilesize == TILESIZE ? 0 : 1;
    SetPosition(iCol, iRow);

    iDrawSprite = iSprite;
    iDrawDirection = iInitialDirection;
}

void WorldMovingObject::Move(short iDirection)
{
    if (iDirection == 0) {
        iDestTileY--;
        iState = 1;
    } else if (iDirection == 1) {
        iDestTileY++;
        iState = 2;
    } else if (iDirection == 2) {
        iDestTileX--;
        iState = 3;
        iDrawDirection = 1;
    } else if (iDirection == 3) {
        iDestTileX++;
        iState = 4;
        iDrawDirection = 0;
    }
}

bool WorldMovingObject::Update()
{
    if (++iAnimationTimer > 15) {
        iAnimationTimer = 0;
        iAnimationFrame += 2;
        if (iAnimationFrame > 2)
            iAnimationFrame = 0;
    }

    if (iState == 1) {
        iy -= 2;
        if (iy < iDestTileY * iTileSize) {
            iy = iDestTileY * iTileSize;
            iState = 0;
            iCurrentTileY = iDestTileY;

            return true;
        }
    } else if (iState == 2) { //down
        iy += 2;
        if (iy > iDestTileY * iTileSize) {
            iy = iDestTileY * iTileSize;
            iState = 0;
            iCurrentTileY = iDestTileY;

            return true;
        }
    } else if (iState == 3) { //left
        ix -= 2;
        if (ix < iDestTileX * iTileSize) {
            ix = iDestTileX * iTileSize;
            iState = 0;
            iCurrentTileX = iDestTileX;

            return true;
        }
    } else if (iState == 4) { //right
        ix += 2;
        if (ix > iDestTileX * iTileSize) {
            ix = iDestTileX * iTileSize;
            iState = 0;
            iCurrentTileX = iDestTileX;

            return true;
        }
    }

    return false;
}

void WorldMovingObject::FaceDirection(short iDirection)
{
    iDrawDirection = iDirection;
}

void WorldMovingObject::SetPosition(short iCol, short iRow)
{
    ix = iCol * iTileSize;
    iy = iRow * iTileSize;
    iCurrentTileX = iCol;
    iCurrentTileY = iRow;
    iDestTileX = iCol;
    iDestTileY = iRow;

    iState = 0;
    iAnimationFrame = 0;
    iAnimationTimer = 0;
}

/**********************************
* WorldPlayer
**********************************/

WorldPlayer::WorldPlayer() :
    WorldMovingObject()
{}

WorldPlayer::~WorldPlayer()
{}

void WorldPlayer::Init(short iCol, short iRow)
{
    WorldMovingObject::Init(iCol, iRow, 0, 0, 32);
}

void WorldPlayer::Draw(short iMapOffsetX, short iMapOffsetY)
{
    rm->spr_player[iDrawSprite][iAnimationFrame + iDrawDirection]->draw(ix + iMapOffsetX, iy + iMapOffsetY, 0, 0, 32, 32);
}

void WorldPlayer::SetSprite(short iPlayer)
{
    while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], true)) {
        if (++game_values.skinids[iPlayer] >= skinlist->GetCount())
            game_values.skinids[iPlayer] = 0;
    }

    iDrawSprite = iPlayer;
}


/**********************************
* WorldVehicle
**********************************/

WorldVehicle::WorldVehicle() :
    WorldMovingObject()
{}

WorldVehicle::~WorldVehicle()
{}

void WorldVehicle::Init(short iCol, short iRow, short iAction, short iSprite, short minMoves, short maxMoves, bool spritePaces, short iInitialDirection, short boundary, short tilesize)
{
    WorldMovingObject::Init(iCol, iRow, iSprite, iInitialDirection, tilesize);

    fEnabled = true;

    short iRectOffsetX = 0;
    short iRectOffsetY = 0;

    if (iDrawSprite >= 0 && iDrawSprite <= 8) {
        iRectOffsetX = 0;
        iRectOffsetY = iDrawSprite * tilesize;
    }

    for (short iRect = 0; iRect < 5; iRect++)
        gfx_setrect(&srcRects[iRect], iRect * tilesize + iRectOffsetX, iRectOffsetY, tilesize, tilesize);

    iNumMoves = 0;
    iActionId = iAction;

    iMinMoves = minMoves;
    iMaxMoves = maxMoves;

    fSpritePaces = spritePaces;
    iPaceOffset = 0;
    iPaceTimer = 0;

    iBoundary = boundary;
}

void WorldVehicle::Move()
{
    iNumMoves = RANDOM_INT(iMaxMoves - iMinMoves + 1) + iMinMoves;

    if (iNumMoves > 0) {
        iPaceOffset = 0;
        iPaceTimer = 0;
    }

    SetNextDest();
}

void WorldVehicle::SetNextDest()
{
    if (iState != 0 || iMaxMoves == 0)
        return;

    WorldMapTile * tile = &g_worldmap.tiles[iCurrentTileX][iCurrentTileY];

    short iPlayerCurrentTileX, iPlayerCurrentTileY;
    g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

    if (iNumMoves-- <= 0) {
        if (tile->iType == 0 && (iPlayerCurrentTileX != iCurrentTileX || iPlayerCurrentTileY != iCurrentTileY) &&
                g_worldmap.NumVehiclesInTile(iCurrentTileX, iCurrentTileY) <= 1)
            return;
    }

    //Don't allow vehicle to move forever, cap it at 10 moves over the number attempted
    if (iNumMoves <= -10)
        return;

    //Check to see what directions are available to move
    //Can't move through doors or vehicle boundaries
    short iConnections[4];
    short iNumConnections = 0;
    for (short iDirection = 0; iDirection < 4; iDirection++) {
        bool fIsDoor = false;
        if (iDirection == 0)
            fIsDoor = g_worldmap.IsDoor(iCurrentTileX, iCurrentTileY - 1) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(iCurrentTileX, iCurrentTileY - 1) == iBoundary);
        else if (iDirection == 1)
            fIsDoor = g_worldmap.IsDoor(iCurrentTileX, iCurrentTileY + 1) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(iCurrentTileX, iCurrentTileY + 1) == iBoundary);
        else if (iDirection == 2)
            fIsDoor = g_worldmap.IsDoor(iCurrentTileX - 1, iCurrentTileY) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(iCurrentTileX - 1, iCurrentTileY) == iBoundary);
        else if (iDirection == 3)
            fIsDoor = g_worldmap.IsDoor(iCurrentTileX + 1, iCurrentTileY) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(iCurrentTileX + 1, iCurrentTileY) == iBoundary);

        if (tile->fConnection[iDirection] && !fIsDoor)
            iConnections[iNumConnections++] = iDirection;
    }

    if (iNumConnections > 0) {
        short iConnection = iConnections[RANDOM_INT(iNumConnections)];
        WorldMovingObject::Move(iConnection);
    }
}

bool WorldVehicle::Update()
{
    bool fMoveDone = WorldMovingObject::Update();

    if (fMoveDone) {
        short iPlayerTileX, iPlayerTileY;
        g_worldmap.GetPlayerCurrentTile(&iPlayerTileX, &iPlayerTileY);

        if (iCurrentTileX == iPlayerTileX && iCurrentTileY == iPlayerTileY)
            return true;

        SetNextDest();
    }

    //If we're done moving, start pacing in place
    if (fSpritePaces && iState == 0 && ++iPaceTimer > 1) {
        iPaceTimer = 0;

        if (iDrawDirection) {
            if (--iPaceOffset <= -(iTileSize >> 1))
                iDrawDirection = 0;
        } else {
            if (++iPaceOffset >= (iTileSize >> 1))
                iDrawDirection = 1;
        }
    }

    return false;
}

void WorldVehicle::Draw(short iWorldOffsetX, short iWorldOffsetY, bool fVehiclesSleeping)
{
    if (fVehiclesSleeping) {
        SDL_Rect rDst = {ix + iWorldOffsetX, iy + iWorldOffsetY, iTileSize, iTileSize};
        SDL_BlitSurface(rm->spr_worldvehicle[iTileSheet].getSurface(), &srcRects[4], blitdest, &rDst);
    } else {
        SDL_Rect rDst = {ix + iWorldOffsetX + iPaceOffset, iy + iWorldOffsetY, iTileSize, iTileSize};
        SDL_BlitSurface(rm->spr_worldvehicle[iTileSheet].getSurface(), &srcRects[iDrawDirection + iAnimationFrame], blitdest, &rDst);
    }
}


/**********************************
* WorldWarp
**********************************/

WorldWarp::WorldWarp()
{
    iCol1 = 0;
    iRow1 = 0;
    iCol2 = 0;
    iRow2 = 0;
}

void WorldWarp::Init(short id, short col1, short row1, short col2, short row2)
{
    iID = id;
    iCol1 = col1;
    iRow1 = row1;
    iCol2 = col2;
    iRow2 = row2;
}

void WorldWarp::GetOtherSide(short iCol, short iRow, short * iOtherCol, short * iOtherRow)
{
    if (iCol1 == iCol && iRow1 == iRow) {
        *iOtherCol = iCol2;
        *iOtherRow = iRow2;
    } else if (iCol2 == iCol && iRow2 == iRow) {
        *iOtherCol = iCol1;
        *iOtherRow = iRow1;
    } else {
        *iOtherCol = iCol;
        *iOtherRow = iRow;
    }
}


/**********************************
* WorldMap
**********************************/

WorldMap::WorldMap()
{
    iWidth = 0;
    iHeight = 0;
    tiles = NULL;
    vehicles = NULL;
    warps = NULL;
    iNumVehicles = 0;
    iNumStages = 0;
    iNumWarps = 0;
}

WorldMap::~WorldMap()
{
    Cleanup();
}

bool WorldMap::Load(short tilesize)
{
    Cleanup();
    ResetDrawCycle();

    iTileSize = tilesize;

    if (iTileSize == TILESIZE) {
        iTileSizeShift = 5;
        iTileSheet = 0;
    } else if (iTileSize == PREVIEWTILESIZE) {
        iTileSizeShift = 4;
        iTileSheet = 1;
    } else if (iTileSize == THUMBTILESIZE) {
        iTileSizeShift = 3;
        iTileSheet = 2;
    }

    const char * szPath = worldlist->GetIndex(game_values.worldindex);
    FILE * file = fopen(szPath, "r");
    worldName = stripPathAndExtension(szPath);

    if (!file)
        return false;

    char buffer[1024];
    short iReadType = 0;
    int32_t iVersion[4] = {0, 0, 0, 0};
    short iMapTileReadRow = 0;
    short iCurrentStage = 0;
    short iCurrentWarp = 0;
    short iCurrentVehicle = 0;

    while (fgets(buffer, 1024, file)) {
        if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == ' ' || buffer[0] == '\t')
            continue;

        if (iReadType == 0) { //Read version number
            char * psz = strtok(buffer, ".\n");
            if (psz)
                iVersion[0] = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                iVersion[1] = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                iVersion[2] = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                iVersion[3] = atoi(psz);

            iReadType = 1;
        } else if (iReadType == 1) { //music category
            iMusicCategory = atoi(buffer);
            iReadType = 2;
        } else if (iReadType == 2) { //world width
            iWidth = atoi(buffer);
            iReadType = 3;
        } else if (iReadType == 3) { //world height
            iHeight = atoi(buffer);
            iReadType = 4;

            tiles = new WorldMapTile*[iWidth];

            for (short iCol = 0; iCol < iWidth; iCol++)
                tiles[iCol] = new WorldMapTile[iHeight];

            short iDrawSurfaceTiles = iWidth * iHeight;

            if (iDrawSurfaceTiles > 456)
                iDrawSurfaceTiles = 456; //19 * 24 = 456 max tiles in world surface

            iTilesPerCycle = iDrawSurfaceTiles / 8;
        } else if (iReadType == 4) { //background sprites
            char * psz = strtok(buffer, ",\n");

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                if (!psz)
                    goto RETURN;

                WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
                tile->iBackgroundWater = atoi(psz);

                psz = strtok(NULL, ",\n");
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 5;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 5) { //background sprites
            char * psz = strtok(buffer, ",\n");

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                if (!psz)
                    goto RETURN;

                WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
                tile->iBackgroundSprite = atoi(psz);
                tile->fAnimated = (tile->iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE) != 1;

                tile->iID = iMapTileReadRow * iWidth + iMapTileReadCol;
                tile->iCol = iMapTileReadCol;
                tile->iRow = iMapTileReadRow;

                psz = strtok(NULL, ",\n");
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 6;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 6) { //foreground sprites
            char * psz = strtok(buffer, ",\n");

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                if (!psz)
                    goto RETURN;

                WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
                tile->iForegroundSprite = atoi(psz);

                short iForegroundSprite = tile->iForegroundSprite;

                //Animated parts of paths
                if (!tile->fAnimated && iForegroundSprite >= 0 && iForegroundSprite <= 8 * WORLD_PATH_SPRITE_SET_SIZE) {
                    short iForeground = iForegroundSprite % WORLD_PATH_SPRITE_SET_SIZE;
                    tile->fAnimated = iForeground >= 3 && iForeground <= 10;
                }

                //Animated 1-100 stages
                if (!tile->fAnimated)
                    tile->fAnimated = iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399;

                //Animated foreground tiles
                if (!tile->fAnimated)
                    tile->fAnimated = iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29;

                psz = strtok(NULL, ",\n");
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 7;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 7) { //path connections
            char * psz = strtok(buffer, ",\n");

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                if (!psz)
                    goto RETURN;

                WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
                tile->iConnectionType = atoi(psz);

                psz = strtok(NULL, ",\n");
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 8;
                iMapTileReadRow = 0;

                //1 == |  2 == -  3 == -!  4 == L  5 == ,-  6 == -,
                //7 == -|  8 == -`-  9 == |-  10 == -,-  11 == +
                //12 == horizontal bridge starts open,  13 == horizontal bridge closed
                //14 == vertical bridge starts open,  15 == vertical bridge closed
                for (short iRow = 0; iRow < iHeight; iRow++) {
                    for (short iCol = 0; iCol < iWidth; iCol++) {
                        SetTileConnections(iCol, iRow);
                    }
                }
            }
        } else if (iReadType == 8) { //stages
            char * psz = strtok(buffer, ",\n");

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                if (!psz)
                    goto RETURN;

                WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
                tile->iType = atoi(psz);
                tile->iWarp = -1;

                if (tile->iType == 1) {
                    iStartX = iMapTileReadCol;
                    iStartY = iMapTileReadRow;
                }

                tile->iCompleted = tile->iType <= 5 ? -1 : -2;

                psz = strtok(NULL, ",\n");
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 9;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 9) { //vehicle boundaries
            char * psz = strtok(buffer, ",\n");

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                if (!psz)
                    goto RETURN;

                WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
                tile->iVehicleBoundary = atoi(psz);

                psz = strtok(NULL, ",\n");
            }

            if (++iMapTileReadRow == iHeight)
                iReadType = 10;
        } else if (iReadType == 10) { //number of stages
            iNumStages = atoi(buffer);

            iReadType = iNumStages == 0 ? 12 : 11;
        } else if (iReadType == 11) { //stage details
            TourStop * ts = ParseTourStopLine(buffer, iVersion, true);

            game_values.tourstops.push_back(ts);
            game_values.tourstoptotal++;

            if (++iCurrentStage >= iNumStages) {
                //Scan stage IDs and make sure we have a stage for each one
                short iMaxStage = game_values.tourstoptotal + 5;
                for (short iRow = 0; iRow < iHeight; iRow++) {
                    for (short iCol = 0; iCol < iWidth; iCol++) {
                        short iType = tiles[iCol][iRow].iType;
                        if (iType < 0 || iType > iMaxStage)
                            goto RETURN;
                    }
                }

                iReadType = 12;
            }
        } else if (iReadType == 12) { //number of warps
            iNumWarps = atoi(buffer);

            if (iNumWarps < 0)
                iNumWarps = 0;

            if (iNumWarps > 0)
                warps = new WorldWarp[iNumWarps];

            iReadType = iNumWarps == 0 ? 14 : 13;
        } else if (iReadType == 13) { //warp details
            char * psz = strtok(buffer, ",\n");

            if (!psz)
                goto RETURN;

            short iCol1 = atoi(psz);
            if (iCol1 < 0)
                iCol1 = 0;

            psz = strtok(NULL, ",\n");

            short iRow1 = atoi(psz);
            if (iRow1 < 0)
                iRow1 = 0;

            psz = strtok(NULL, ",\n");

            short iCol2 = atoi(psz);
            if (iCol2 < 0)
                iCol2 = 0;

            psz = strtok(NULL, ",\n");

            short iRow2 = atoi(psz);
            if (iRow2 < 0)
                iRow2 = 0;

            warps[iCurrentWarp].Init(iCurrentWarp, iCol1, iRow1, iCol2, iRow2);

            tiles[iCol1][iRow1].iWarp = iCurrentWarp;
            tiles[iCol2][iRow2].iWarp = iCurrentWarp;

            if (++iCurrentWarp >= iNumWarps)
                iReadType = 14;
        } else if (iReadType == 14) { //number of vehicles
            iNumVehicles = atoi(buffer);

            if (iNumVehicles < 0)
                iNumVehicles = 0;

            if (iNumVehicles > 0)
                vehicles = new WorldVehicle[iNumVehicles];

            iReadType = iNumVehicles == 0 ? 16 : 15;
        } else if (iReadType == 15) { //vehicles
            char * psz = strtok(buffer, ",\n");

            if (!psz)
                goto RETURN;

            short iSprite = atoi(psz);

            psz = strtok(NULL, ",\n");

            short iStage = atoi(psz);

            if (iStage > iNumStages)
                iStage = 0;

            psz = strtok(NULL, ",\n");
            short iCol = atoi(psz);

            psz = strtok(NULL, ",\n");
            short iRow = atoi(psz);

            psz = strtok(NULL, ",\n");
            short iMinMoves = atoi(psz);

            if (iMinMoves < 0)
                iMinMoves = 0;

            psz = strtok(NULL, ",\n");
            short iMaxMoves = atoi(psz);

            if (iMaxMoves < iMinMoves)
                iMaxMoves = iMinMoves;

            psz = strtok(NULL, ",\n");
            bool fSpritePaces = atoi(psz) == 1;

            psz = strtok(NULL, ",\n");
            short iInitialDirection = atoi(psz);

            if (iInitialDirection != 0)
                iInitialDirection = 1;

            psz = strtok(NULL, ",\n");
            short iBoundary = atoi(psz);

            vehicles[iCurrentVehicle].Init(iCol, iRow, iStage, iSprite, iMinMoves, iMaxMoves, fSpritePaces, iInitialDirection, iBoundary, iTileSize);

            if (++iCurrentVehicle >= iNumVehicles)
                iReadType = 16;
        } else if (iReadType == 16) { //initial bonus items
            char * psz = strtok(buffer, ",\n");

            iNumInitialBonuses = 0;

            while (psz != NULL) {
                //0 indicates no initial bonuses
                if (psz[0] == '0')
                    break;

                short iBonusOffset = 0;
                if (psz[0] == 'w' || psz[0] == 'W')
                    iBonusOffset += NUM_POWERUPS;

                psz++;

                short iBonus = atoi(psz) + iBonusOffset;

                if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS)
                    iBonus = 0;

                if (iNumInitialBonuses < 32)
                    iInitialBonuses[iNumInitialBonuses++] = iBonus;
                else
                    iInitialBonuses[31] = iBonus;

                psz = strtok(NULL, ",\n");
            }

            iReadType = 17;
        }
    }

RETURN:

    fclose(file);

    return iReadType == 17;
}

void WorldMap::SetTileConnections(short iCol, short iRow)
{
    if (iCol < 0 || iRow < 0 || iCol >= iWidth || iRow >= iHeight)
        return;

    WorldMapTile * tile = &tiles[iCol][iRow];

    for (short iDirection = 0; iDirection < 4; iDirection++)
        tile->fConnection[iDirection] = false;

    if (iRow > 0) {
        WorldMapTile * topTile = &tiles[iCol][iRow - 1];

        tile->fConnection[0] = (topTile->iConnectionType == 1 || topTile->iConnectionType == 5 || topTile->iConnectionType == 6 ||
                                topTile->iConnectionType == 7 || topTile->iConnectionType == 9 || topTile->iConnectionType == 10 ||
                                topTile->iConnectionType == 11 || topTile->iConnectionType == 15) && (tile->iConnectionType == 1 ||
                                        tile->iConnectionType == 3 || tile->iConnectionType == 4 || tile->iConnectionType == 7 ||
                                        tile->iConnectionType == 8 || tile->iConnectionType == 9 || tile->iConnectionType == 11 ||
                                        tile->iConnectionType == 15);
    }

    if (iRow < iHeight - 1) {
        WorldMapTile * bottomTile = &tiles[iCol][iRow + 1];

        tile->fConnection[1] = (bottomTile->iConnectionType == 1 || bottomTile->iConnectionType == 3 || bottomTile->iConnectionType == 4 ||
                                bottomTile->iConnectionType == 7 || bottomTile->iConnectionType == 8 || bottomTile->iConnectionType == 9 ||
                                bottomTile->iConnectionType == 11 || bottomTile->iConnectionType == 15) && (tile->iConnectionType == 1 ||
                                        tile->iConnectionType == 5 || tile->iConnectionType == 6 || tile->iConnectionType == 7 ||
                                        tile->iConnectionType == 9 || tile->iConnectionType == 10 || tile->iConnectionType == 11 ||
                                        tile->iConnectionType == 15);
    }

    if (iCol > 0) {
        WorldMapTile * leftTile = &tiles[iCol - 1][iRow];

        tile->fConnection[2] = (leftTile->iConnectionType == 2 || leftTile->iConnectionType == 4 || leftTile->iConnectionType == 5 ||
                                leftTile->iConnectionType == 8 || leftTile->iConnectionType == 9 || leftTile->iConnectionType == 10 ||
                                leftTile->iConnectionType == 11 || leftTile->iConnectionType == 13) && (tile->iConnectionType == 2 || tile->iConnectionType == 3 ||
                                        tile->iConnectionType == 6 || tile->iConnectionType == 7 || tile->iConnectionType == 8 ||
                                        tile->iConnectionType == 10 || tile->iConnectionType == 11 || tile->iConnectionType == 13);
    }

    if (iCol < iWidth - 1) {
        WorldMapTile * rightTile = &tiles[iCol + 1][iRow];

        tile->fConnection[3] = (rightTile->iConnectionType == 2 || rightTile->iConnectionType == 3 || rightTile->iConnectionType == 6 ||
                                rightTile->iConnectionType == 7 || rightTile->iConnectionType == 8 || rightTile->iConnectionType == 10 ||
                                rightTile->iConnectionType == 11 || rightTile->iConnectionType == 13) && (tile->iConnectionType == 2 || tile->iConnectionType == 4 ||
                                        tile->iConnectionType == 5 || tile->iConnectionType == 8 || tile->iConnectionType == 9 ||
                                        tile->iConnectionType == 10 || tile->iConnectionType == 11 || tile->iConnectionType == 13);
    }
}

//Saves world to file
bool WorldMap::Save()
{
    return Save(worldlist->GetIndex(game_values.worldindex));
}

bool WorldMap::Save(const char * szPath)
{
    FILE * file = fopen(szPath, "w");

    if (!file)
        return false;

    fprintf(file, "#Version\n");
    fprintf(file, "%d.%d.%d.%d\n\n", g_iVersion[0], g_iVersion[1], g_iVersion[2], g_iVersion[3]);

    fprintf(file, "#Music Category\n");
    fprintf(file, "%d\n\n", iMusicCategory);

    fprintf(file, "#Width\n");
    fprintf(file, "%d\n\n", iWidth);

    fprintf(file, "#Height\n");
    fprintf(file, "%d\n\n", iHeight);

    fprintf(file, "#Sprite Water Layer\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < iHeight; iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
            WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile->iBackgroundWater);

            if (iMapTileReadCol == iWidth - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Sprite Background Layer\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < iHeight; iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
            WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile->iBackgroundSprite);

            if (iMapTileReadCol == iWidth - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Sprite Foreground Layer\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < iHeight; iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
            WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile->iForegroundSprite);

            if (iMapTileReadCol == iWidth - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Connections\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < iHeight; iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
            WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile->iConnectionType);

            if (iMapTileReadCol == iWidth - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Tile Types (Stages, Doors, Start Tiles)\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < iHeight; iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
            WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile->iType);

            if (iMapTileReadCol == iWidth - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Vehicle Boundaries\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < iHeight; iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
            WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile->iVehicleBoundary);

            if (iMapTileReadCol == iWidth - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Stages\n");
    fprintf(file, "#Stage Type 0,Map,Mode,Goal,Points,Bonus List(Max 10),Name,End World, then mode settings (see sample tour file for details)\n");
    fprintf(file, "#Stage Type 1,Bonus House Name,Sequential/Random Order,Display Text,Powerup List(Max 5)\n");

    fprintf(file, "%d\n", game_values.tourstoptotal);

    for (short iStage = 0; iStage < game_values.tourstoptotal; iStage++) {
        char szLine[4096];
        WriteTourStopLine(game_values.tourstops[iStage], szLine, true);
        fprintf(file, "%s", szLine);
    }
    fprintf(file, "\n");

    fprintf(file, "#Warps\n");
    fprintf(file, "#location 1 x, y, location 2 x, y\n");

    fprintf(file, "%d\n", iNumWarps);

    for (short iWarp = 0; iWarp < iNumWarps; iWarp++) {
        fprintf(file, "%d,", warps[iWarp].iCol1);
        fprintf(file, "%d,", warps[iWarp].iRow1);
        fprintf(file, "%d,", warps[iWarp].iCol2);
        fprintf(file, "%d\n", warps[iWarp].iRow2);
    }
    fprintf(file, "\n");

    fprintf(file, "#Vehicles\n");
    fprintf(file, "#Sprite,Stage Type, Start Column, Start Row, Min Moves, Max Moves, Sprite Paces, Sprite Direction, Boundary\n");

    fprintf(file, "%d\n", iNumVehicles);

    for (short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++) {
        fprintf(file, "%d,", vehicles[iVehicle].iDrawSprite);
        fprintf(file, "%d,", vehicles[iVehicle].iActionId);
        fprintf(file, "%d,", vehicles[iVehicle].iCurrentTileX);
        fprintf(file, "%d,", vehicles[iVehicle].iCurrentTileY);
        fprintf(file, "%d,", vehicles[iVehicle].iMinMoves);
        fprintf(file, "%d,", vehicles[iVehicle].iMaxMoves);
        fprintf(file, "%d,", vehicles[iVehicle].fSpritePaces);
        fprintf(file, "%d,", vehicles[iVehicle].iDrawDirection);
        fprintf(file, "%d\n", vehicles[iVehicle].iBoundary);
    }
    fprintf(file, "\n");

    fprintf(file, "#Initial Items\n");

    for (short iItem = 0; iItem < iNumInitialBonuses; iItem++) {
        if (iItem != 0)
            fprintf(file, ",");

        short iBonus = iInitialBonuses[iItem];
        char cBonusType = 'p';
        if (iBonus >= NUM_POWERUPS) {
            iBonus -= NUM_POWERUPS;
            cBonusType = 'w';
        }

        fprintf(file, "%c%d", cBonusType, iBonus);
    }

    if (iNumInitialBonuses == 0)
        fprintf(file, "0");

    fprintf(file, "\n");

    fclose(file);

#if defined(__APPLE__)
    chmod(szPath, S_IRWXU | S_IRWXG | S_IROTH);
#endif

    return true;
}

void WorldMap::Clear()
{
    if (tiles) {
        for (short iCol = 0; iCol < iWidth; iCol++) {
            for (short iRow = 0; iRow < iHeight; iRow++) {
                tiles[iCol][iRow].iBackgroundSprite = 0;
                tiles[iCol][iRow].iBackgroundWater = 0;
                tiles[iCol][iRow].iForegroundSprite = 0;
                tiles[iCol][iRow].iConnectionType = 0;
                tiles[iCol][iRow].iType = 0;
                tiles[iCol][iRow].iID = iRow * iWidth + iCol;
                tiles[iCol][iRow].iVehicleBoundary = 0;
                tiles[iCol][iRow].iWarp = 0;
            }
        }
    }

    if (vehicles) {
        delete [] vehicles;
        vehicles = NULL;
    }

    iNumVehicles = 0;

    if (warps) {
        delete [] warps;
        warps = NULL;
    }

    iNumWarps = 0;
}

//Creates clears world and resizes (essentially creating a new world to work on for editor)
void WorldMap::New(short w, short h)
{
    Cleanup();

    iWidth = w;
    iHeight = h;

    tiles = new WorldMapTile*[iWidth];

    for (short iCol = 0; iCol < iWidth; iCol++)
        tiles[iCol] = new WorldMapTile[iHeight];

    Clear();
}

//Resizes world keeping intact current tiles (if possible)
void WorldMap::Resize(short w, short h)
{
    //Copy tiles from old map
    WorldMapTile ** tempTiles = NULL;
    short iOldWidth = iWidth;
    short iOldHeight = iHeight;

    if (tiles) {
        tempTiles = new WorldMapTile*[w];

        for (short iCol = 0; iCol < w && iCol < iOldWidth; iCol++) {
            tempTiles[iCol] = new WorldMapTile[h];

            for (short iRow = 0; iRow < h && iRow < iOldHeight; iRow++) {
                tempTiles[iCol][iRow].iBackgroundSprite = tiles[iCol][iRow].iBackgroundSprite;
                tempTiles[iCol][iRow].iBackgroundWater = tiles[iCol][iRow].iBackgroundWater;
                tempTiles[iCol][iRow].iForegroundSprite = tiles[iCol][iRow].iForegroundSprite;
                tempTiles[iCol][iRow].iConnectionType = tiles[iCol][iRow].iConnectionType;
                tempTiles[iCol][iRow].iType = tiles[iCol][iRow].iType;
            }
        }
    }

    //Create new map
    New(w, h);

    //Copy into new map
    if (tempTiles) {
        for (short iCol = 0; iCol < w && iCol < iOldWidth; iCol++) {
            for (short iRow = 0; iRow < h && iRow < iOldHeight; iRow++) {
                tiles[iCol][iRow].iBackgroundSprite = tempTiles[iCol][iRow].iBackgroundSprite;
                tiles[iCol][iRow].iBackgroundWater = tempTiles[iCol][iRow].iBackgroundWater;
                tiles[iCol][iRow].iForegroundSprite = tempTiles[iCol][iRow].iForegroundSprite;
                tiles[iCol][iRow].iConnectionType = tempTiles[iCol][iRow].iConnectionType;
                tiles[iCol][iRow].iType = tempTiles[iCol][iRow].iType;
            }
        }

        for (short iCol = 0; iCol < w; iCol++)
            delete [] tempTiles[iCol];

        delete [] tempTiles;
    }
}

void WorldMap::InitPlayer()
{
    player.Init(iStartX, iStartY);
}

bool WorldMap::Update(bool * fPlayerVehicleCollision)
{
    bool fPlayMovingVehicleSound = false;

    bool fPlayerDoneMove = player.Update();

    *fPlayerVehicleCollision = false;
    for (short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++) {
        if (!vehicles[iVehicle].fEnabled)
            continue;

        *fPlayerVehicleCollision |= vehicles[iVehicle].Update();

        if (vehicles[iVehicle].iState > 0)
            fPlayMovingVehicleSound = true;
    }

    if (fPlayMovingVehicleSound && !rm->sfx_boomerang.isPlaying())
        ifSoundOnPlay(rm->sfx_boomerang);

    return fPlayerDoneMove;
}

void WorldMap::Draw(short iMapOffsetX, short iMapOffsetY, bool fDrawPlayer, bool fVehiclesSleeping)
{
    for (short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++) {
        if (!vehicles[iVehicle].fEnabled)
            continue;

        vehicles[iVehicle].Draw(iMapOffsetX, iMapOffsetY, fVehiclesSleeping);
    }

    if (fDrawPlayer)
        player.Draw(iMapOffsetX, iMapOffsetY);
}

void WorldMap::UpdateTile(SDL_Surface * surface, short iCol, short iRow, short iMapDrawOffsetCol, short iMapDrawOffsetRow, short iAnimationFrame)
{
    DrawTileToSurface(surface, iCol, iRow, iMapDrawOffsetCol, iMapDrawOffsetRow, true, iAnimationFrame);
}

void WorldMap::DrawMapToSurface(SDL_Surface * surface)
{
    for (short iRow = 0; iRow < iHeight; iRow++) {
        for (short iCol = 0; iCol < iWidth; iCol++) {
            DrawTileToSurface(surface, iCol, iRow, 0, 0, true, 0, 0);
        }
    }
}

void WorldMap::ResetDrawCycle()
{
    iLastDrawRow = 0;
    iLastDrawCol = 0;
}

void WorldMap::DrawMapToSurface(short iCycleIndex, bool fFullRefresh, SDL_Surface * surface, short iMapDrawOffsetCol, short iMapDrawOffsetRow, short iAnimationFrame)
{
    short iRowEnd = 19 + iMapDrawOffsetRow < iHeight ? 19 : iHeight - iMapDrawOffsetRow;
    short iColEnd = 24 + iMapDrawOffsetCol < iWidth ? 24 : iWidth - iMapDrawOffsetCol;

    short iCounter = 0, iRow, iCol;
    short iLayer = iCycleIndex == -1 ? 0 : iCycleIndex < 8 ? 1 : 2;
    for (iRow = iLastDrawRow; iRow < iRowEnd; iRow++) {
        for (iCol = iLastDrawCol; iCol < iColEnd; iCol++) {
            DrawTileToSurface(surface, iCol, iRow, iMapDrawOffsetCol, iMapDrawOffsetRow, fFullRefresh, iAnimationFrame, iLayer);

            if (iCycleIndex >= 0 && iCycleIndex != 7 && iCycleIndex != 15 && ++iCounter > iTilesPerCycle)
                goto STOPDRAWING;
        }

        iLastDrawCol = 0;
    }

    iLastDrawRow = 0;

    return;

STOPDRAWING:
    iLastDrawRow = iRow;
    iLastDrawCol = iCol;
}

void WorldMap::DrawTileToSurface(SDL_Surface * surface, short iCol, short iRow, short iMapDrawOffsetCol, short iMapDrawOffsetRow, bool fFullRefresh, short iAnimationFrame, short iLayer)
{
    WorldMapTile * tile = &tiles[iCol + iMapDrawOffsetCol][iRow + iMapDrawOffsetRow];

    if (!tile->fAnimated && !fFullRefresh)
        return;

    SDL_Rect r = {iCol * iTileSize, iRow * iTileSize, iTileSize, iTileSize};

    short iBackgroundSprite = tile->iBackgroundSprite;
    short iBackgroundWater = tile->iBackgroundWater;
    short iForegroundSprite = tile->iForegroundSprite;

    short iBackgroundStyleOffset = iBackgroundSprite / WORLD_BACKGROUND_SPRITE_SET_SIZE * (4 << iTileSizeShift);

    iBackgroundSprite %= WORLD_BACKGROUND_SPRITE_SET_SIZE;

    //The solid background tile is not animated, but all the rest are
    if (iLayer != 2) {
        if (iBackgroundSprite == 1) {
            SDL_Rect rSrc = {iTileSize + iBackgroundStyleOffset, iTileSize, iTileSize, iTileSize};
            SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
        } else {
            SDL_Rect rSrc = {iAnimationFrame + (iBackgroundWater << (2 + iTileSizeShift)), 0, iTileSize, iTileSize};
            SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);

            if ((iBackgroundSprite >= 2 && iBackgroundSprite <= 48)) {
                if (iBackgroundSprite >= 45) {
                    SDL_Rect rSrc = {(3 << iTileSizeShift) + iBackgroundStyleOffset, (iBackgroundSprite - 44) << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else if (iBackgroundSprite >= 30) {
                    SDL_Rect rSrc = {(2 << iTileSizeShift) + iBackgroundStyleOffset, (iBackgroundSprite - 29) << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else if (iBackgroundSprite >= 16) {
                    SDL_Rect rSrc = {iTileSize + iBackgroundStyleOffset, (iBackgroundSprite - 14) << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else {
                    SDL_Rect rSrc = {iBackgroundStyleOffset, iBackgroundSprite << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                }
            }
        }
    }

    if (iLayer != 1) {
        if (tile->iCompleted >= 0) {
            SDL_Rect rSrc = {(tile->iCompleted + 10) << iTileSizeShift, 5 << iTileSizeShift, iTileSize, iTileSize};
            SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);
        } else {
            if (iForegroundSprite >= 0 && iForegroundSprite < WORLD_FOREGROUND_STAGE_OFFSET) {
                short iPathStyle = iForegroundSprite / WORLD_PATH_SPRITE_SET_SIZE;
                short iPathOffsetX = (iPathStyle % 4) * (5 << iTileSizeShift);
                short iPathOffsetY = (iPathStyle >> 2) * (10 << iTileSizeShift);
                iForegroundSprite %= WORLD_PATH_SPRITE_SET_SIZE;

                if (iForegroundSprite == 1 || iForegroundSprite == 2) { //Non-animated straight paths
                    SDL_Rect rSrc = {iPathOffsetX, ((iForegroundSprite - 1) << iTileSizeShift) + iPathOffsetY, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldpaths[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else if (iForegroundSprite >= 3 && iForegroundSprite <= 10) { //Animated paths with "coins" in them
                    SDL_Rect rSrc = {iPathOffsetX + iAnimationFrame, ((iForegroundSprite - 1) << iTileSizeShift) + iPathOffsetY, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldpaths[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else if (iForegroundSprite >= 11 && iForegroundSprite <= 18) { //Non-animated straight paths over water
                    short iSpriteX = (((iForegroundSprite - 11) / 2) + 1) << iTileSizeShift;
                    short iSpriteY = ((iForegroundSprite - 11) % 2) << iTileSizeShift;

                    SDL_Rect rSrc = {iPathOffsetX + iSpriteX, iSpriteY + iPathOffsetY, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldpaths[iTileSheet].getSurface(), &rSrc, surface, &r);
                }
            } else if (iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399) {
                short iTileColor = (iForegroundSprite - WORLD_FOREGROUND_STAGE_OFFSET) / 100;
                SDL_Rect rSrc = {(10 << iTileSizeShift) + iAnimationFrame, iTileColor << iTileSizeShift, iTileSize, iTileSize};
                SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);

                short iTileNumber = (iForegroundSprite - WORLD_FOREGROUND_STAGE_OFFSET) % 100;
                rSrc.x = (iTileNumber % 10) << iTileSizeShift;
                rSrc.y = (iTileNumber / 10) << iTileSizeShift;
                SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);
            } else if (iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) {
                SDL_Rect rSrc = {(iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 10) << iTileSizeShift, 7 << iTileSizeShift, iTileSize, iTileSize};
                SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);
            } else if (iForegroundSprite >= WORLD_START_SPRITE_OFFSET && iForegroundSprite <= WORLD_START_SPRITE_OFFSET + 1) {
                SDL_Rect rSrc = {(iForegroundSprite - WORLD_START_SPRITE_OFFSET + 10) << iTileSizeShift, 4 << iTileSizeShift, iTileSize, iTileSize};
                SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);
            } else if (iForegroundSprite >= WORLD_FOREGROUND_SPRITE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_OFFSET + 179) {
                short iSprite = iForegroundSprite - WORLD_FOREGROUND_SPRITE_OFFSET;
                SDL_Rect rSrc = {(iSprite % 12) << iTileSizeShift, (iSprite / 12) << iTileSizeShift, iTileSize, iTileSize};
                SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);
            } else if (iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29) {
                short iSprite = iForegroundSprite - WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET;
                SDL_Rect rSrc = {(iSprite >= 15 ? (16 << iTileSizeShift) : (12 << iTileSizeShift)) + iAnimationFrame, (iSprite % 15) << iTileSizeShift, iTileSize, iTileSize};
                SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);
            }
        }

        //Draw doors
        short iType = tile->iType;
        if (iType >= 2 && iType <= 5) {
            SDL_Rect rSrc = {(iType + 8) << iTileSizeShift, 6 << iTileSizeShift, iTileSize, iTileSize};
            SDL_BlitSurface(rm->spr_worldforeground[iTileSheet].getSurface(), &rSrc, surface, &r);
        }
    }
}

void WorldMap::Cleanup()
{
    ResetTourStops();
    iNumStages = 0;
    iNumInitialBonuses = 0;

    if (tiles) {
        for (short iCol = 0; iCol < iWidth; iCol++)
            delete [] tiles[iCol];

        delete [] tiles;

        tiles = NULL;
    }

    if (vehicles) {
        delete [] vehicles;
        vehicles = NULL;
    }

    iNumVehicles = 0;

    if (warps) {
        delete [] warps;
        warps = NULL;
    }

    iNumWarps = 0;
}

void WorldMap::SetPlayerSprite(short iPlayerSprite)
{
    player.SetSprite(iPlayerSprite);
}

bool WorldMap::IsVehicleMoving()
{
    for (short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++) {
        if (!vehicles[iVehicle].fEnabled)
            continue;

        if (vehicles[iVehicle].iState > 0)
            return true;
    }

    return false;
}

void WorldMap::GetPlayerPosition(short * iPlayerX, short * iPlayerY)
{
    *iPlayerX = player.ix;
    *iPlayerY = player.iy;
}

void WorldMap::SetPlayerPosition(short iPlayerCol, short iPlayerRow)
{
    player.SetPosition(iPlayerCol, iPlayerRow);
}

void WorldMap::GetPlayerCurrentTile(short * iPlayerCurrentTileX, short * iPlayerCurrentTileY)
{
    *iPlayerCurrentTileX = player.iCurrentTileX;
    *iPlayerCurrentTileY = player.iCurrentTileY;
}

void WorldMap::GetPlayerDestTile(short * iPlayerDestTileX, short * iPlayerDestTileY)
{
    *iPlayerDestTileX = player.iDestTileX;
    *iPlayerDestTileY = player.iDestTileY;
}

short WorldMap::GetPlayerState()
{
    return player.iState;
}

short WorldMap::GetVehicleInPlayerTile(short * vehicleIndex)
{
    for (short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++) {
        WorldVehicle * vehicle = &vehicles[iVehicle];

        if (!vehicle->fEnabled)
            continue;

        if (vehicle->iCurrentTileX == player.iCurrentTileX && vehicle->iCurrentTileY == player.iCurrentTileY) {
            *vehicleIndex = iVehicle;
            return vehicle->iActionId;
        }
    }

    *vehicleIndex = -1;
    return -1;
}

bool WorldMap::GetWarpInPlayerTile(short * iWarpCol, short * iWarpRow)
{
    short iWarp = tiles[player.iCurrentTileX][player.iCurrentTileY].iWarp;

    if (iWarp < 0)
        return false;

    warps[iWarp].GetOtherSide(player.iCurrentTileX, player.iCurrentTileY, iWarpCol, iWarpRow);
    return true;
}

void WorldMap::MovePlayer(short iDirection)
{
    player.Move(iDirection);
}

void WorldMap::FacePlayer(short iDirection)
{
    player.FaceDirection(iDirection);
}

void WorldMap::MoveVehicles()
{
    for (short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++) {
        if (!vehicles[iVehicle].fEnabled)
            continue;

        vehicles[iVehicle].Move();
    }
}

void WorldMap::RemoveVehicle(short iVehicleIndex)
{
    vehicles[iVehicleIndex].fEnabled = false;
}

short WorldMap::NumVehiclesInTile(short iTileX, short iTileY)
{
    short iVehicleCount = 0;
    for (short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++) {
        WorldVehicle * vehicle = &vehicles[iVehicle];

        if (!vehicle->fEnabled)
            continue;

        if (vehicle->iCurrentTileX == iTileX && vehicle->iCurrentTileY == iTileY)
            iVehicleCount++;
    }

    return iVehicleCount;
}

short WorldMap::GetVehicleStageScore(short iVehicleIndex)
{
    return game_values.tourstops[vehicles[iVehicleIndex].iActionId]->iPoints;
}

void WorldMap::MoveBridges()
{
    for (short iRow = 0; iRow < iHeight; iRow++) {
        for (short iCol = 0; iCol < iWidth; iCol++) {
            if (tiles[iCol][iRow].iConnectionType == 12) {
                tiles[iCol][iRow].iConnectionType = 13;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol - 1, iRow);
                SetTileConnections(iCol + 1, iRow);
            } else if (tiles[iCol][iRow].iConnectionType == 13) {
                tiles[iCol][iRow].iConnectionType = 12;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol - 1, iRow);
                SetTileConnections(iCol + 1, iRow);
            } else if (tiles[iCol][iRow].iConnectionType == 14) {
                tiles[iCol][iRow].iConnectionType = 15;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol, iRow - 1);
                SetTileConnections(iCol, iRow + 1);
            } else if (tiles[iCol][iRow].iConnectionType == 15) {
                tiles[iCol][iRow].iConnectionType = 14;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol, iRow - 1);
                SetTileConnections(iCol, iRow + 1);
            }

            if (tiles[iCol][iRow].iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET)
                tiles[iCol][iRow].iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET + 1;
            else if (tiles[iCol][iRow].iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET + 1)
                tiles[iCol][iRow].iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET;
            else if (tiles[iCol][iRow].iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET + 2)
                tiles[iCol][iRow].iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET + 3;
            else if (tiles[iCol][iRow].iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET + 3)
                tiles[iCol][iRow].iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET + 2;
        }
    }
}

void WorldMap::IsTouchingDoor(short iCol, short iRow, bool doors[4])
{
    WorldMapTile * tile = &tiles[iCol][iRow];

    if (iCol > 0) {
        if (tile->iCompleted >= -1) {
            short iType = tiles[iCol - 1][iRow].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iCol < iWidth - 1) {
        if (tile->iCompleted >= -1) {
            short iType = tiles[iCol + 1][iRow].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iRow > 0) {
        if (tile->iCompleted >= -1) {
            short iType = tiles[iCol][iRow - 1].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iCol < iHeight - 1) {
        if (tile->iCompleted >= -1) {
            short iType = tiles[iCol][iRow + 1].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }
}

bool WorldMap::IsDoor(short iCol, short iRow)
{
    if (iCol >= 0 && iRow >= 0 && iCol < iWidth && iRow < iHeight) {
        short iType = tiles[iCol][iRow].iType;
        if (iType >= 2 && iType <= 5)
            return true;
    }

    return false;
}

short WorldMap::UseKey(short iKeyType, short iCol, short iRow, bool fCloud)
{
    short iDoorsOpened = 0;

    WorldMapTile * tile = &tiles[iCol][iRow];

    if (iCol > 0) {
        if ((tile->iCompleted >= -1 || fCloud) && tiles[iCol - 1][iRow].iType - 2 == iKeyType) {
            tiles[iCol - 1][iRow].iType = 0;
            iDoorsOpened |= 1;
        }
    }

    if (iCol < iWidth - 1) {
        if ((tile->iCompleted >= -1 || fCloud) && tiles[iCol + 1][iRow].iType - 2 == iKeyType) {
            tiles[iCol + 1][iRow].iType = 0;
            iDoorsOpened |= 2;
        }
    }

    if (iRow > 0) {
        if ((tile->iCompleted >= -1 || fCloud) && tiles[iCol][iRow - 1].iType - 2 == iKeyType) {
            tiles[iCol][iRow - 1].iType = 0;
            iDoorsOpened |= 4;
        }
    }

    if (iRow < iHeight - 1) {
        if ((tile->iCompleted >= -1 || fCloud) && tiles[iCol][iRow + 1].iType - 2 == iKeyType) {
            tiles[iCol][iRow + 1].iType = 0;
            iDoorsOpened |= 8;
        }
    }

    return iDoorsOpened;
}

short WorldMap::GetVehicleBoundary(short iCol, short iRow)
{
    if (iCol >= 0 && iRow >= 0 && iCol < iWidth && iRow < iHeight) {
        return tiles[iCol][iRow].iVehicleBoundary;
    }

    return 0;
}

//Implements breadth first search to find a stage or vehicle of interest
short WorldMap::GetNextInterestingMove(short iCol, short iRow)
{
    WorldMapTile * currentTile = &tiles[iCol][iRow];

    //Look for stages or vehicles, but not bonus houses
    if ((currentTile->iType >= 6 && currentTile->iCompleted == -2) || NumVehiclesInTile(iCol, iRow) > 0)
        return 4; //Signal to press select on this tile

    short iCurrentId = currentTile->iID;

    std::queue<WorldMapTile*> next;
    std::map<short, short> visitedTiles;
    visitedTiles[currentTile->iID] = -1;
    next.push(currentTile);

    while (!next.empty()) {
        WorldMapTile * tile = next.front();

        if (tile == NULL)
            return -1;

        next.pop();

        //Look for stages or vehicles, but not bonus houses
        if ((tile->iType >= 6 && tile->iCompleted == -2) || NumVehiclesInTile(tile->iCol, tile->iRow) > 0) {
            short iBackTileDirection = visitedTiles[tile->iID];
            short iBackTileId = tile->iID;

            while (true) {
                if (iBackTileDirection == 0)
                    iBackTileId -= iWidth;
                else if (iBackTileDirection == 1)
                    iBackTileId += iWidth;
                else if (iBackTileDirection == 2)
                    iBackTileId -= 1;
                else if (iBackTileDirection == 3)
                    iBackTileId += 1;
                else if (iBackTileDirection == 4) {
                    short iWarpCol, iWarpRow;
                    short iCol = iBackTileId % iWidth;
                    short iRow = iBackTileId / iWidth;

                    warps[tiles[iCol][iRow].iWarp].GetOtherSide(iCol, iRow, &iWarpCol, &iWarpRow);
                    iBackTileId = tiles[iWarpCol][iWarpRow].iID;
                }

                if (iBackTileId == iCurrentId) {
                    if (iBackTileDirection == 0 || iBackTileDirection == 1)
                        return 1 - iBackTileDirection;
                    else if (iBackTileDirection == 2 || iBackTileDirection == 3)
                        return 5 - iBackTileDirection;
                    else
                        return iBackTileDirection;
                }

                iBackTileDirection = visitedTiles[iBackTileId];
            }
        }

        for (short iNeighbor = 0; iNeighbor < 4; iNeighbor++) {
            if (tile->fConnection[iNeighbor]) {
                if (iNeighbor == 0 && tile->iRow > 0) {
                    WorldMapTile * topTile = &tiles[tile->iCol][tile->iRow - 1];

                    //Stop at door tiles
                    if (topTile->iType >= 2 && topTile->iType <= 5)
                        continue;

                    if (visitedTiles.find(topTile->iID) == visitedTiles.end()) {
                        visitedTiles[topTile->iID] = 1;
                        next.push(topTile);
                    }
                } else if (iNeighbor == 1 && tile->iRow < iHeight - 1) {
                    WorldMapTile * bottomTile = &tiles[tile->iCol][tile->iRow + 1];

                    //Stop at door tiles
                    if (bottomTile->iType >= 2 && bottomTile->iType <= 5)
                        continue;

                    if (visitedTiles.find(bottomTile->iID) == visitedTiles.end()) {
                        visitedTiles[bottomTile->iID] = 0;
                        next.push(bottomTile);
                    }
                } else if (iNeighbor == 2 && tile->iCol > 0) {
                    WorldMapTile * leftTile = &tiles[tile->iCol - 1][tile->iRow];

                    //Stop at door tiles
                    if (leftTile->iType >= 2 && leftTile->iType <= 5)
                        continue;

                    if (visitedTiles.find(leftTile->iID) == visitedTiles.end()) {
                        visitedTiles[leftTile->iID] = 3;
                        next.push(leftTile);
                    }
                } else if (iNeighbor == 3 && tile->iCol < iWidth - 1) {
                    WorldMapTile * rightTile = &tiles[tile->iCol + 1][tile->iRow];

                    //Stop at door tiles
                    if (rightTile->iType >= 2 && rightTile->iType <= 5)
                        continue;

                    if (visitedTiles.find(rightTile->iID) == visitedTiles.end()) {
                        visitedTiles[rightTile->iID] = 2;
                        next.push(rightTile);
                    }
                }
            }

            if (tile->iWarp >= 0) {
                short iWarpCol, iWarpRow;
                warps[tile->iWarp].GetOtherSide(tile->iCol, tile->iRow, &iWarpCol, &iWarpRow);

                WorldMapTile * warpTile = &tiles[iWarpCol][iWarpRow];

                //Stop at door tiles
                if (warpTile->iType >= 2 && warpTile->iType <= 5)
                    continue;

                if (visitedTiles.find(warpTile->iID) == visitedTiles.end()) {
                    visitedTiles[warpTile->iID] = 4;
                    next.push(warpTile);
                }
            }
        }
    }

    return -1;
}

void WorldMap::SetInitialPowerups()
{
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        game_values.worldpowerupcount[iTeam] = iNumInitialBonuses;

        for (short iItem = 0; iItem < iNumInitialBonuses; iItem++)
            game_values.worldpowerups[iTeam][iItem] = iInitialBonuses[iItem];
    }
}


/**********************************
* Tour Stop functions
**********************************/

short ReadTourStopSetting(short * iSetting, bool * fSetting, short iDefault, bool fDefault)
{
    char * pszTemp = strtok(NULL, ",\n");
    if (pszTemp) {
        if (iSetting)
            *iSetting = atoi(pszTemp);

        if (fSetting)
            *fSetting = atoi(pszTemp) == 1;

        return 1;
    } else {
        if (iSetting)
            *iSetting = iDefault;

        if (fSetting)
            *fSetting = fDefault;
    }

    return 0;
}

TourStop * ParseTourStopLine(char * buffer, int32_t iVersion[4], bool fIsWorld)
{
    TourStop * ts = new TourStop();
    ts->fUseSettings = false;
    ts->iNumUsedSettings = 0;

    char * pszTemp = strtok(buffer, ",\n");

    short iStageType = 0;
    if (fIsWorld) {
        iStageType = atoi(pszTemp);
        if (iStageType < 0 || iStageType > 1)
            iStageType = 0;

        pszTemp = strtok(NULL, ",\n");
    }

    ts->iStageType = iStageType;

    ts->szBonusText[0][0] = 0;
    ts->szBonusText[1][0] = 0;
    ts->szBonusText[2][0] = 0;
    ts->szBonusText[3][0] = 0;
    ts->szBonusText[4][0] = 0;

    if (iStageType == 0) {
        char * szMap = new char[strlen(pszTemp) + 1];
        strcpy(szMap, pszTemp);

        pszTemp = strtok(NULL, ",\n");

        if (pszTemp)
            ts->iMode = atoi(pszTemp);
        else
            ts->iMode = -1;

        //If this is 1.8.0.2 or earlier and we are playing a minigame, use the default map
        if (VersionIsEqualOrBefore(iVersion, 1, 8, 0, 2) &&
                (ts->iMode == game_mode_pipe_minigame || ts->iMode == game_mode_boss_minigame || ts->iMode == game_mode_boxes_minigame)) {
            //Get a bogus map name so the mode will know to load the default map
            ts->pszMapFile = maplist->GetUnknownMapName();
        } else {
            //Using the maplist to cheat and find a map for us
            maplist->SaveCurrent();

            //If that map is not found
            bool fMapFound = maplist->findexact(szMap, true);

            if (!fMapFound) {
                if (ts->iMode == game_mode_pipe_minigame || ts->iMode == game_mode_boss_minigame || ts->iMode == game_mode_boxes_minigame) {
                    //Get a bogus map name so the mode will know to load the default map
                    ts->pszMapFile = maplist->GetUnknownMapName();
                } else {
                    maplist->random(false);
                    ts->pszMapFile = maplist->currentShortmapname();
                }
            } else {
                ts->pszMapFile = maplist->currentShortmapname();
            }

            maplist->ResumeCurrent();
        }

        delete [] szMap;

        //The pipe minigame was using the value 24 from version 1.8.0.0 to 1.8.0.2
        //It was later switched to 1000 to accomodate new modes easily
        if (VersionIsEqualOrBefore(iVersion, 1, 8, 0, 2)) {
            if (ts->iMode == 24)
                ts->iMode = game_mode_pipe_minigame;
        }

        //If a valid mode was not detected, then just choose a random mode
        if (ts->iMode < 0 || (ts->iMode >= GAMEMODE_LAST && ts->iMode != game_mode_pipe_minigame && ts->iMode != game_mode_boss_minigame && ts->iMode != game_mode_boxes_minigame))
            ts->iMode = RANDOM_INT(GAMEMODE_LAST);

        pszTemp = strtok(NULL, ",\n");

        //This gets the closest game mode to what the tour has
        ts->iGoal = -1;
        if (pszTemp) {
            //If it is commented out, this will allow things like 33 coins, 17 kill goals, etc.
            //ts->iGoal = gamemodes[ts->iMode]->GetClosestGoal(atoi(pszTemp));
            ts->iGoal = atoi(pszTemp);
        }

        //Default to a random goal if an invalid goal was used
        if (ts->iGoal <= 0) {
            if (ts->iMode < GAMEMODE_LAST)
                ts->iGoal = gamemodes[ts->iMode]->GetOptions()[RANDOM_INT(GAMEMODE_NUM_OPTIONS - 1)].iValue;
            else
                ts->iGoal = 50;
        }

        if (VersionIsEqualOrAfter(iVersion, 1, 7, 0, 2)) {
            pszTemp = strtok(NULL, ",\n");

            //Read in point value for tour stop
            if (pszTemp)
                ts->iPoints = atoi(pszTemp);
            else
                ts->iPoints = 1;

            pszTemp = strtok(NULL, ",\n");

            if (fIsWorld) {
                ts->iBonusType = 0;
                ts->iNumBonuses = 0;

                char * pszStart = pszTemp;

                while (pszStart != NULL) {
                    char * pszEnd = strstr(pszStart, "|");
                    if (pszEnd)
                        *pszEnd = 0;

                    //if it is "0", then no bonuses
                    short iWinnerPlace = pszStart[0] - 48;
                    if (iWinnerPlace == 0)
                        break;
                    else if (iWinnerPlace < 1 || iWinnerPlace > 4)
                        iWinnerPlace = 1;

                    strcpy(ts->wsbBonuses[ts->iNumBonuses].szBonusString, pszStart);

                    ts->wsbBonuses[ts->iNumBonuses].iWinnerPlace = iWinnerPlace - 1;

                    short iPowerupOffset = 0;
                    if (pszStart[1] == 'w' || pszStart[1] == 'W')
                        iPowerupOffset += NUM_POWERUPS;

                    pszStart += 2;

                    short iBonus = atoi(pszStart) + iPowerupOffset;
                    if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS)
                        iBonus = 0;

                    ts->wsbBonuses[ts->iNumBonuses].iBonus = iBonus;

                    if (++ts->iNumBonuses >= 10)
                        break;

                    if (pszEnd)
                        pszStart = pszEnd + 1;
                    else
                        pszStart = NULL;
                }
            } else {
                if (pszTemp)
                    ts->iBonusType = atoi(pszTemp);
                else
                    ts->iBonusType = 0;
            }

            pszTemp = strtok(NULL, ",\n");

            if (pszTemp) {
                strncpy(ts->szName, pszTemp, 127);
                ts->szName[127] = 0;
            } else {
                sprintf(ts->szName, "Tour Stop %d", game_values.tourstoptotal + 1);
            }
        } else {
            ts->iPoints = 1;
            ts->iBonusType = 0;
            sprintf(ts->szName, "Tour Stop %d", game_values.tourstoptotal + 1);
        }

        if (VersionIsEqualOrAfter(iVersion, 1, 8, 0, 0)) {
            if (fIsWorld) {
                //is this a world ending stage?
                pszTemp = strtok(NULL, ",\n");

                if (pszTemp)
                    ts->fEndStage = pszTemp[0] == '1';
                else
                    ts->fEndStage = false;
            }

            //Copy in default values first
            memcpy(&ts->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

            if (ts->iMode == 0) { //classic
                ts->fUseSettings = true;
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.classic.style, NULL, game_values.gamemodemenusettings.classic.style, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.classic.scoring, NULL, game_values.gamemodemenusettings.classic.scoring, false);
            } else if (ts->iMode == 1) { //frag
                ts->fUseSettings = true;
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frag.style, NULL, game_values.gamemodemenusettings.frag.style, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frag.scoring, NULL, game_values.gamemodemenusettings.frag.scoring, false);
            } else if (ts->iMode == 2) { //time
                ts->fUseSettings = true;
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.time.style, NULL, game_values.gamemodemenusettings.time.style, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.time.scoring, NULL, game_values.gamemodemenusettings.time.scoring, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.time.percentextratime, NULL, game_values.gamemodemenusettings.time.percentextratime, false);
            } else if (ts->iMode == 3) { //jail
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.jail.style, NULL, game_values.gamemodemenusettings.jail.style, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.jail.timetofree, NULL, game_values.gamemodemenusettings.jail.timetofree, false);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.jail.tagfree, 0, game_values.gamemodemenusettings.jail.tagfree);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.jail.percentkey, NULL, game_values.gamemodemenusettings.jail.percentkey, false);
            } else if (ts->iMode == 4) { //coins
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.coins.penalty, 0, game_values.gamemodemenusettings.coins.penalty);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.coins.quantity, NULL, game_values.gamemodemenusettings.coins.quantity, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.coins.percentextracoin, NULL, game_values.gamemodemenusettings.coins.percentextracoin, false);
            } else if (ts->iMode == 5) { //stomp
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.stomp.rate, NULL, game_values.gamemodemenusettings.stomp.rate, false);

                for (int iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
                    ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.stomp.enemyweight[iEnemy]), NULL, game_values.gamemodemenusettings.stomp.enemyweight[iEnemy], false);
            } else if (ts->iMode == 6) { //egg
                ts->fUseSettings = true;

                for (int iEgg = 0; iEgg < 4; iEgg++)
                    ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.egg.eggs[iEgg]), NULL, game_values.gamemodemenusettings.egg.eggs[iEgg], false);

                for (int iYoshi = 0; iYoshi < 4; iYoshi++)
                    ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.egg.yoshis[iYoshi]), NULL, game_values.gamemodemenusettings.egg.yoshis[iYoshi], false);

                ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.egg.explode), NULL, game_values.gamemodemenusettings.egg.explode, false);
            } else if (ts->iMode == 7) { //capture the flag
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.flag.speed, NULL, game_values.gamemodemenusettings.flag.speed, false);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.touchreturn, 0, game_values.gamemodemenusettings.flag.touchreturn);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.pointmove, 0, game_values.gamemodemenusettings.flag.pointmove);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.flag.autoreturn, NULL, game_values.gamemodemenusettings.flag.autoreturn, false);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.homescore, 0, game_values.gamemodemenusettings.flag.homescore);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.centerflag, 0, game_values.gamemodemenusettings.flag.centerflag);
            } else if (ts->iMode == 8) { //chicken
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.chicken.usetarget, 0, game_values.gamemodemenusettings.chicken.usetarget);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.chicken.glide, 0, game_values.gamemodemenusettings.chicken.glide);
            } else if (ts->iMode == 9) { //tag
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.tag.tagontouch, 0, game_values.gamemodemenusettings.tag.tagontouch);
            } else if (ts->iMode == 10) { //star
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.star.time, NULL, game_values.gamemodemenusettings.star.time, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.star.shine, NULL, game_values.gamemodemenusettings.star.shine, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.star.percentextratime, NULL, game_values.gamemodemenusettings.star.percentextratime, false);
            } else if (ts->iMode == 11) { //domination
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.domination.quantity, NULL, game_values.gamemodemenusettings.domination.quantity, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.domination.relocationfrequency, NULL, game_values.gamemodemenusettings.domination.relocationfrequency, false);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.domination.loseondeath, 0, game_values.gamemodemenusettings.domination.loseondeath);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.domination.relocateondeath, 0, game_values.gamemodemenusettings.domination.relocateondeath);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.domination.stealondeath, 0, game_values.gamemodemenusettings.domination.stealondeath);
            } else if (ts->iMode == 12) { //king of the hill
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.kingofthehill.areasize, NULL, game_values.gamemodemenusettings.kingofthehill.areasize, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.kingofthehill.relocationfrequency, NULL, game_values.gamemodemenusettings.kingofthehill.relocationfrequency, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.kingofthehill.maxmultiplier, NULL, game_values.gamemodemenusettings.kingofthehill.maxmultiplier, false);
            } else if (ts->iMode == 13) { //race
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.race.quantity, NULL, game_values.gamemodemenusettings.race.quantity, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.race.speed, NULL, game_values.gamemodemenusettings.race.speed, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.race.penalty, NULL, game_values.gamemodemenusettings.race.penalty, false);
            } else if (ts->iMode == 15) { //frenzy
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frenzy.quantity, NULL, game_values.gamemodemenusettings.frenzy.quantity, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frenzy.rate, NULL, game_values.gamemodemenusettings.frenzy.rate, false);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.frenzy.storedshells, 0, game_values.gamemodemenusettings.frenzy.storedshells);

                for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
                    ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.frenzy.powerupweight[iPowerup]), NULL, game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup], false);
            } else if (ts->iMode == 16) { //survival
                ts->fUseSettings = true;

                for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
                    ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.survival.enemyweight[iEnemy]), NULL, game_values.gamemodemenusettings.survival.enemyweight[iEnemy], false);

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.survival.density, NULL, game_values.gamemodemenusettings.survival.density, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.survival.speed, NULL, game_values.gamemodemenusettings.survival.speed, false);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.survival.shield, 0, game_values.gamemodemenusettings.survival.shield);
            } else if (ts->iMode == 17) { //greed
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.greed.coinlife, NULL, game_values.gamemodemenusettings.greed.coinlife, false);
                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.greed.owncoins, 0, game_values.gamemodemenusettings.greed.owncoins);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.greed.multiplier, NULL, game_values.gamemodemenusettings.greed.multiplier, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.greed.percentextracoin, NULL, game_values.gamemodemenusettings.greed.percentextracoin, false);
            } else if (ts->iMode == 18) { //health
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.health.startlife, NULL, game_values.gamemodemenusettings.health.startlife, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.health.maxlife, NULL, game_values.gamemodemenusettings.health.maxlife, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.health.percentextralife, NULL, game_values.gamemodemenusettings.health.percentextralife, false);
            } else if (ts->iMode == 19) { //card collection
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.collection.quantity, NULL, game_values.gamemodemenusettings.collection.quantity, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.collection.rate, NULL, game_values.gamemodemenusettings.collection.rate, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.collection.banktime, NULL, game_values.gamemodemenusettings.collection.banktime, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.collection.cardlife, NULL, game_values.gamemodemenusettings.collection.cardlife, false);
            } else if (ts->iMode == 20) { //chase (phanto)
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.chase.phantospeed, NULL, game_values.gamemodemenusettings.chase.phantospeed, false);

                for (short iPhanto = 0; iPhanto < 3; iPhanto++)
                    ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.chase.phantoquantity[iPhanto], NULL, game_values.gamemodemenusettings.chase.phantoquantity[iPhanto], false);
            } else if (ts->iMode == 21) { //shyguy tag
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.shyguytag.tagonsuicide, 0, game_values.gamemodemenusettings.shyguytag.tagonsuicide);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.shyguytag.tagtransfer, NULL, game_values.gamemodemenusettings.shyguytag.tagtransfer, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.shyguytag.freetime, NULL, game_values.gamemodemenusettings.shyguytag.freetime, false);
            } else if (ts->iMode == 1001) { //boss minigame
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.boss.bosstype, NULL, game_values.gamemodemenusettings.boss.bosstype, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.boss.difficulty, NULL, game_values.gamemodemenusettings.boss.difficulty, false);
                ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.boss.hitpoints, NULL, game_values.gamemodemenusettings.boss.hitpoints, false);
            }
        }
    } else if (iStageType == 1) { //Bonus House
        if (pszTemp) {
            strncpy(ts->szName, pszTemp, 127);
            ts->szName[127] = 0;
        } else {
            sprintf(ts->szName, "Bonus House %d", game_values.tourstoptotal + 1);
        }

        pszTemp = strtok(NULL, ",\n");

        short iBonusOrdering = atoi(pszTemp);
        if (iBonusOrdering < 0 || iBonusOrdering > 1)
            iBonusOrdering = 0;

        ts->iBonusType = iBonusOrdering;

        pszTemp = strtok(NULL, ",\n");

        char * pszStart = pszTemp;

        ts->iBonusTextLines = 0;
        while (pszStart != NULL && pszStart[0] != '-') {
            char * pszEnd = strstr(pszStart, "|");

            if (pszEnd)
                *pszEnd = 0;

            strcpy(ts->szBonusText[ts->iBonusTextLines], pszStart);

            if (++ts->iBonusTextLines >= 5 || !pszEnd)
                break;

            pszStart = pszEnd + 1;
        }

        ts->iNumBonuses = 0;
        pszTemp = strtok(NULL, ",\n");
        while (pszTemp) {
            strcpy(ts->wsbBonuses[ts->iNumBonuses].szBonusString, pszTemp);

            short iPowerupOffset = 0;
            if (pszTemp[0] == 'w' || pszTemp[0] == 'W')
                iPowerupOffset += NUM_POWERUPS;
            else if (pszTemp[0] == 's' || pszTemp[0] == 'S')
                iPowerupOffset += NUM_POWERUPS + NUM_WORLD_POWERUPS - 1;

            pszTemp++;

            short iBonus = atoi(pszTemp) + iPowerupOffset;
            if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS + NUM_WORLD_SCORE_BONUSES)
                iBonus = 0;

            ts->wsbBonuses[ts->iNumBonuses].iBonus = iBonus;
            ts->wsbBonuses[ts->iNumBonuses].iWinnerPlace = -1;

            if (++ts->iNumBonuses >= MAX_BONUS_CHESTS)
                break;

            pszTemp = strtok(NULL, ",\n");
        }
    }

    return ts;
}

void WriteTourStopLine(TourStop * ts, char * buffer, bool fIsWorld)
{
    buffer[0] = 0;
    char szTemp[32];

    if (fIsWorld) {
        //Write stage type (battle stage vs. bonus house, etc.)
        sprintf(szTemp, "%d,", ts->iStageType);
        strcat(buffer, szTemp);
    }

    //Battle stage
    if (ts->iStageType == 0) {
        strcat(buffer, ts->pszMapFile);
        strcat(buffer, ",");

        sprintf(szTemp, "%d,", ts->iMode);
        strcat(buffer, szTemp);

        sprintf(szTemp, "%d,", ts->iGoal);
        strcat(buffer, szTemp);

        sprintf(szTemp, "%d,", ts->iPoints);
        strcat(buffer, szTemp);

        if (fIsWorld) {
            if (ts->iNumBonuses <= 0) {
                strcat(buffer, "0");
            } else {
                for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
                    if (iBonus > 0)
                        strcat(buffer, "|");

                    strcat(buffer, ts->wsbBonuses[iBonus].szBonusString);
                }
            }

            strcat(buffer, ",");
        } else {
            sprintf(szTemp, "%d,", ts->iBonusType);
            strcat(buffer, szTemp);
        }

        strcat(buffer, ts->szName);
        strcat(buffer, ",");

        if (fIsWorld) {
            sprintf(szTemp, "%d", ts->fEndStage);
            strcat(buffer, szTemp);
        }

        if (ts->fUseSettings) {
            if (ts->iMode == 0) { //classic
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.classic.style);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.classic.scoring);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 1) { //frag
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frag.style);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frag.scoring);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 2) { //time
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.time.style);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.time.scoring);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.time.percentextratime);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 3) { //jail
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.jail.style);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.jail.timetofree);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.jail.tagfree);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.jail.percentkey);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 4) { //coins
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.coins.penalty);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.coins.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.coins.percentextracoin);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 5) { //stomp
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.stomp.rate);
                    strcat(buffer, szTemp);
                }

                for (int iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++) {
                    if (ts->iNumUsedSettings > iEnemy + 1) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.stomp.enemyweight[iEnemy]);
                        strcat(buffer, szTemp);
                    }
                }
            } else if (ts->iMode == 6) { //egg
                for (int iEgg = 0; iEgg < 4; iEgg++) {
                    if (ts->iNumUsedSettings > iEgg) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.egg.eggs[iEgg]);
                        strcat(buffer, szTemp);
                    }
                }

                for (int iYoshi = 0; iYoshi < 4; iYoshi++) {
                    if (ts->iNumUsedSettings > iYoshi + 4) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.egg.yoshis[iYoshi]);
                        strcat(buffer, szTemp);
                    }
                }

                if (ts->iNumUsedSettings > 8) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.egg.explode);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 7) { //capture the flag
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.speed);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.touchreturn);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.pointmove);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.autoreturn);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 4) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.homescore);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 5) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.centerflag);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 8) { //chicken
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.chicken.usetarget);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.chicken.glide);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 9) { //tag
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.tag.tagontouch);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 10) { //star
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.star.time);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.star.shine);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.star.percentextratime);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 11) { //domination
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.relocationfrequency);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.loseondeath);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.relocateondeath);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 4) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.stealondeath);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 12) { //king of the hill
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.areasize);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.relocationfrequency);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.maxmultiplier);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 13) { //race
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.race.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.race.speed);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.race.penalty);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 15) { //frenzy
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.rate);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.storedshells);
                    strcat(buffer, szTemp);
                }

                for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
                    if (ts->iNumUsedSettings > iPowerup + 3) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.powerupweight[iPowerup]);
                        strcat(buffer, szTemp);
                    }
                }
            } else if (ts->iMode == 16) { //survival
                for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++) {
                    if (ts->iNumUsedSettings > iEnemy) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.survival.enemyweight[iEnemy]);
                        strcat(buffer, szTemp);
                    }
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.survival.density);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 4) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.survival.speed);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 5) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.survival.shield);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 17) { //greed
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.coinlife);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.owncoins);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.multiplier);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.percentextracoin);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 18) { //health
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.health.startlife);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.health.maxlife);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.health.percentextralife);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 19) { //card collection
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.rate);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.banktime);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.cardlife);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 20) { //phanto chase mode
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.chase.phantospeed);
                    strcat(buffer, szTemp);
                }

                for (short iPhanto = 0; iPhanto < 3; iPhanto++) {
                    if (ts->iNumUsedSettings > iPhanto + 1) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.chase.phantoquantity[iPhanto]);
                        strcat(buffer, szTemp);
                    }
                }
            } else if (ts->iMode == 21) { //shyguy tag
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.shyguytag.tagonsuicide);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.shyguytag.tagtransfer);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.shyguytag.freetime);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == 1001) { //boss minigame
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.boss.bosstype);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.boss.difficulty);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.boss.hitpoints);
                    strcat(buffer, szTemp);
                }
            }
        }
    } else if (ts->iStageType == 1) { //Bonus House
        strcat(buffer, ts->szName);
        strcat(buffer, ",");

        sprintf(szTemp, "%d,", ts->iBonusType);
        strcat(buffer, szTemp);

        for (short iText = 0; iText < ts->iBonusTextLines; iText++) {
            if (iText != 0)
                strcat(buffer, "|");

            strcat(buffer, ts->szBonusText[iText]);
        }

        if (ts->iNumBonuses == 0) {
            strcat(buffer, ",p0");
        } else {
            for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
                strcat(buffer, ",");
                strcat(buffer, ts->wsbBonuses[iBonus].szBonusString);
            }
        }
    }

    strcat(buffer, "\n");
}

void ResetTourStops()
{
    game_values.tourstopcurrent = 0;
    game_values.tourstoptotal = 0;

    // added to prevent 'vector iterators incompatible' exception
    if (!game_values.tourstops.empty())
        game_values.tourstops.clear();
}
