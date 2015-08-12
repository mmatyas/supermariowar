#include "MapReader.h"

#include "map.h"
#include "FileIO.h"
#include "TilesetManager.h"

#include <iostream>

extern CTilesetManager* g_tilesetmanager;
extern short g_iTileConversion[];
extern short g_iTileTypeConversion[NUMTILETYPES];
extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

MapReader1600::MapReader1600()
    : MapReader1500()
    , parse_nospawn(false)
    , fix_spawnareas(false)
{ }

void MapReader1600::read_tiles(CMap& map, FILE* mapfile)
{
    for (unsigned short j = 0; j < MAPHEIGHT; j++) {
        for (unsigned short i = 0; i < MAPWIDTH; i++) {
            for (unsigned short k = 0; k < MAPLAYERS; k++) {
                //This converts from 1.6 tileset to 1.7 tileset
                short iTile = (short)ReadInt(mapfile);

                if (iTile == 300) {
                    map.mapdata[i][j][k].iID = TILESETNONE;
                } else {
                    short iTileID = g_iTileConversion[iTile];

                    TilesetTile * tile = &map.mapdata[i][j][k];
                    tile->iID = g_tilesetmanager->GetClassicTilesetIndex();
                    tile->iCol = iTileID % 32;
                    tile->iRow = iTileID / 32;
                }
            }

            map.mapdatatop[i][j].iType = tile_nonsolid;
            map.mapdatatop[i][j].iFlags = tile_flag_nonsolid;

            for (unsigned short k = MAPLAYERS - 1; k >= 0; k--) {
                TilesetTile * tile = &map.mapdata[i][j][k];
                TileType type = g_tilesetmanager->GetClassicTileset()->GetTileType(tile->iCol, tile->iRow);
                if (type != tile_nonsolid) {
                    if (type >= 0 && type < NUMTILETYPES) {
                        map.mapdatatop[i][j].iType = type;
                        map.mapdatatop[i][j].iFlags = g_iTileTypeConversion[type];
                    } else {
                        map.mapdatatop[i][j].iType = tile_nonsolid;
                        map.mapdatatop[i][j].iFlags = tile_flag_nonsolid;
                    }

                    break;
                }
            }

            map.objectdata[i][j].iType = (short)ReadInt(mapfile);
            if (map.objectdata[i][j].iType == 6)
                map.objectdata[i][j].iType = -1;

            map.objectdata[i][j].fHidden = false;

            if (map.objectdata[i][j].iType == 1) {
                for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
                    map.objectdata[i][j].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
            }

            map.warpdata[i][j].direction = (WarpEnterDirection)ReadInt(mapfile);
            map.warpdata[i][j].connection = (short)ReadInt(mapfile);
            map.warpdata[i][j].id = (short)ReadInt(mapfile);

            if (parse_nospawn) {
                map.nospawn[0][i][j] = ReadInt(mapfile) == 0 ? false : true;

                for (short iType = 1; iType < NUMSPAWNAREATYPES; iType++)
                    map.nospawn[iType][i][j] = map.nospawn[0][i][j];
            }
        }
    }
}

void MapReader1600::read_eyecandy(CMap& map, FILE* mapfile)
{
    //Read in eyecandy to use
    map.eyecandy[2] = (short)ReadInt(mapfile);
}

void MapReader1600::read_warp_exits(CMap& map, FILE* mapfile)
{
    map.maxConnection = 0;

    map.numwarpexits = (short)ReadInt(mapfile);
    for (unsigned short i = 0; i < map.numwarpexits && i < MAXWARPS; i++) {
        map.warpexits[i].direction = (WarpExitDirection)ReadInt(mapfile);
        map.warpexits[i].connection = (short)ReadInt(mapfile);
        map.warpexits[i].id = (short)ReadInt(mapfile);
        map.warpexits[i].x = (short)ReadInt(mapfile);
        map.warpexits[i].y = (short)ReadInt(mapfile);

        map.warpexits[i].lockx = (short)ReadInt(mapfile);
        map.warpexits[i].locky = (short)ReadInt(mapfile);

        map.warpexits[i].warpx = (short)ReadInt(mapfile);
        map.warpexits[i].warpy = (short)ReadInt(mapfile);
        map.warpexits[i].numblocks = (short)ReadInt(mapfile);

        if (map.warpexits[i].connection > map.maxConnection)
            map.maxConnection = map.warpexits[i].connection;
    }

    //Ignore any more warps than the max
    for (unsigned short i = 0; i < map.numwarpexits - MAXWARPS; i++) {
        for (unsigned short j = 0; j < 10; j++)
            ReadInt(mapfile);
    }

    if (map.numwarpexits > MAXWARPS)
        map.numwarpexits = MAXWARPS;
}

bool MapReader1600::read_spawn_areas(CMap& map, FILE* mapfile)
{
    //Read spawn areas
    map.totalspawnsize[0] = 0;
    map.numspawnareas[0] = (short)ReadInt(mapfile);

    if (map.numspawnareas[0] > MAXSPAWNAREAS) {
        std::cout << std::endl << " ERROR: Number of spawn areas (" << map.numspawnareas[0]
                  << ") was greater than max allowed (" << MAXSPAWNAREAS << ')'
                  << std::endl;
        return false;
    }

    //Read the only spawn area definition in the file
    for (int m = 0; m < map.numspawnareas[0]; m++) {
        map.spawnareas[0][m].left = (short)ReadInt(mapfile);
        map.spawnareas[0][m].top = (short)ReadInt(mapfile);
        map.spawnareas[0][m].width = (short)ReadInt(mapfile);
        map.spawnareas[0][m].height = (short)ReadInt(mapfile);
        map.spawnareas[0][m].size = (short)ReadInt(mapfile);

        if (fix_spawnareas) {
            map.spawnareas[0][m].width -= map.spawnareas[0][m].left;
            map.spawnareas[0][m].height -= map.spawnareas[0][m].top;
        }

        map.totalspawnsize[0] += map.spawnareas[0][m].size;
    }

    //Then duplicate it for all the other spawn areas
    for (short i2 = 1; i2 < NUMSPAWNAREATYPES; i2++) {
        map.totalspawnsize[i2] = map.totalspawnsize[0];
        map.numspawnareas[i2] = map.numspawnareas[0];

        //Read the only spawn area definition in the file
        for (int m = 0; m < map.numspawnareas[0]; m++) {
            map.spawnareas[i2][m].left = map.spawnareas[0][m].left;
            map.spawnareas[i2][m].top = map.spawnareas[0][m].top;
            map.spawnareas[i2][m].width = map.spawnareas[0][m].width;
            map.spawnareas[i2][m].height = map.spawnareas[0][m].height;
            map.spawnareas[i2][m].size = map.spawnareas[0][m].size;

            if (fix_spawnareas) {
                map.spawnareas[i2][m].width -= map.spawnareas[i2][m].left;
                map.spawnareas[i2][m].height -= map.spawnareas[i2][m].top;
            }
        }
    }

    return true;
}

bool MapReader1600::read_draw_areas(CMap& map, FILE* mapfile)
{
    //Read draw areas (foreground tiles drawing optimization)
    map.numdrawareas = (short)ReadInt(mapfile);

    if (map.numdrawareas > MAXDRAWAREAS) {
        std::cout << std::endl << " ERROR: Number of draw areas (" << map.numdrawareas
                  << ") was greater than max allowed (" << MAXDRAWAREAS << ')'
                  << std::endl;
        return false;
    }

    //Load rects to help optimize drawing the foreground
    for (int m = 0; m < map.numdrawareas; m++) {
        map.drawareas[m].x = (Sint16)ReadInt(mapfile);
        map.drawareas[m].y = (Sint16)ReadInt(mapfile);
        map.drawareas[m].w = (Uint16)ReadInt(mapfile);
        map.drawareas[m].h = (Uint16)ReadInt(mapfile);
    }

    return true;
}

bool MapReader1600::load(CMap& map, FILE* mapfile, ReadType readtype)
{
    read_autofilters(map, mapfile);

    if (readtype == read_type_summary)
        return true;

    map.clearPlatforms();

    read_tiles(map, mapfile);
    read_background(map, mapfile);
    read_music_category(map, mapfile);

    read_eyecandy(map, mapfile);
    read_warp_exits(map, mapfile);

    if (!read_spawn_areas(map, mapfile))
        return false;

    return true;
}

bool MapReader160A::load(CMap& map, FILE* mapfile, ReadType readtype)
{
    fix_spawnareas = true;

    return MapReader1600::load(map, mapfile, readtype);
}


bool MapReader1610::load(CMap& map, FILE* mapfile, ReadType readtype)
{
    parse_nospawn = true;

    if (!MapReader160A::load(map, mapfile, readtype))
        return false;

    return read_draw_areas(map, mapfile);
}
