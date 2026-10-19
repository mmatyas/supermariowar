#include "WorldWarp.h"

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
