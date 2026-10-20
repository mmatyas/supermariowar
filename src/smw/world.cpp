#include "world.h"

#include "FileList.h"
#include "GameValues.h"
#include "GlobalConstants.h"
#include "WorldTourStop.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

#include <algorithm>
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


/**********************************
* WorldMap
**********************************/

WorldMap::WorldMap(short w, short h)
    : tiles(w, h)
{
    ResetTourStops();  // FIXME
}

void WorldMap::nextMusicCategory()
{
    iMusicCategory++;
}

void WorldMap::SetTileConnections(short iCol, short iRow)
{
    if (iCol < 0 || iRow < 0 || iCol >= getWidth() || iRow >= getHeight())
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

    if (iRow < getHeight() - 1) {
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

    if (iCol < getWidth() - 1) {
        const WorldMapTile& rightTile = tiles.at(iCol + 1, iRow);

        tile.fConnection[3] = (rightTile.iConnectionType == 2 || rightTile.iConnectionType == 3 || rightTile.iConnectionType == 6 ||
                                rightTile.iConnectionType == 7 || rightTile.iConnectionType == 8 || rightTile.iConnectionType == 10 ||
                                rightTile.iConnectionType == 11 || rightTile.iConnectionType == 13) && (tile.iConnectionType == 2 || tile.iConnectionType == 4 ||
                                        tile.iConnectionType == 5 || tile.iConnectionType == 8 || tile.iConnectionType == 9 ||
                                        tile.iConnectionType == 10 || tile.iConnectionType == 11 || tile.iConnectionType == 13);
    }
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
            tile.iID = row * getWidth() + col;
            tile.iVehicleBoundary = 0;
            tile.iWarp = 0;
        }
    }

    vehicles.clear();
    warps.clear();
}

//Sets the tile size the world is drawn with (game, preview and thumbnails use different sizes)
void WorldMap::setTileSize(short tilesize)
{
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
}

//Resizes world keeping intact current tiles (if possible)
void WorldMap::Resize(short w, short h)
{
    const short iOldWidth = static_cast<short>(tiles.cols());
    const short iOldHeight = static_cast<short>(tiles.rows());

    //Create new map
    Grid<WorldMapTile> newTiles(w, h);

    //Copy tiles to new map
    for (short iCol = 0; iCol < getWidth(); iCol++) {
        for (short iRow = 0; iRow < getHeight(); iRow++) {
            WorldMapTile& newTile = newTiles.at(iCol, iRow);
            if (iCol < iOldWidth && iRow < iOldHeight) {
                newTile = std::move(tiles.at(iCol, iRow));
            } else {
                newTile.iBackgroundSprite = 0;
                newTile.iBackgroundWater = 0;
                newTile.iForegroundSprite = 0;
                newTile.iConnectionType = 0;
                newTile.iType = 0;
                newTile.iID = iRow * getWidth() + iCol;
                newTile.iVehicleBoundary = 0;
                newTile.iWarp = 0;
            }
        }
    }

    // Apply the new tiles
    tiles = std::move(newTiles);

    // The incremental draw refreshes part of the world surface per frame
    // 19 * 24 = 456 max tiles in world surface
    short iDrawSurfaceTiles = std::min(getWidth() * getHeight(), 456);
    iTilesPerCycle = iDrawSurfaceTiles / 8;
}

bool WorldMap::Update(bool * fPlayerVehicleCollision)
{
    bool fPlayMovingVehicleSound = false;

    bool fPlayerDoneMove = player.Update();

    *fPlayerVehicleCollision = false;
    for (WorldVehicle& vehicle : vehicles) {
        if (!vehicle.isEnabled())
            continue;

        *fPlayerVehicleCollision |= vehicle.Update();

        if (vehicle.getState() > 0)
            fPlayMovingVehicleSound = true;
    }

    if (fPlayMovingVehicleSound && !rm->sfx_boomerang.isPlaying())
        ifSoundOnPlay(rm->sfx_boomerang);

    return fPlayerDoneMove;
}

void WorldMap::Draw(short iMapOffsetX, short iMapOffsetY, bool fDrawPlayer, bool fVehiclesSleeping) const
{
    for (const WorldVehicle& vehicle : vehicles) {
        if (!vehicle.isEnabled())
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
    for (short iRow = 0; iRow < getHeight(); iRow++) {
        for (short iCol = 0; iCol < getWidth(); iCol++) {
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
    short iRowEnd = 19 + iMapDrawOffsetRow < getHeight() ? 19 : getHeight() - iMapDrawOffsetRow;
    short iColEnd = 24 + iMapDrawOffsetCol < getWidth() ? 24 : getWidth() - iMapDrawOffsetCol;

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
            rm->spr_worldbackground[iTileSheet].draw(rSrc, surface, r);
        } else {
            SDL_Rect rSrc = {iAnimationFrame + (iBackgroundWater << (2 + iTileSizeShift)), 0, iTileSize, iTileSize};
            rm->spr_worldbackground[iTileSheet].draw(rSrc, surface, r);

            if ((iBackgroundSprite >= 2 && iBackgroundSprite <= 48)) {
                if (iBackgroundSprite >= 45) {
                    rSrc = {(3 << iTileSizeShift) + iBackgroundStyleOffset, (iBackgroundSprite - 44) << iTileSizeShift, iTileSize, iTileSize};
                    rm->spr_worldbackground[iTileSheet].draw(rSrc, surface, r);
                } else if (iBackgroundSprite >= 30) {
                    rSrc = {(2 << iTileSizeShift) + iBackgroundStyleOffset, (iBackgroundSprite - 29) << iTileSizeShift, iTileSize, iTileSize};
                    rm->spr_worldbackground[iTileSheet].draw(rSrc, surface, r);
                } else if (iBackgroundSprite >= 16) {
                    rSrc = {iTileSize + iBackgroundStyleOffset, (iBackgroundSprite - 14) << iTileSizeShift, iTileSize, iTileSize};
                    rm->spr_worldbackground[iTileSheet].draw(rSrc, surface, r);
                } else {
                    rSrc = {iBackgroundStyleOffset, iBackgroundSprite << iTileSizeShift, iTileSize, iTileSize};
                    rm->spr_worldbackground[iTileSheet].draw(rSrc, surface, r);
                }
            }
        }
    }

    if (iLayer != 1) {
        if (tile.iCompleted >= 0) {
            SDL_Rect rSrc = {(tile.iCompleted + 10) << iTileSizeShift, 5 << iTileSizeShift, iTileSize, iTileSize};
            rm->spr_worldforegroundspecial[iTileSheet].draw(rSrc, surface, r);
        } else {
            if (iForegroundSprite >= 0 && iForegroundSprite < WORLD_FOREGROUND_STAGE_OFFSET) {
                short iPathStyle = iForegroundSprite / WORLD_PATH_SPRITE_SET_SIZE;
                short iPathOffsetX = (iPathStyle % 4) * (5 << iTileSizeShift);
                short iPathOffsetY = (iPathStyle >> 2) * (10 << iTileSizeShift);
                iForegroundSprite %= WORLD_PATH_SPRITE_SET_SIZE;

                if (iForegroundSprite == 1 || iForegroundSprite == 2) { //Non-animated straight paths
                    SDL_Rect rSrc = {iPathOffsetX, ((iForegroundSprite - 1) << iTileSizeShift) + iPathOffsetY, iTileSize, iTileSize};
                    rm->spr_worldpaths[iTileSheet].draw(rSrc, surface, r);
                } else if (iForegroundSprite >= 3 && iForegroundSprite <= 10) { //Animated paths with "coins" in them
                    SDL_Rect rSrc = {iPathOffsetX + iAnimationFrame, ((iForegroundSprite - 1) << iTileSizeShift) + iPathOffsetY, iTileSize, iTileSize};
                    rm->spr_worldpaths[iTileSheet].draw(rSrc, surface, r);
                } else if (iForegroundSprite >= 11 && iForegroundSprite <= 18) { //Non-animated straight paths over water
                    short iSpriteX = (((iForegroundSprite - 11) / 2) + 1) << iTileSizeShift;
                    short iSpriteY = ((iForegroundSprite - 11) % 2) << iTileSizeShift;

                    SDL_Rect rSrc = {iPathOffsetX + iSpriteX, iSpriteY + iPathOffsetY, iTileSize, iTileSize};
                    rm->spr_worldpaths[iTileSheet].draw(rSrc, surface, r);
                }
            } else if (iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399) {
                short iTileColor = (iForegroundSprite - WORLD_FOREGROUND_STAGE_OFFSET) / 100;
                SDL_Rect rSrc = {(10 << iTileSizeShift) + iAnimationFrame, iTileColor << iTileSizeShift, iTileSize, iTileSize};
                rm->spr_worldforegroundspecial[iTileSheet].draw(rSrc, surface, r);

                short iTileNumber = (iForegroundSprite - WORLD_FOREGROUND_STAGE_OFFSET) % 100;
                rSrc.x = (iTileNumber % 10) << iTileSizeShift;
                rSrc.y = (iTileNumber / 10) << iTileSizeShift;
                rm->spr_worldforegroundspecial[iTileSheet].draw(rSrc, surface, r);
            } else if (iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) {
                SDL_Rect rSrc = {(iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 10) << iTileSizeShift, 7 << iTileSizeShift, iTileSize, iTileSize};
                rm->spr_worldforegroundspecial[iTileSheet].draw(rSrc, surface, r);
            } else if (iForegroundSprite >= WORLD_START_SPRITE_OFFSET && iForegroundSprite <= WORLD_START_SPRITE_OFFSET + 1) {
                SDL_Rect rSrc = {(iForegroundSprite - WORLD_START_SPRITE_OFFSET + 10) << iTileSizeShift, 4 << iTileSizeShift, iTileSize, iTileSize};
                rm->spr_worldforegroundspecial[iTileSheet].draw(rSrc, surface, r);
            } else if (iForegroundSprite >= WORLD_FOREGROUND_SPRITE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_OFFSET + 179) {
                short iSprite = iForegroundSprite - WORLD_FOREGROUND_SPRITE_OFFSET;
                SDL_Rect rSrc = {(iSprite % 12) << iTileSizeShift, (iSprite / 12) << iTileSizeShift, iTileSize, iTileSize};
                rm->spr_worldforeground[iTileSheet].draw(rSrc, surface, r);
            } else if (iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29) {
                short iSprite = iForegroundSprite - WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET;
                SDL_Rect rSrc = {(iSprite >= 15 ? (16 << iTileSizeShift) : (12 << iTileSizeShift)) + iAnimationFrame, (iSprite % 15) << iTileSizeShift, iTileSize, iTileSize};
                rm->spr_worldforeground[iTileSheet].draw(rSrc, surface, r);
            }
        }

        //Draw doors
        short iType = tile.iType;
        if (iType >= 2 && iType <= 5) {
            SDL_Rect rSrc = {(iType + 8) << iTileSizeShift, 6 << iTileSizeShift, iTileSize, iTileSize};
            rm->spr_worldforegroundspecial[iTileSheet].draw(rSrc, surface, r);
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
            return vehicle.isEnabled() && vehicle.getState() > 0;
        });
}

short WorldMap::GetVehicleInPlayerTile(short * vehicleIndex) const
{
    for (size_t i = 0; i < vehicles.size(); i++) {
        const WorldVehicle& vehicle = vehicles[i];

        if (!vehicle.isEnabled())
            continue;

        if (vehicle.getCurrentTile() == player.getCurrentTile()) {
            *vehicleIndex = i;
            return vehicle.getActionId();
        }
    }

    *vehicleIndex = -1;
    return -1;
}

void WorldMap::MoveVehicles()
{
    for (WorldVehicle& vehicle : vehicles) {
        if (vehicle.isEnabled())
            vehicle.Move();
    }
}

void WorldMap::RemoveVehicle(short iVehicleIndex)
{
    vehicles[iVehicleIndex].setEnabled(false);
}

size_t WorldMap::NumVehiclesInTile(Vec2s iTile) const
{
    return std::count_if(vehicles.cbegin(), vehicles.cend(),
        [iTile](const WorldVehicle& vehicle) {
            return vehicle.isEnabled() && vehicle.getCurrentTile() == iTile;
        });
}

short WorldMap::GetVehicleStageScore(short iVehicleIndex) const
{
    return game_values.tourstops[vehicles[iVehicleIndex].getActionId()]->iPoints;
}

bool WorldMap::GetWarpInPlayerTile(short * iWarpCol, short * iWarpRow) const
{
    const Vec2s tile = player.getCurrentTile();
    short iWarp = tiles.at(tile.x, tile.y).iWarp;
    if (iWarp < 0)
        return false;

    Vec2s pos = warps[iWarp].getOtherSide(tile);
    *iWarpCol = pos.x;
    *iWarpRow = pos.y;
    return true;
}

void WorldMap::MoveBridges()
{
    for (short iRow = 0; iRow < getHeight(); iRow++) {
        for (short iCol = 0; iCol < getWidth(); iCol++) {
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

    if (iCol < getWidth() - 1) {
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

    if (iCol < getHeight() - 1) {
        if (tile.iCompleted >= -1) {
            short iType = tiles.at(iCol, iRow + 1).iType - 2;

            if (iType >= 0 && iType <= 3)
                doors[iType] = true;
        }
    }
}

bool WorldMap::IsDoor(short iCol, short iRow) const
{
    if (iCol >= 0 && iRow >= 0 && iCol < getWidth() && iRow < getHeight()) {
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

    if (iCol < getWidth() - 1) {
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

    if (iRow < getHeight() - 1) {
        if ((tile.iCompleted >= -1 || fCloud) && tiles.at(iCol, iRow + 1).iType - 2 == iKeyType) {
            tiles.at(iCol, iRow + 1).iType = 0;
            iDoorsOpened |= 8;
        }
    }

    return iDoorsOpened;
}

short WorldMap::GetVehicleBoundary(short iCol, short iRow) const
{
    if (iCol >= 0 && iRow >= 0 && iCol < getWidth() && iRow < getHeight()) {
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
                    iBackTileId -= getWidth();
                else if (iBackTileDirection == 1)
                    iBackTileId += getWidth();
                else if (iBackTileDirection == 2)
                    iBackTileId -= 1;
                else if (iBackTileDirection == 3)
                    iBackTileId += 1;
                else if (iBackTileDirection == 4) {
                    const Vec2s target(iBackTileId % getWidth(), iBackTileId / getWidth());
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
                } else if (iNeighbor == 1 && tile->iRow < getHeight() - 1) {
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
                } else if (iNeighbor == 3 && tile->iCol < getWidth() - 1) {
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
        game_values.worldpowerupcount[iTeam] = iNumInitialBonuses;

        for (short iItem = 0; iItem < iNumInitialBonuses; iItem++)
            game_values.worldpowerups[iTeam][iItem] = iInitialBonuses[iItem];
    }
}
