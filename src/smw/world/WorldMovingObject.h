#pragma once

#include "math/Vec2.h"

class WorldMovingObject {
public:
    WorldMovingObject();
    virtual ~WorldMovingObject() = default;

    void Init(short iCol, short iRow, short iSprite, short iInitialDirection, short tilesize);

    virtual void Move(short iDirection);
    virtual bool Update();

    void FaceDirection(short iDirection);
    void SetPosition(short iCol, short iRow);

protected:
    Vec2s pos;
    Vec2s currentTile;
    Vec2s destTile;

    short iState = 0;
    short iDrawSprite = 0;
    short iDrawDirection = 0;
    short iAnimationFrame = 0;
    short iAnimationTimer = 0;

    short iTileSize = 0;
    short iTileSheet = 0;

    friend class WorldMap;
    friend void takescreenshot();
};
