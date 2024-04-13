#include "TileTypes.h"

#include <algorithm>
#include <array>


namespace {
constexpr std::array<TileType, 19> ordered_tile_types {
    tile_nonsolid,
    tile_solid,
    tile_solid_on_top,
    tile_ice,
    tile_death,
    tile_death_on_top,
    tile_death_on_bottom,
    tile_death_on_left,
    tile_death_on_right,
    tile_ice_on_top,
    tile_ice_death_on_bottom,
    tile_ice_death_on_left,
    tile_ice_death_on_right,
    tile_super_death,
    tile_super_death_top,
    tile_super_death_bottom,
    tile_super_death_left,
    tile_super_death_right,
    tile_player_death,
};
} // namespace


TileType NextTileType(TileType type)
{
    auto it = std::find(ordered_tile_types.cbegin(), ordered_tile_types.cend(), type);
    if (it == ordered_tile_types.cend())
        return tile_nonsolid;

    ++it;
    return it == ordered_tile_types.cend() ? ordered_tile_types.front() : *it;
}


TileType PrevTileType(TileType type)
{
    auto it = std::find(ordered_tile_types.cbegin(), ordered_tile_types.cend(), type);
    if (it == ordered_tile_types.cend())
        return tile_nonsolid;

    return it == ordered_tile_types.cbegin() ? ordered_tile_types.back() : *(--it);
}
