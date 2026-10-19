#pragma once

#include "math/Vec2.h"

struct WorldWarp {
    WorldWarp(short id, Vec2s posA, Vec2s posB);

    Vec2s getOtherSide(Vec2s target) const;

    short id;
    Vec2s posA;
    Vec2s posB;
};
