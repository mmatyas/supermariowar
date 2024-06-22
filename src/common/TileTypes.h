#pragma once

#include <cassert>


enum class TileType : unsigned char {
    NonSolid = 0,
    Solid = 1,
    SolidOnTop = 2,
    Ice = 3,
    Death = 4,
    DeathOnTop = 5,
    DeathOnBottom = 6,
    DeathOnLeft = 7,
    DeathOnRight = 8,
    IceOnTop = 9,
    IceDeathOnBottom = 10,
    IceDeathOnLleft = 11,
    IceDeathOnRight = 12,
    SuperDeath = 13,
    SuperDeathTop = 14,
    SuperDeathBottom = 15,
    SuperDeathLeft = 16,
    SuperDeathRight = 17,
    PlayerDeath = 18,
    Gap = 19,
};


enum TileTypeFlag {
    tile_flag_nonsolid = 0,
    tile_flag_solid = 1,
    tile_flag_solid_on_top = 2,
    tile_flag_ice = 4,
    tile_flag_death_on_top = 8,
    tile_flag_death_on_bottom = 16,
    tile_flag_death_on_left = 32,
    tile_flag_death_on_right = 64,
    tile_flag_gap = 128,
    tile_flag_super_death_top = 256,
    tile_flag_super_death_bottom = 512,
    tile_flag_super_death_left = 1024,
    tile_flag_super_death_right = 2048,
    tile_flag_player_death = 4096,

    tile_flag_has_regular_death = tile_flag_death_on_top|tile_flag_death_on_bottom|tile_flag_death_on_left|tile_flag_death_on_right,
    tile_flag_has_super_death = tile_flag_super_death_top|tile_flag_super_death_bottom|tile_flag_super_death_left|tile_flag_super_death_right,
    tile_flag_has_death = tile_flag_player_death|tile_flag_has_regular_death|tile_flag_has_super_death,

    tile_flag_super_or_player_death_top = tile_flag_player_death|tile_flag_super_death_top,
    tile_flag_super_or_player_death_bottom = tile_flag_player_death|tile_flag_super_death_bottom,
    tile_flag_super_or_player_death_left = tile_flag_player_death|tile_flag_super_death_left,
    tile_flag_super_or_player_death_right = tile_flag_player_death|tile_flag_super_death_right,
    tile_flag_player_or_death_on_bottom = tile_flag_player_death|tile_flag_death_on_bottom
};


constexpr unsigned short tileToFlags(TileType tiletype) {
    switch (tiletype) {
        case TileType::NonSolid: return tile_flag_nonsolid;
        case TileType::Solid: return tile_flag_solid;
        case TileType::SolidOnTop: return tile_flag_solid_on_top;
        case TileType::Ice: return tile_flag_solid | tile_flag_ice;

        case TileType::Death: return tile_flag_solid | tile_flag_has_regular_death;
        case TileType::DeathOnTop: return tile_flag_solid | tile_flag_death_on_top;
        case TileType::DeathOnBottom: return tile_flag_solid | tile_flag_death_on_bottom;
        case TileType::DeathOnLeft: return tile_flag_solid | tile_flag_death_on_left;
        case TileType::DeathOnRight: return tile_flag_solid | tile_flag_death_on_right;

        case TileType::IceOnTop: return tile_flag_ice | tile_flag_solid_on_top;
        case TileType::IceDeathOnBottom: return tile_flag_solid | tile_flag_ice | tile_flag_death_on_bottom;
        case TileType::IceDeathOnLleft: return tile_flag_solid | tile_flag_ice | tile_flag_death_on_left;
        case TileType::IceDeathOnRight: return tile_flag_solid | tile_flag_ice | tile_flag_death_on_right;

        case TileType::SuperDeath: return tile_flag_solid | tile_flag_has_regular_death | tile_flag_has_super_death;
        case TileType::SuperDeathTop: return tile_flag_solid | tile_flag_super_death_top | tile_flag_death_on_top;
        case TileType::SuperDeathBottom: return tile_flag_solid | tile_flag_super_death_bottom | tile_flag_death_on_bottom;
        case TileType::SuperDeathLeft: return tile_flag_solid | tile_flag_super_death_left | tile_flag_death_on_left;
        case TileType::SuperDeathRight: return tile_flag_solid | tile_flag_super_death_right | tile_flag_death_on_right;
        case TileType::PlayerDeath: return tile_flag_player_death;

        case TileType::Gap: return tile_flag_gap;
        default:
            assert(false);
            return tile_flag_nonsolid;
    }
}


TileType NextTileType(TileType type);
TileType PrevTileType(TileType type);
