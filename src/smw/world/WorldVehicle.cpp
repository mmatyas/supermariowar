#include "WorldVehicle.h"

#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "world.h"

extern WorldMap g_worldmap;

extern SDL_Surface* blitdest;

extern CResourceManager* rm;

WorldVehicle::WorldVehicle() :
    WorldMovingObject()
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

    WorldMapTile * tile = &g_worldmap.getTile(currentTile.x, currentTile.y);
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
        rm->spr_worldvehicle[iTileSheet].draw(srcRects[4], blitdest, rDst);
    } else {
        SDL_Rect rDst = {pos.x + iWorldOffsetX + iPaceOffset, pos.y + iWorldOffsetY, iTileSize, iTileSize};
        rm->spr_worldvehicle[iTileSheet].draw(srcRects[iDrawDirection + iAnimationFrame], blitdest, rDst);
    }
}
