#include "world.h"

#include "FileList.h"
#include "GameValues.h"
#include "linfunc.h"
#include "path.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Version.h"
#include "WorldTourStop.h"

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <queue>
#include <string>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif

WorldMap g_worldmap(0, 0);

extern SDL_Surface* blitdest;

extern CGameValues game_values;
extern CResourceManager* rm;

extern SkinList *skinlist;


namespace {
std::string_view popNext(std::list<std::string_view>& list)
{
    std::string_view result;
    if (!list.empty()) {
        result = std::move(list.front());
        list.pop_front();
    }
    return result;
}

int toInt(std::string_view text, int defval = 0)
{
    int value = defval;
    std::from_chars(text.data(), text.data() + text.size(), value);  // TODO: Handle errors
    return value;
}

int popNextInt(std::list<std::string_view>& list, int defval = 0)
{
    return toInt(popNext(list));
}
} // namespace


/**********************************
* WorldMovingObject
**********************************/

WorldMovingObject::WorldMovingObject()
{
    SetPosition(0, 0);
}

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
        destTile.y--;
        iState = 1;
    } else if (iDirection == 1) {
        destTile.y++;
        iState = 2;
    } else if (iDirection == 2) {
        destTile.x--;
        iState = 3;
        iDrawDirection = 1;
    } else if (iDirection == 3) {
        destTile.x++;
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
        pos.y -= 2;
        if (pos.y < destTile.y * iTileSize) {
            pos.y = destTile.y * iTileSize;
            iState = 0;
            currentTile.y = destTile.y;

            return true;
        }
    } else if (iState == 2) { //down
        pos.y += 2;
        if (pos.y > destTile.y * iTileSize) {
            pos.y = destTile.y * iTileSize;
            iState = 0;
            currentTile.y = destTile.y;

            return true;
        }
    } else if (iState == 3) { //left
        pos.x -= 2;
        if (pos.x < destTile.x * iTileSize) {
            pos.x = destTile.x * iTileSize;
            iState = 0;
            currentTile.x = destTile.x;

            return true;
        }
    } else if (iState == 4) { //right
        pos.x += 2;
        if (pos.x > destTile.x * iTileSize) {
            pos.x = destTile.x * iTileSize;
            iState = 0;
            currentTile.x = destTile.x;

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
    pos = {iCol * iTileSize, iRow * iTileSize};
    currentTile = {iCol, iRow};
    destTile = {iCol, iRow};

    iState = 0;
    iAnimationFrame = 0;
    iAnimationTimer = 0;
}

/**********************************
* WorldPlayer
**********************************/

WorldPlayer::WorldPlayer()
    : WorldPlayer(0, 0)
{}

WorldPlayer::WorldPlayer(short iCol, short iRow)
    : WorldMovingObject()
{
    WorldMovingObject::Init(iCol, iRow, 0, 0, 32);
}

void WorldPlayer::Draw(short iMapOffsetX, short iMapOffsetY) const
{
    rm->spr_player[iDrawSprite][iAnimationFrame + iDrawDirection]->draw(pos.x + iMapOffsetX, pos.y + iMapOffsetY, 0, 0, 32, 32);
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

    WorldMapTile * tile = &g_worldmap.tiles.at(currentTile.x, currentTile.y);
    const Vec2s iPlayerCurrentTile = g_worldmap.GetPlayerCurrentTile();

    if (iNumMoves-- <= 0) {
        if (tile->iType == 0 && iPlayerCurrentTile != currentTile && g_worldmap.NumVehiclesInTile(currentTile) <= 1)
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
            fIsDoor = g_worldmap.IsDoor(currentTile.x, currentTile.y - 1) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(currentTile.x, currentTile.y - 1) == iBoundary);
        else if (iDirection == 1)
            fIsDoor = g_worldmap.IsDoor(currentTile.x, currentTile.y + 1) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(currentTile.x, currentTile.y + 1) == iBoundary);
        else if (iDirection == 2)
            fIsDoor = g_worldmap.IsDoor(currentTile.x - 1, currentTile.y) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(currentTile.x - 1, currentTile.y) == iBoundary);
        else if (iDirection == 3)
            fIsDoor = g_worldmap.IsDoor(currentTile.x + 1, currentTile.y) || (iBoundary != 0 && g_worldmap.GetVehicleBoundary(currentTile.x + 1, currentTile.y) == iBoundary);

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
        if (currentTile == g_worldmap.GetPlayerCurrentTile())
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
        SDL_Rect rDst = {pos.x + iWorldOffsetX, pos.y + iWorldOffsetY, iTileSize, iTileSize};
#ifdef USE_SDL2
        SDL_BlitSurface(rm->spr_worldvehicle[iTileSheet].getSurface(), &srcRects[4], blitdest, &rDst);
#else  // SDL_BlitSurface uses a non-const source rect
        SDL_BlitSurface(rm->spr_worldvehicle[iTileSheet].getSurface(), const_cast<SDL_Rect*>(&srcRects[4]), blitdest, &rDst);
#endif
    } else {
        SDL_Rect rDst = {pos.x + iWorldOffsetX + iPaceOffset, pos.y + iWorldOffsetY, iTileSize, iTileSize};
#ifdef USE_SDL2
        SDL_BlitSurface(rm->spr_worldvehicle[iTileSheet].getSurface(), &srcRects[iDrawDirection + iAnimationFrame], blitdest, &rDst);
#else  // SDL_BlitSurface uses a non-const source rect
        SDL_BlitSurface(rm->spr_worldvehicle[iTileSheet].getSurface(), const_cast<SDL_Rect*>(&srcRects[iDrawDirection + iAnimationFrame]), blitdest, &rDst);
#endif
    }
}


/**********************************
* WorldWarp
**********************************/

WorldWarp::WorldWarp(short id, Vec2s posA, Vec2s posB)
    : id(id)
    , posA(posA)
    , posB(posB)
{}

Vec2s WorldWarp::getOtherSide(Vec2s target) const
{
    if (target == posA) {
        return posB;
    }
    if (target == posB) {
        return posA;
    }
    return target;
}


/**********************************
* WorldMap
**********************************/

WorldMap::WorldMap(short w, short h)
    : iWidth(w)
    , iHeight(h)
    , tiles(w, h)
{
    ResetTourStops();  // FIXME
}

WorldMap::WorldMap(const std::string& path, short tilesize)
{
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

    worldName = stripPathAndExtension(path);

    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Could not open the world file");

    std::string line;
    short iReadType = 0;
    Version version;
    short iMapTileReadRow = 0;
    short iCurrentStage = 0;
    short iNumWarps = 0;
    short iNumVehicles = 0;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        if (line[0] == '#' || line[0] == '\r' || line[0] == ' ' || line[0] == '\t')
            continue;

        if (iReadType == 0) { //Read version number
            std::list<std::string_view> tokens = tokenize(line, '.');
            version.major = popNextInt(tokens);
            version.minor = popNextInt(tokens);
            version.patch = popNextInt(tokens);
            version.build = popNextInt(tokens);
            iReadType = 1;
        } else if (iReadType == 1) { //music category
            iMusicCategory = std::stoi(line);
            iReadType = 2;
        } else if (iReadType == 2) { //world width
            iWidth = std::stoi(line);
            iReadType = 3;
        } else if (iReadType == 3) { //world height
            iHeight = std::stoi(line);
            iReadType = 4;

            tiles = decltype(tiles)(iWidth, iHeight);

            short iDrawSurfaceTiles = iWidth * iHeight;

            if (iDrawSurfaceTiles > 456)
                iDrawSurfaceTiles = 456; //19 * 24 = 456 max tiles in world surface

            iTilesPerCycle = iDrawSurfaceTiles / 8;
        } else if (iReadType == 4) { //background water
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < iWidth)
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iBackgroundWater = popNextInt(tokens);
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 5;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 5) { //background sprites
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < iWidth)
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iBackgroundSprite = popNextInt(tokens);
                tile.fAnimated = (tile.iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE) != 1;

                tile.iID = iMapTileReadRow * iWidth + iMapTileReadCol;
                tile.iCol = iMapTileReadCol;
                tile.iRow = iMapTileReadRow;
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 6;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 6) { //foreground sprites
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < iWidth)
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iForegroundSprite = popNextInt(tokens);

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
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 7;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 7) { //path connections
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < iWidth)
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iConnectionType = popNextInt(tokens);
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
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < iWidth)
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iType = popNextInt(tokens);
                tile.iWarp = -1;

                if (tile.iType == 1) {
                    iStartX = iMapTileReadCol;
                    iStartY = iMapTileReadRow;
                    player.SetPosition(iStartX, iStartY);
                }

                tile.iCompleted = tile.iType <= 5 ? -1 : -2;
            }

            if (++iMapTileReadRow == iHeight) {
                iReadType = 9;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 9) { //vehicle boundaries
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < iWidth)
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++) {
                WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iVehicleBoundary = popNextInt(tokens);
            }

            if (++iMapTileReadRow == iHeight)
                iReadType = 10;
        } else if (iReadType == 10) { //number of stages
            iNumStages = std::stoi(line);

            iReadType = iNumStages == 0 ? 12 : 11;
        } else if (iReadType == 11) { //stage details
            TourStop* ts = new TourStop();
            char* buffer = new char[line.size() + 1];
            std::copy(line.begin(), line.end(), buffer);
            buffer[line.size()] = '\0';
            *ts = ParseTourStopLine(buffer, version, true);
            delete[] buffer;

            game_values.tourstops.push_back(ts);

            if (++iCurrentStage >= iNumStages) {
                //Scan stage IDs and make sure we have a stage for each one
                short iMaxStage = game_values.tourstops.size() + 5;
                for (short iRow = 0; iRow < iHeight; iRow++) {
                    for (short iCol = 0; iCol < iWidth; iCol++) {
                        short iType = tiles.at(iCol, iRow).iType;
                        if (iType < 0 || iType > iMaxStage)
                            goto RETURN;
                    }
                }

                iReadType = 12;
            }
        } else if (iReadType == 12) { //number of warps
            iNumWarps = std::stoi(line);

            if (iNumWarps < 0)
                iNumWarps = 0;

            if (iNumWarps > 0)
                warps.reserve(iNumWarps);

            iReadType = iNumWarps == 0 ? 14 : 13;
        } else if (iReadType == 13) { //warp details
            std::list<std::string_view> tokens = tokenize(line, ',');

            short iCol1 = std::max(0, popNextInt(tokens));
            short iRow1 = std::max(0, popNextInt(tokens));
            short iCol2 = std::max(0, popNextInt(tokens));
            short iRow2 = std::max(0, popNextInt(tokens));

            short warpId = warps.size();
            warps.emplace_back(WorldWarp(warpId, {iCol1, iRow1}, {iCol2, iRow2}));

            tiles.at(iCol1, iRow1).iWarp = warpId;
            tiles.at(iCol2, iRow2).iWarp = warpId;

            if (warps.size() >= iNumWarps)
                iReadType = 14;
        } else if (iReadType == 14) { //number of vehicles
            iNumVehicles = std::stoi(line);

            if (iNumVehicles < 0)
                iNumVehicles = 0;

            if (iNumVehicles > 0)
                vehicles.reserve(iNumVehicles);

            iReadType = iNumVehicles == 0 ? 16 : 15;
        } else if (iReadType == 15) { //vehicles
            std::list<std::string_view> tokens = tokenize(line, ',');

            short iSprite = popNextInt(tokens);

            short iStage = popNextInt(tokens);
            if (iStage > iNumStages)
                iStage = 0;

            short iCol = popNextInt(tokens);
            short iRow = popNextInt(tokens);

            short iMinMoves = std::max(0, popNextInt(tokens));
            short iMaxMoves = std::max<short>(iMinMoves, popNextInt(tokens));

            bool fSpritePaces = popNextInt(tokens) == 1;

            short iInitialDirection = popNextInt(tokens);
            if (iInitialDirection != 0)
                iInitialDirection = 1;

            short iBoundary = popNextInt(tokens);

            vehicles.emplace_back(WorldVehicle());
            vehicles.back().Init(iCol, iRow, iStage, iSprite, iMinMoves, iMaxMoves, fSpritePaces, iInitialDirection, iBoundary, iTileSize);

            if (vehicles.size() >= iNumVehicles)
                iReadType = 16;
        } else if (iReadType == 16) { //initial bonus items
            std::list<std::string_view> tokens = tokenize(line, ',');

            m_initialBonuses.reserve(32);

            while (!tokens.empty()) {
                std::string_view token = popNext(tokens);
                if (token.empty())
                    break;

                //0 indicates no initial bonuses
                if (token[0] == '0')
                    break;

                short iBonusOffset = 0;
                if (token[0] == 'w' || token[0] == 'W')
                    iBonusOffset += NUM_POWERUPS;

                short iBonus = toInt(token.substr(1)) + iBonusOffset;
                if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS)
                    iBonus = 0;

                if (m_initialBonuses.size() < 32)
                    m_initialBonuses.emplace_back(iBonus);
                else
                    m_initialBonuses.back() = iBonus;
            }

            iReadType = 17;
        }
    }

RETURN:
    if (iReadType != 17)
        throw std::runtime_error("Invalid world file");

    ResetTourStops();  // FIXME
}

void WorldMap::SetTileConnections(short iCol, short iRow)
{
    if (iCol < 0 || iRow < 0 || iCol >= iWidth || iRow >= iHeight)
        return;

    WorldMapTile& tile = tiles.at(iCol, iRow);

    for (short iDirection = 0; iDirection < 4; iDirection++)
        tile.fConnection[iDirection] = false;

    if (iRow > 0) {
        const WorldMapTile& topTile = tiles.at(iCol, iRow - 1);

        tile.fConnection[0] = (topTile.iConnectionType == 1 || topTile.iConnectionType == 5 || topTile.iConnectionType == 6 ||
                                topTile.iConnectionType == 7 || topTile.iConnectionType == 9 || topTile.iConnectionType == 10 ||
                                topTile.iConnectionType == 11 || topTile.iConnectionType == 15) && (tile.iConnectionType == 1 ||
                                        tile.iConnectionType == 3 || tile.iConnectionType == 4 || tile.iConnectionType == 7 ||
                                        tile.iConnectionType == 8 || tile.iConnectionType == 9 || tile.iConnectionType == 11 ||
                                        tile.iConnectionType == 15);
    }

    if (iRow < iHeight - 1) {
        const WorldMapTile& bottomTile = tiles.at(iCol, iRow + 1);

        tile.fConnection[1] = (bottomTile.iConnectionType == 1 || bottomTile.iConnectionType == 3 || bottomTile.iConnectionType == 4 ||
                                bottomTile.iConnectionType == 7 || bottomTile.iConnectionType == 8 || bottomTile.iConnectionType == 9 ||
                                bottomTile.iConnectionType == 11 || bottomTile.iConnectionType == 15) && (tile.iConnectionType == 1 ||
                                        tile.iConnectionType == 5 || tile.iConnectionType == 6 || tile.iConnectionType == 7 ||
                                        tile.iConnectionType == 9 || tile.iConnectionType == 10 || tile.iConnectionType == 11 ||
                                        tile.iConnectionType == 15);
    }

    if (iCol > 0) {
        const WorldMapTile& leftTile = tiles.at(iCol - 1, iRow);

        tile.fConnection[2] = (leftTile.iConnectionType == 2 || leftTile.iConnectionType == 4 || leftTile.iConnectionType == 5 ||
                                leftTile.iConnectionType == 8 || leftTile.iConnectionType == 9 || leftTile.iConnectionType == 10 ||
                                leftTile.iConnectionType == 11 || leftTile.iConnectionType == 13) && (tile.iConnectionType == 2 || tile.iConnectionType == 3 ||
                                        tile.iConnectionType == 6 || tile.iConnectionType == 7 || tile.iConnectionType == 8 ||
                                        tile.iConnectionType == 10 || tile.iConnectionType == 11 || tile.iConnectionType == 13);
    }

    if (iCol < iWidth - 1) {
        const WorldMapTile& rightTile = tiles.at(iCol + 1, iRow);

        tile.fConnection[3] = (rightTile.iConnectionType == 2 || rightTile.iConnectionType == 3 || rightTile.iConnectionType == 6 ||
                                rightTile.iConnectionType == 7 || rightTile.iConnectionType == 8 || rightTile.iConnectionType == 10 ||
                                rightTile.iConnectionType == 11 || rightTile.iConnectionType == 13) && (tile.iConnectionType == 2 || tile.iConnectionType == 4 ||
                                        tile.iConnectionType == 5 || tile.iConnectionType == 8 || tile.iConnectionType == 9 ||
                                        tile.iConnectionType == 10 || tile.iConnectionType == 11 || tile.iConnectionType == 13);
    }
}

//Saves world to file
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
            const WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
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
            const WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
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
            const WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
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
            const WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
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
            const WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
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
            const WorldMapTile& tile = tiles.at(iMapTileReadCol, iMapTileReadRow);
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

    fprintf(file, "%d\n", game_values.tourstops.size());

    for (short iStage = 0; iStage < game_values.tourstops.size(); iStage++) {
        std::string line = WriteTourStopLine(*game_values.tourstops[iStage], true);
        fprintf(file, "%s", line.c_str());
    }
    fprintf(file, "\n");

    fprintf(file, "#Warps\n");
    fprintf(file, "#location 1 x, y, location 2 x, y\n");

    fprintf(file, "%d\n", warps.size());

    for (const WorldWarp& warp : warps) {
        fprintf(file, "%d,", warp.posA.x);
        fprintf(file, "%d,", warp.posA.y);
        fprintf(file, "%d,", warp.posB.x);
        fprintf(file, "%d\n", warp.posB.y);
    }
    fprintf(file, "\n");

    fprintf(file, "#Vehicles\n");
    fprintf(file, "#Sprite,Stage Type, Start Column, Start Row, Min Moves, Max Moves, Sprite Paces, Sprite Direction, Boundary\n");

    fprintf(file, "%d\n", vehicles.size());

    for (short iVehicle = 0; iVehicle < vehicles.size(); iVehicle++) {
        fprintf(file, "%d,", vehicles[iVehicle].iDrawSprite);
        fprintf(file, "%d,", vehicles[iVehicle].iActionId);
        fprintf(file, "%d,", vehicles[iVehicle].currentTile.x);
        fprintf(file, "%d,", vehicles[iVehicle].currentTile.y);
        fprintf(file, "%d,", vehicles[iVehicle].iMinMoves);
        fprintf(file, "%d,", vehicles[iVehicle].iMaxMoves);
        fprintf(file, "%d,", vehicles[iVehicle].fSpritePaces);
        fprintf(file, "%d,", vehicles[iVehicle].iDrawDirection);
        fprintf(file, "%d\n", vehicles[iVehicle].iBoundary);
    }
    fprintf(file, "\n");

    fprintf(file, "#Initial Items\n");

    for (short iItem = 0; iItem < m_initialBonuses.size(); iItem++) {
        if (iItem != 0)
            fprintf(file, ",");

        short iBonus = m_initialBonuses[iItem];
        char cBonusType = 'p';
        if (iBonus >= NUM_POWERUPS) {
            iBonus -= NUM_POWERUPS;
            cBonusType = 'w';
        }

        fprintf(file, "%c%d", cBonusType, iBonus);
    }

    if (m_initialBonuses.empty())
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
    for (size_t row = 0; row < tiles.rows(); row++) {
        for (size_t col = 0; col < tiles.cols(); col++) {
            WorldMapTile& tile = tiles.at(col, row);
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

//Resizes world keeping intact current tiles (if possible)
void WorldMap::Resize(short w, short h)
{
    short iOldWidth = iWidth;
    short iOldHeight = iHeight;

    //Create new map
    iWidth = w;
    iHeight = h;
    Grid<WorldMapTile> newTiles(w, h);

    //Copy tiles to new map
    for (short iCol = 0; iCol < iWidth; iCol++) {
        for (short iRow = 0; iRow < iHeight; iRow++) {
            WorldMapTile& newTile = newTiles.at(iCol, iRow);
            if (iCol < iOldWidth && iRow < iOldHeight) {
                newTile = std::move(tiles.at(iCol, iRow));
            } else {
                newTile.iBackgroundSprite = 0;
                newTile.iBackgroundWater = 0;
                newTile.iForegroundSprite = 0;
                newTile.iConnectionType = 0;
                newTile.iType = 0;
                newTile.iID = iRow * iWidth + iCol;
                newTile.iVehicleBoundary = 0;
                newTile.iWarp = 0;
            }
        }
    }

    // Apply the new tiles
    tiles = std::move(newTiles);

    // Clear warp ends outside the world
    for (WorldWarp& warp : warps) {
        if (iWidth <= warp.posA.x || iHeight <= warp.posA.y)
            warp.posA = WorldWarp::NULL_POS;
        if (iWidth <= warp.posB.x || iHeight <= warp.posB.y)
            warp.posB = WorldWarp::NULL_POS;
    }

    // Remove warps fully outside the world
    {
        const auto eraseFrom = std::remove_if(
            warps.begin(),
            warps.end(),
            [](const WorldWarp& warp) { return warp.posA == WorldWarp::NULL_POS && warp.posB == WorldWarp::NULL_POS; });
        warps.erase(eraseFrom, warps.end());
    }
    // Remove vehicles fully outside the world
    {
        const Vec2s worldDim(iWidth, iHeight);
        const auto eraseFrom = std::remove_if(
            vehicles.begin(),
            vehicles.end(),
            [worldDim](const WorldVehicle& vehicle) { return vehicle.getCurrentTile().x >= worldDim.x || vehicle.getCurrentTile().y >= worldDim.y; });
        vehicles.erase(eraseFrom, vehicles.end());
    }
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
    const WorldMapTile& tile = tiles.at(iCol + iMapDrawOffsetCol, iRow + iMapDrawOffsetRow);

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

void WorldMap::SetPlayerSprite(short iPlayerSprite)
{
    player.SetSprite(iPlayerSprite);
}

void WorldMap::SetPlayerPosition(short iPlayerCol, short iPlayerRow)
{
    player.SetPosition(iPlayerCol, iPlayerRow);
}

void WorldMap::MovePlayer(short iDirection)
{
    player.Move(iDirection);
}

void WorldMap::FacePlayer(short iDirection)
{
    player.FaceDirection(iDirection);
}

bool WorldMap::IsVehicleMoving() const
{
    return std::any_of(vehicles.cbegin(), vehicles.cend(),
        [](const WorldVehicle& vehicle) {
            return vehicle.fEnabled && vehicle.iState > 0;
        });
}

short WorldMap::GetVehicleInPlayerTile(short * vehicleIndex) const
{
    for (size_t i = 0; i < vehicles.size(); i++) {
        const WorldVehicle& vehicle = vehicles[i];

        if (!vehicle.fEnabled)
            continue;

        if (vehicle.currentTile == player.currentTile) {
            *vehicleIndex = i;
            return vehicle.iActionId;
        }
    }

    *vehicleIndex = -1;
    return -1;
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

size_t WorldMap::NumVehiclesInTile(Vec2s iTile) const
{
    return std::count_if(vehicles.cbegin(), vehicles.cend(),
        [iTile](const WorldVehicle& vehicle) {
            return vehicle.fEnabled && vehicle.currentTile == iTile;
        });
}

short WorldMap::GetVehicleStageScore(short iVehicleIndex) const
{
    return game_values.tourstops[vehicles[iVehicleIndex].iActionId]->iPoints;
}

bool WorldMap::GetWarpInPlayerTile(short * iWarpCol, short * iWarpRow) const
{
    short iWarp = tiles.at(player.currentTile.x, player.currentTile.y).iWarp;
    if (iWarp < 0)
        return false;

    Vec2s pos = warps[iWarp].getOtherSide(player.currentTile);
    *iWarpCol = pos.x;
    *iWarpRow = pos.y;
    return true;
}

void WorldMap::MoveBridges()
{
    for (short iRow = 0; iRow < iHeight; iRow++) {
        for (short iCol = 0; iCol < iWidth; iCol++) {
            if (tiles.at(iCol, iRow).iConnectionType == 12) {
                tiles.at(iCol, iRow).iConnectionType = 13;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol - 1, iRow);
                SetTileConnections(iCol + 1, iRow);
            } else if (tiles.at(iCol, iRow).iConnectionType == 13) {
                tiles.at(iCol, iRow).iConnectionType = 12;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol - 1, iRow);
                SetTileConnections(iCol + 1, iRow);
            } else if (tiles.at(iCol, iRow).iConnectionType == 14) {
                tiles.at(iCol, iRow).iConnectionType = 15;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol, iRow - 1);
                SetTileConnections(iCol, iRow + 1);
            } else if (tiles.at(iCol, iRow).iConnectionType == 15) {
                tiles.at(iCol, iRow).iConnectionType = 14;
                SetTileConnections(iCol, iRow);
                SetTileConnections(iCol, iRow - 1);
                SetTileConnections(iCol, iRow + 1);
            }

            if (tiles.at(iCol, iRow).iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET)
                tiles.at(iCol, iRow).iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET + 1;
            else if (tiles.at(iCol, iRow).iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET + 1)
                tiles.at(iCol, iRow).iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET;
            else if (tiles.at(iCol, iRow).iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET + 2)
                tiles.at(iCol, iRow).iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET + 3;
            else if (tiles.at(iCol, iRow).iForegroundSprite == WORLD_BRIDGE_SPRITE_OFFSET + 3)
                tiles.at(iCol, iRow).iForegroundSprite = WORLD_BRIDGE_SPRITE_OFFSET + 2;
        }
    }
}

void WorldMap::IsTouchingDoor(short iCol, short iRow, bool doors[4]) const
{
    const WorldMapTile& tile = tiles.at(iCol, iRow);

    if (iCol > 0) {
        if (tile.iCompleted >= -1) {
            short iType = tiles.at(iCol - 1, iRow).iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iCol < iWidth - 1) {
        if (tile.iCompleted >= -1) {
            short iType = tiles.at(iCol + 1, iRow).iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iRow > 0) {
        if (tile.iCompleted >= -1) {
            short iType = tiles.at(iCol, iRow - 1).iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }

    if (iCol < iHeight - 1) {
        if (tile.iCompleted >= -1) {
            short iType = tiles.at(iCol, iRow + 1).iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }
}

bool WorldMap::IsDoor(short iCol, short iRow) const
{
    if (iCol >= 0 && iRow >= 0 && iCol < iWidth && iRow < iHeight) {
        short iType = tiles.at(iCol, iRow).iType;
        if (iType >= 2 && iType <= 5)
            return true;
    }

    return false;
}

short WorldMap::UseKey(short iKeyType, short iCol, short iRow, bool fCloud)
{
    short iDoorsOpened = 0;

    const WorldMapTile& tile = tiles.at(iCol, iRow);

    if (iCol > 0) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles.at(iCol - 1, iRow).iType - 2 == iKeyType) {
            tiles.at(iCol - 1, iRow).iType = 0;
            iDoorsOpened |= 1;
        }
    }

    if (iCol < iWidth - 1) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles.at(iCol + 1, iRow).iType - 2 == iKeyType) {
            tiles.at(iCol + 1, iRow).iType = 0;
            iDoorsOpened |= 2;
        }
    }

    if (iRow > 0) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles.at(iCol, iRow - 1).iType - 2 == iKeyType) {
            tiles.at(iCol, iRow - 1).iType = 0;
            iDoorsOpened |= 4;
        }
    }

    if (iRow < iHeight - 1) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles.at(iCol, iRow + 1).iType - 2 == iKeyType) {
            tiles.at(iCol, iRow + 1).iType = 0;
            iDoorsOpened |= 8;
        }
    }

    return iDoorsOpened;
}

short WorldMap::GetVehicleBoundary(short iCol, short iRow) const
{
    if (iCol >= 0 && iRow >= 0 && iCol < iWidth && iRow < iHeight) {
        return tiles.at(iCol, iRow).iVehicleBoundary;
    }

    return 0;
}

//Implements breadth first search to find a stage or vehicle of interest
short WorldMap::GetNextInterestingMove(short iCol, short iRow) const
{
    const WorldMapTile& currentTile = tiles.at(iCol, iRow);

    //Look for stages or vehicles, but not bonus houses
    if ((currentTile.iType >= 6 && currentTile.iCompleted == -2) || NumVehiclesInTile({iCol, iRow}) > 0)
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
        if ((tile->iType >= 6 && tile->iCompleted == -2) || NumVehiclesInTile({tile->iCol, tile->iRow}) > 0) {
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
                    const Vec2s target(iBackTileId % iWidth, iBackTileId / iWidth);
                    const Vec2s pos = warps[tiles.at(iCol, iRow).iWarp].getOtherSide(target);
                    iBackTileId = tiles.at(pos.x, pos.y).iID;
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
                    const WorldMapTile& topTile = tiles.at(tile->iCol, tile->iRow - 1);

                    //Stop at door tiles
                    if (topTile.iType >= 2 && topTile.iType <= 5)
                        continue;

                    if (visitedTiles.find(topTile.iID) == visitedTiles.end()) {
                        visitedTiles[topTile.iID] = 1;
                        next.push(&topTile);
                    }
                } else if (iNeighbor == 1 && tile->iRow < iHeight - 1) {
                    const WorldMapTile& bottomTile = tiles.at(tile->iCol, tile->iRow + 1);

                    //Stop at door tiles
                    if (bottomTile.iType >= 2 && bottomTile.iType <= 5)
                        continue;

                    if (visitedTiles.find(bottomTile.iID) == visitedTiles.end()) {
                        visitedTiles[bottomTile.iID] = 0;
                        next.push(&bottomTile);
                    }
                } else if (iNeighbor == 2 && tile->iCol > 0) {
                    const WorldMapTile& leftTile = tiles.at(tile->iCol - 1, tile->iRow);

                    //Stop at door tiles
                    if (leftTile.iType >= 2 && leftTile.iType <= 5)
                        continue;

                    if (visitedTiles.find(leftTile.iID) == visitedTiles.end()) {
                        visitedTiles[leftTile.iID] = 3;
                        next.push(&leftTile);
                    }
                } else if (iNeighbor == 3 && tile->iCol < iWidth - 1) {
                    const WorldMapTile& rightTile = tiles.at(tile->iCol + 1, tile->iRow);

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
                const Vec2s pos = warps[tile->iWarp].getOtherSide({tile->iCol, tile->iRow});
                const WorldMapTile& warpTile = tiles.at(pos.x, pos.y);

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
        game_values.worldpowerupcount[iTeam] = m_initialBonuses.size();

        for (short iItem = 0; iItem < m_initialBonuses.size(); iItem++)
            game_values.worldpowerups[iTeam][iItem] = m_initialBonuses[iItem];  // TODO
    }
}
