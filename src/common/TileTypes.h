#pragma once


enum TileType {
    tile_nonsolid = 0,
    tile_solid = 1,
    tile_solid_on_top = 2,
    tile_ice = 3,
    tile_death = 4,
    tile_death_on_top = 5,
    tile_death_on_bottom = 6,
    tile_death_on_left = 7,
    tile_death_on_right = 8,
    tile_ice_on_top = 9,
    tile_ice_death_on_bottom = 10,
    tile_ice_death_on_left = 11,
    tile_ice_death_on_right = 12,
    tile_super_death = 13,
    tile_super_death_top = 14,
    tile_super_death_bottom = 15,
    tile_super_death_left = 16,
    tile_super_death_right = 17,
    tile_player_death = 18,
    tile_gap = 19,
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
        case tile_nonsolid: return tile_flag_nonsolid;
        case tile_solid: return tile_flag_solid;
        case tile_solid_on_top: return tile_flag_solid_on_top;
        case tile_ice: return tile_flag_solid | tile_flag_ice;

        case tile_death: return tile_flag_solid | tile_flag_has_regular_death;
        case tile_death_on_top: return tile_flag_solid | tile_flag_death_on_top;
        case tile_death_on_bottom: return tile_flag_solid | tile_flag_death_on_bottom;
        case tile_death_on_left: return tile_flag_solid | tile_flag_death_on_left;
        case tile_death_on_right: return tile_flag_solid | tile_flag_death_on_right;

        case tile_ice_on_top: return tile_flag_ice | tile_flag_solid_on_top;
        case tile_ice_death_on_bottom: return tile_flag_solid | tile_flag_ice | tile_flag_death_on_bottom;
        case tile_ice_death_on_left: return tile_flag_solid | tile_flag_ice | tile_flag_death_on_left;
        case tile_ice_death_on_right: return tile_flag_solid | tile_flag_ice | tile_flag_death_on_right;

        case tile_super_death: return tile_flag_solid | tile_flag_has_regular_death | tile_flag_has_super_death;
        case tile_super_death_top: return tile_flag_solid | tile_flag_super_death_top | tile_flag_death_on_top;
        case tile_super_death_bottom: return tile_flag_solid | tile_flag_super_death_bottom | tile_flag_death_on_bottom;
        case tile_super_death_left: return tile_flag_solid | tile_flag_super_death_left | tile_flag_death_on_left;
        case tile_super_death_right: return tile_flag_solid | tile_flag_super_death_right | tile_flag_death_on_right;
        case tile_player_death: return tile_flag_player_death;

        case tile_gap: return tile_flag_gap;
    }
}


TileType NextTileType(TileType type);
TileType PrevTileType(TileType type);
