#include "WorldMovingObject.h"

#include "GlobalConstants.h"

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
