#include "world.h"

#include "FileList.h"
#include "GameValues.h"
#include "path.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Version.h"
#include "WorldTourStop.h"

#include <cstdio>
#include <cstdlib> // atoi()
#include <cstring>

#include <fstream>
#include <map>
#include <queue>
#include <string>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif

WorldMap g_worldmap;

extern std::string stripPathAndExtension(const std::string &path);

extern SDL_Surface* blitdest;

extern CGameValues game_values;
extern CResourceManager* rm;

extern SkinList *skinlist;
extern WorldList *worldlist;


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

void WorldPlayer::Draw(short iMapOffsetX, short iMapOffsetY) const
{
    rm->spr_player[iDrawSprite][iAnimationFrame + iDrawDirection]->draw(ix + iMapOffsetX, iy + iMapOffsetY, 0, 0, 32, 32);
}

void WorldPlayer::SetSprite(short iPlayer)
{
    while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], true)) {
        if (++game_values.skinids[iPlayer] >= skinlist->count())
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
        srcRects[iRect] = {iRect * tilesize + iRectOffsetX, iRectOffsetY, tilesize, tilesize};

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

void WorldVehicle::Draw(short iWorldOffsetX, short iWorldOffsetY, bool fVehiclesSleeping) const
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

void WorldWarp::GetOtherSide(short iCol, short iRow, short * iOtherCol, short * iOtherRow) const
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
{}

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

    const std::string szPath = worldlist->at(game_values.worldindex);
    worldName = stripPathAndExtension(szPath);

    std::ifstream file(szPath);
    if (!file)
        return false;

    std::string line;
    char* buffer = NULL;
    short iReadType = 0;
    Version version;
    short iMapTileReadRow = 0;
    short iCurrentStage = 0;
    short iNumWarps = 0;
    short iNumVehicles = 0;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        if (buffer)
            delete[] buffer;

        buffer = new char[line.size() + 1];
        std::copy(line.begin(), line.end(), buffer);
        buffer[line.size()] = '\0';

        if (buffer[0] == '#' || buffer[0] == '\r' || buffer[0] == ' ' || buffer[0] == '\t')
            continue;

        if (iReadType == 0) { //Read version number
            char * psz = strtok(buffer, ".\n");
            if (psz)
                version.major = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                version.minor = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                version.patch = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                version.build = atoi(psz);

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

            tiles.clear();
            tiles.resize(iWidth);
            for (short iCol = 0; iCol < iWidth; iCol++)
                tiles[iCol].resize(iHeight);

            short iDrawSurfaceTiles = iWidth * iHeight;

            if (iDrawSurfaceTiles > 456)
                iDrawSurfaceTiles = 456; //19 * 24 = 456 max tiles in world surface

            iTilesPerCycle = iDrawSurfaceTiles / 8;
        } else if (iReadType == 4) { //background water
            char * psz = strtok(buffer, ",\n");

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                if (!psz)
                    goto RETURN;

                WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
                tile.iBackgroundWater = atoi(psz);

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

                WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
                tile.iBackgroundSprite = atoi(psz);
                tile.fAnimated = (tile.iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE) != 1;

                tile.iID = iMapTileReadRow * iWidth + iMapTileReadCol;
                tile.iCol = iMapTileReadCol;
                tile.iRow = iMapTileReadRow;

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

                WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
                tile.iForegroundSprite = atoi(psz);

                short iForegroundSprite = tile.iForegroundSprite;

                //Animated parts of paths
                if (!tile.fAnimated && iForegroundSprite >= 0 && iForegroundSprite <= 8 * WORLD_PATH_SPRITE_SET_SIZE) {
                    short iForeground = iForegroundSprite % WORLD_PATH_SPRITE_SET_SIZE;
                    tile.fAnimated = iForeground >= 3 && iForeground <= 10;
                }

                //Animated 1-100 stages
                if (!tile.fAnimated)
                    tile.fAnimated = iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399;

                //Animated foreground tiles
                if (!tile.fAnimated)
                    tile.fAnimated = iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29;

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

                WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
                tile.iConnectionType = atoi(psz);

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

                WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
                tile.iType = atoi(psz);
                tile.iWarp = -1;

                if (tile.iType == 1) {
                    iStartX = iMapTileReadCol;
                    iStartY = iMapTileReadRow;
                }

                tile.iCompleted = tile.iType <= 5 ? -1 : -2;

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

                WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
                tile.iVehicleBoundary = atoi(psz);

                psz = strtok(NULL, ",\n");
            }

            if (++iMapTileReadRow == iHeight)
                iReadType = 10;
        } else if (iReadType == 10) { //number of stages
            iNumStages = atoi(buffer);

            iReadType = iNumStages == 0 ? 12 : 11;
        } else if (iReadType == 11) { //stage details
            TourStop* ts = new TourStop();
            *ts = ParseTourStopLine(buffer, version, true);

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
                warps.reserve(iNumWarps);

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

            short warpId = warps.size();

            warps.emplace_back(WorldWarp());
            warps.back().Init(warpId, iCol1, iRow1, iCol2, iRow2);

            tiles[iCol1][iRow1].iWarp = warpId;
            tiles[iCol2][iRow2].iWarp = warpId;

            if (warps.size() >= iNumWarps)
                iReadType = 14;
        } else if (iReadType == 14) { //number of vehicles
            iNumVehicles = atoi(buffer);

            if (iNumVehicles < 0)
                iNumVehicles = 0;

            if (iNumVehicles > 0)
                vehicles.reserve(iNumVehicles);

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

            vehicles.emplace_back(WorldVehicle());
            vehicles.back().Init(iCol, iRow, iStage, iSprite, iMinMoves, iMaxMoves, fSpritePaces, iInitialDirection, iBoundary, iTileSize);

            if (vehicles.size() >= iNumVehicles)
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
    if (buffer)
        delete[] buffer;

    return iReadType == 17;
}

void WorldMap::SetTileConnections(short iCol, short iRow)
{
    if (iCol < 0 || iRow < 0 || iCol >= iWidth || iRow >= iHeight)
        return;

    WorldMapTile& tile = tiles[iCol][iRow];

    for (short iDirection = 0; iDirection < 4; iDirection++)
        tile.fConnection[iDirection] = false;

    if (iRow > 0) {
        const WorldMapTile& topTile = tiles[iCol][iRow - 1];

        tile.fConnection[0] = (topTile.iConnectionType == 1 || topTile.iConnectionType == 5 || topTile.iConnectionType == 6 ||
                                topTile.iConnectionType == 7 || topTile.iConnectionType == 9 || topTile.iConnectionType == 10 ||
                                topTile.iConnectionType == 11 || topTile.iConnectionType == 15) && (tile.iConnectionType == 1 ||
                                        tile.iConnectionType == 3 || tile.iConnectionType == 4 || tile.iConnectionType == 7 ||
                                        tile.iConnectionType == 8 || tile.iConnectionType == 9 || tile.iConnectionType == 11 ||
                                        tile.iConnectionType == 15);
    }

    if (iRow < iHeight - 1) {
        const WorldMapTile& bottomTile = tiles[iCol][iRow + 1];

        tile.fConnection[1] = (bottomTile.iConnectionType == 1 || bottomTile.iConnectionType == 3 || bottomTile.iConnectionType == 4 ||
                                bottomTile.iConnectionType == 7 || bottomTile.iConnectionType == 8 || bottomTile.iConnectionType == 9 ||
                                bottomTile.iConnectionType == 11 || bottomTile.iConnectionType == 15) && (tile.iConnectionType == 1 ||
                                        tile.iConnectionType == 5 || tile.iConnectionType == 6 || tile.iConnectionType == 7 ||
                                        tile.iConnectionType == 9 || tile.iConnectionType == 10 || tile.iConnectionType == 11 ||
                                        tile.iConnectionType == 15);
    }

    if (iCol > 0) {
        const WorldMapTile& leftTile = tiles[iCol - 1][iRow];

        tile.fConnection[2] = (leftTile.iConnectionType == 2 || leftTile.iConnectionType == 4 || leftTile.iConnectionType == 5 ||
                                leftTile.iConnectionType == 8 || leftTile.iConnectionType == 9 || leftTile.iConnectionType == 10 ||
                                leftTile.iConnectionType == 11 || leftTile.iConnectionType == 13) && (tile.iConnectionType == 2 || tile.iConnectionType == 3 ||
                                        tile.iConnectionType == 6 || tile.iConnectionType == 7 || tile.iConnectionType == 8 ||
                                        tile.iConnectionType == 10 || tile.iConnectionType == 11 || tile.iConnectionType == 13);
    }

    if (iCol < iWidth - 1) {
        const WorldMapTile& rightTile = tiles[iCol + 1][iRow];

        tile.fConnection[3] = (rightTile.iConnectionType == 2 || rightTile.iConnectionType == 3 || rightTile.iConnectionType == 6 ||
                                rightTile.iConnectionType == 7 || rightTile.iConnectionType == 8 || rightTile.iConnectionType == 10 ||
                                rightTile.iConnectionType == 11 || rightTile.iConnectionType == 13) && (tile.iConnectionType == 2 || tile.iConnectionType == 4 ||
                                        tile.iConnectionType == 5 || tile.iConnectionType == 8 || tile.iConnectionType == 9 ||
                                        tile.iConnectionType == 10 || tile.iConnectionType == 11 || tile.iConnectionType == 13);
    }
}

//Saves world to file
bool WorldMap::Save() const
{
    return Save(worldlist->at(game_values.worldindex));
}

bool WorldMap::Save(const std::string& szPath) const
{
    FILE * file = fopen(szPath.c_str(), "w");

    if (!file)
        return false;

    fprintf(file, "#Version\n");
    // For compatibility, let's use the final 1.8 version until
    // there's no actual change in the world format.
    fprintf(file, "1.8.0.4\n\n");
    //fprintf(file, "%d.%d.%d.%d\n\n", g_iVersion[0], g_iVersion[1], g_iVersion[2], g_iVersion[3]);

    fprintf(file, "#Music Category\n");
    fprintf(file, "%d\n\n", iMusicCategory);

    fprintf(file, "#Width\n");
    fprintf(file, "%d\n\n", iWidth);

    fprintf(file, "#Height\n");
    fprintf(file, "%d\n\n", iHeight);

    fprintf(file, "#Sprite Water Layer\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < iHeight; iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
            const WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile.iBackgroundWater);

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
            const WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile.iBackgroundSprite);

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
            const WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile.iForegroundSprite);

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
            const WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile.iConnectionType);

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
            const WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile.iType);

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
            const WorldMapTile& tile = tiles[iMapTileReadCol][iMapTileReadRow];
            fprintf(file, "%d", tile.iVehicleBoundary);

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
        std::string line = WriteTourStopLine(*game_values.tourstops[iStage], true);
        fprintf(file, "%s", line.c_str());
    }
    fprintf(file, "\n");

    fprintf(file, "#Warps\n");
    fprintf(file, "#location 1 x, y, location 2 x, y\n");

    fprintf(file, "%d\n", warps.size());

    for (const WorldWarp& warp : warps) {
        fprintf(file, "%d,", warp.iCol1);
        fprintf(file, "%d,", warp.iRow1);
        fprintf(file, "%d,", warp.iCol2);
        fprintf(file, "%d\n", warp.iRow2);
    }
    fprintf(file, "\n");

    fprintf(file, "#Vehicles\n");
    fprintf(file, "#Sprite,Stage Type, Start Column, Start Row, Min Moves, Max Moves, Sprite Paces, Sprite Direction, Boundary\n");

    fprintf(file, "%d\n", vehicles.size());

    for (short iVehicle = 0; iVehicle < vehicles.size(); iVehicle++) {
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
    chmod(szPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif

    return true;
}

void WorldMap::Clear()
{
    for (size_t col = 0; col < tiles.size(); col++) {
        std::vector<WorldMapTile>& column = tiles[col];

        for (size_t row = 0; row < column.size(); row++) {
            WorldMapTile& tile = column[row];

            tile.iBackgroundSprite = 0;
            tile.iBackgroundWater = 0;
            tile.iForegroundSprite = 0;
            tile.iConnectionType = 0;
            tile.iType = 0;
            tile.iID = row * iWidth + col;
            tile.iVehicleBoundary = 0;
            tile.iWarp = 0;
        }
    }

    vehicles.clear();
    warps.clear();
}

//Creates clears world and resizes (essentially creating a new world to work on for editor)
void WorldMap::New(short w, short h)
{
    Cleanup();

    iWidth = w;
    iHeight = h;

    tiles.clear();
    tiles.resize(iWidth);
    for (short iCol = 0; iCol < iWidth; iCol++)
        tiles[iCol].resize(iHeight);

    Clear();
}

//Resizes world keeping intact current tiles (if possible)
void WorldMap::Resize(short w, short h)
{
    //Copy tiles from old map
    std::vector<std::vector<WorldMapTile>> tempTiles;
    tempTiles.swap(tiles);

    short iOldWidth = iWidth;
    short iOldHeight = iHeight;

    //Create new map
    iWidth = w;
    iHeight = h;

    tiles.resize(iWidth);

    //Copy tiles to new map
    for (short iCol = 0; iCol < iWidth; iCol++) {
        tiles[iCol].resize(iHeight);

        for (short iRow = 0; iRow < iHeight; iRow++) {
            if (iCol < iOldWidth && iRow < iOldHeight)
                tiles[iCol][iRow] = tempTiles[iCol][iRow];
            else {
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
    for (WorldVehicle& vehicle : vehicles) {
        if (!vehicle.fEnabled)
            continue;

        *fPlayerVehicleCollision |= vehicle.Update();

        if (vehicle.iState > 0)
            fPlayMovingVehicleSound = true;
    }

    if (fPlayMovingVehicleSound && !rm->sfx_boomerang.isPlaying())
        ifSoundOnPlay(rm->sfx_boomerang);

    return fPlayerDoneMove;
}

void WorldMap::Draw(short iMapOffsetX, short iMapOffsetY, bool fDrawPlayer, bool fVehiclesSleeping) const
{
    for (const WorldVehicle& vehicle : vehicles) {
        if (!vehicle.fEnabled)
            continue;

        vehicle.Draw(iMapOffsetX, iMapOffsetY, fVehiclesSleeping);
    }

    if (fDrawPlayer)
        player.Draw(iMapOffsetX, iMapOffsetY);
}

void WorldMap::UpdateTile(SDL_Surface * surface, short iCol, short iRow, short iMapDrawOffsetCol, short iMapDrawOffsetRow, short iAnimationFrame)
{
    DrawTileToSurface(surface, iCol, iRow, iMapDrawOffsetCol, iMapDrawOffsetRow, true, iAnimationFrame);
}

void WorldMap::DrawMapToSurface(SDL_Surface * surface) const
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

void WorldMap::DrawMapToSurface(short iCycleIndex, bool fFullRefresh, SDL_Surface* surface, short iMapDrawOffsetCol, short iMapDrawOffsetRow, short iAnimationFrame)
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

void WorldMap::DrawTileToSurface(SDL_Surface* surface, short iCol, short iRow, short iMapDrawOffsetCol, short iMapDrawOffsetRow, bool fFullRefresh, short iAnimationFrame, short iLayer) const
{
    const WorldMapTile& tile = tiles[iCol + iMapDrawOffsetCol][iRow + iMapDrawOffsetRow];

    if (!tile.fAnimated && !fFullRefresh)
        return;

    SDL_Rect r = {iCol * iTileSize, iRow * iTileSize, iTileSize, iTileSize};

    short iBackgroundSprite = tile.iBackgroundSprite;
    short iBackgroundWater = tile.iBackgroundWater;
    short iForegroundSprite = tile.iForegroundSprite;

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
                    rSrc = {(3 << iTileSizeShift) + iBackgroundStyleOffset, (iBackgroundSprite - 44) << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else if (iBackgroundSprite >= 30) {
                    rSrc = {(2 << iTileSizeShift) + iBackgroundStyleOffset, (iBackgroundSprite - 29) << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else if (iBackgroundSprite >= 16) {
                    rSrc = {iTileSize + iBackgroundStyleOffset, (iBackgroundSprite - 14) << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                } else {
                    rSrc = {iBackgroundStyleOffset, iBackgroundSprite << iTileSizeShift, iTileSize, iTileSize};
                    SDL_BlitSurface(rm->spr_worldbackground[iTileSheet].getSurface(), &rSrc, surface, &r);
                }
            }
        }
    }

    if (iLayer != 1) {
        if (tile.iCompleted >= 0) {
            SDL_Rect rSrc = {(tile.iCompleted + 10) << iTileSizeShift, 5 << iTileSizeShift, iTileSize, iTileSize};
            SDL_BlitSurface(rm->spr_worldforegroundspecial[iTileSheet].getSurface(), &rSrc, surface, &r);
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
                SDL_BlitSurface(rm->spr_worldforegroundspecial[iTileSheet].getSurface(), &rSrc, surface, &r);

                short iTileNumber = (iForegroundSprite - WORLD_FOREGROUND_STAGE_OFFSET) % 100;
                rSrc.x = (iTileNumber % 10) << iTileSizeShift;
                rSrc.y = (iTileNumber / 10) << iTileSizeShift;
                SDL_BlitSurface(rm->spr_worldforegroundspecial[iTileSheet].getSurface(), &rSrc, surface, &r);
            } else if (iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) {
                SDL_Rect rSrc = {(iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 10) << iTileSizeShift, 7 << iTileSizeShift, iTileSize, iTileSize};
                SDL_BlitSurface(rm->spr_worldforegroundspecial[iTileSheet].getSurface(), &rSrc, surface, &r);
            } else if (iForegroundSprite >= WORLD_START_SPRITE_OFFSET && iForegroundSprite <= WORLD_START_SPRITE_OFFSET + 1) {
                SDL_Rect rSrc = {(iForegroundSprite - WORLD_START_SPRITE_OFFSET + 10) << iTileSizeShift, 4 << iTileSizeShift, iTileSize, iTileSize};
                SDL_BlitSurface(rm->spr_worldforegroundspecial[iTileSheet].getSurface(), &rSrc, surface, &r);
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
        short iType = tile.iType;
        if (iType >= 2 && iType <= 5) {
            SDL_Rect rSrc = {(iType + 8) << iTileSizeShift, 6 << iTileSizeShift, iTileSize, iTileSize};
            SDL_BlitSurface(rm->spr_worldforegroundspecial[iTileSheet].getSurface(), &rSrc, surface, &r);
        }
    }
}

void WorldMap::Cleanup()
{
    ResetTourStops();
    iNumStages = 0;
    iNumInitialBonuses = 0;

    tiles.clear();
    vehicles.clear();
    warps.clear();
}

void WorldMap::SetPlayerSprite(short iPlayerSprite)
{
    player.SetSprite(iPlayerSprite);
}

bool WorldMap::IsVehicleMoving() const
{
    for (const WorldVehicle& vehicle : vehicles) {
        if (vehicle.fEnabled && vehicle.iState > 0)
            return true;
    }
    return false;
}

void WorldMap::GetPlayerPosition(short * iPlayerX, short * iPlayerY) const
{
    *iPlayerX = player.ix;
    *iPlayerY = player.iy;
}

void WorldMap::SetPlayerPosition(short iPlayerCol, short iPlayerRow)
{
    player.SetPosition(iPlayerCol, iPlayerRow);
}

void WorldMap::GetPlayerCurrentTile(short * iPlayerCurrentTileX, short * iPlayerCurrentTileY) const
{
    *iPlayerCurrentTileX = player.iCurrentTileX;
    *iPlayerCurrentTileY = player.iCurrentTileY;
}

void WorldMap::GetPlayerDestTile(short * iPlayerDestTileX, short * iPlayerDestTileY) const
{
    *iPlayerDestTileX = player.iDestTileX;
    *iPlayerDestTileY = player.iDestTileY;
}

short WorldMap::GetPlayerState() const
{
    return player.iState;
}

short WorldMap::GetVehicleInPlayerTile(short * vehicleIndex) const
{
    for (size_t i = 0; i < vehicles.size(); i++) {
        const WorldVehicle& vehicle = vehicles[i];

        if (!vehicle.fEnabled)
            continue;

        if (vehicle.iCurrentTileX == player.iCurrentTileX && vehicle.iCurrentTileY == player.iCurrentTileY) {
            *vehicleIndex = i;
            return vehicle.iActionId;
        }
    }

    *vehicleIndex = -1;
    return -1;
}

bool WorldMap::GetWarpInPlayerTile(short * iWarpCol, short * iWarpRow) const
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
    for (WorldVehicle& vehicle : vehicles) {
        if (vehicle.fEnabled)
            vehicle.Move();
    }
}

void WorldMap::RemoveVehicle(short iVehicleIndex)
{
    vehicles[iVehicleIndex].fEnabled = false;
}

short WorldMap::NumVehiclesInTile(short iTileX, short iTileY) const
{
    short iVehicleCount = 0;

    for (const WorldVehicle& vehicle : vehicles) {
        if (!vehicle.fEnabled)
            continue;

        if (vehicle.iCurrentTileX == iTileX && vehicle.iCurrentTileY == iTileY)
            iVehicleCount++;
    }

    return iVehicleCount;
}

short WorldMap::GetVehicleStageScore(short iVehicleIndex) const
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

void WorldMap::IsTouchingDoor(short iCol, short iRow, bool doors[4]) const
{
    const WorldMapTile& tile = tiles[iCol][iRow];

    if (iCol > 0) {
        if (tile.iCompleted >= -1) {
            short iType = tiles[iCol - 1][iRow].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iCol < iWidth - 1) {
        if (tile.iCompleted >= -1) {
            short iType = tiles[iCol + 1][iRow].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iRow > 0) {
        if (tile.iCompleted >= -1) {
            short iType = tiles[iCol][iRow - 1].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iCol < iHeight - 1) {
        if (tile.iCompleted >= -1) {
            short iType = tiles[iCol][iRow + 1].iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }
}

bool WorldMap::IsDoor(short iCol, short iRow) const
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

    const WorldMapTile& tile = tiles[iCol][iRow];

    if (iCol > 0) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles[iCol - 1][iRow].iType - 2 == iKeyType) {
            tiles[iCol - 1][iRow].iType = 0;
            iDoorsOpened |= 1;
        }
    }

    if (iCol < iWidth - 1) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles[iCol + 1][iRow].iType - 2 == iKeyType) {
            tiles[iCol + 1][iRow].iType = 0;
            iDoorsOpened |= 2;
        }
    }

    if (iRow > 0) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles[iCol][iRow - 1].iType - 2 == iKeyType) {
            tiles[iCol][iRow - 1].iType = 0;
            iDoorsOpened |= 4;
        }
    }

    if (iRow < iHeight - 1) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles[iCol][iRow + 1].iType - 2 == iKeyType) {
            tiles[iCol][iRow + 1].iType = 0;
            iDoorsOpened |= 8;
        }
    }

    return iDoorsOpened;
}

short WorldMap::GetVehicleBoundary(short iCol, short iRow) const
{
    if (iCol >= 0 && iRow >= 0 && iCol < iWidth && iRow < iHeight) {
        return tiles[iCol][iRow].iVehicleBoundary;
    }

    return 0;
}

//Implements breadth first search to find a stage or vehicle of interest
short WorldMap::GetNextInterestingMove(short iCol, short iRow) const
{
    const WorldMapTile& currentTile = tiles[iCol][iRow];

    //Look for stages or vehicles, but not bonus houses
    if ((currentTile.iType >= 6 && currentTile.iCompleted == -2) || NumVehiclesInTile(iCol, iRow) > 0)
        return 4; //Signal to press select on this tile

    short iCurrentId = currentTile.iID;

    std::queue<const WorldMapTile*> next;
    std::map<short, short> visitedTiles;
    visitedTiles[currentTile.iID] = -1;
    next.push(&currentTile);

    while (!next.empty()) {
        const WorldMapTile* const tile = next.front();

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
                    const WorldMapTile& topTile = tiles[tile->iCol][tile->iRow - 1];

                    //Stop at door tiles
                    if (topTile.iType >= 2 && topTile.iType <= 5)
                        continue;

                    if (visitedTiles.find(topTile.iID) == visitedTiles.end()) {
                        visitedTiles[topTile.iID] = 1;
                        next.push(&topTile);
                    }
                } else if (iNeighbor == 1 && tile->iRow < iHeight - 1) {
                    const WorldMapTile& bottomTile = tiles[tile->iCol][tile->iRow + 1];

                    //Stop at door tiles
                    if (bottomTile.iType >= 2 && bottomTile.iType <= 5)
                        continue;

                    if (visitedTiles.find(bottomTile.iID) == visitedTiles.end()) {
                        visitedTiles[bottomTile.iID] = 0;
                        next.push(&bottomTile);
                    }
                } else if (iNeighbor == 2 && tile->iCol > 0) {
                    const WorldMapTile& leftTile = tiles[tile->iCol - 1][tile->iRow];

                    //Stop at door tiles
                    if (leftTile.iType >= 2 && leftTile.iType <= 5)
                        continue;

                    if (visitedTiles.find(leftTile.iID) == visitedTiles.end()) {
                        visitedTiles[leftTile.iID] = 3;
                        next.push(&leftTile);
                    }
                } else if (iNeighbor == 3 && tile->iCol < iWidth - 1) {
                    const WorldMapTile& rightTile = tiles[tile->iCol + 1][tile->iRow];

                    //Stop at door tiles
                    if (rightTile.iType >= 2 && rightTile.iType <= 5)
                        continue;

                    if (visitedTiles.find(rightTile.iID) == visitedTiles.end()) {
                        visitedTiles[rightTile.iID] = 2;
                        next.push(&rightTile);
                    }
                }
            }

            if (tile->iWarp >= 0) {
                short iWarpCol, iWarpRow;
                warps[tile->iWarp].GetOtherSide(tile->iCol, tile->iRow, &iWarpCol, &iWarpRow);

                const WorldMapTile& warpTile = tiles[iWarpCol][iWarpRow];

                //Stop at door tiles
                if (warpTile.iType >= 2 && warpTile.iType <= 5)
                    continue;

                if (visitedTiles.find(warpTile.iID) == visitedTiles.end()) {
                    visitedTiles[warpTile.iID] = 4;
                    next.push(&warpTile);
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
