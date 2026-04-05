#include "TileTypes.h"

#include <algorithm>
#include <array>


namespace {
constexpr std::array<TileType, 19> ordered_tile_types {
    TileType::NonSolid,
    TileType::Solid,
    TileType::SolidOnTop,
    TileType::Ice,
    TileType::Death,
    TileType::DeathOnTop,
    TileType::DeathOnBottom,
    TileType::DeathOnLeft,
    TileType::DeathOnRight,
    TileType::IceOnTop,
    TileType::IceDeathOnBottom,
    TileType::IceDeathOnLleft,
    TileType::IceDeathOnRight,
    TileType::SuperDeath,
    TileType::SuperDeathTop,
    TileType::SuperDeathBottom,
    TileType::SuperDeathLeft,
    TileType::SuperDeathRight,
    TileType::PlayerDeath,
};
} // namespace


TileType NextTileType(TileType type)
{
    auto it = std::find(ordered_tile_types.cbegin(), ordered_tile_types.cend(), type);
    if (it == ordered_tile_types.cend())
        return TileType::NonSolid;

    ++it;
    return it == ordered_tile_types.cend() ? ordered_tile_types.front() : *it;
}


TileType PrevTileType(TileType type)
{
    auto it = std::find(ordered_tile_types.cbegin(), ordered_tile_types.cend(), type);
    if (it == ordered_tile_types.cend())
        return TileType::NonSolid;

    return it == ordered_tile_types.cbegin() ? ordered_tile_types.back() : *(--it);
}
