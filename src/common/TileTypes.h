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
    tile_gap = 19
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
    tile_flag_has_death = 8056,
    tile_flag_super_death_top = 256,
    tile_flag_super_death_bottom = 512,
    tile_flag_super_death_left = 1024,
    tile_flag_super_death_right = 2048,
    tile_flag_player_death = 4096,
    tile_flag_super_or_player_death_top = 4352,
    tile_flag_super_or_player_death_bottom = 4608,
    tile_flag_super_or_player_death_left = 5120,
    tile_flag_super_or_player_death_right = 6144,
    tile_flag_player_or_death_on_bottom = 4112
};


TileType NextTileType(TileType type);
TileType PrevTileType(TileType type);
