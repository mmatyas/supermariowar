#pragma once

#include "world/WorldMovingObject.h"

class WorldPlayer : public WorldMovingObject {
public:
    WorldPlayer();
    WorldPlayer(short iCol, short iRow);

    void SetSprite(short iPlayer);
    void Draw(short iWorldOffsetX, short iWorldOffsetY) const;
};
