#include "MapReader.h"

#include "map.h"
#include "FileIO.h"
#include "TilesetManager.h"

#include <iostream>

extern CTilesetManager* g_tilesetmanager;
extern short g_iTileTypeConversion[NUMTILETYPES];
extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];


namespace {
// Conversion to switch 1.6 tiles into 1.7 tiles
constexpr short g_iTileConversion[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 575,670,702,703,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47, 332,331,330,637,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
    74, 75, 76, 77, 78, 79, 365,299,366,853,
    537,595,505,658,659,656,657,774,775,776,
    540,96, 97, 98, 643,644,645,26, 27, 506,
    122,123,124,690,691,688,689,745,746,747,
    569,128,129,130,704,677,907,90, 91, 572,
    931,602,539,885,728,729,730,731,186,187,
    188,160,161,162,736,192,194,30, 31, 898,
    737,738,739,800,760,761,762,763,218,219,
    220,864,865,509,768,224,226,62, 63, 930,
    769,770,771,508,598,599,600,507,601,510,
    603,896,897,192,193,260,259,193,194,541,
    627,699,697,940,941,942,860,861,862,250,
    252,543,158,498,499,500,922,924,854,886,
    605,125,126,127,720,721,752,754,753,722,
    723,928,929,563,531,532,923,571,882,851,
    309,310,311,343,278,341,99, 100,101,489,
    490,491,384,385,386,147,113,148,914,664,
    373,374,375,376,310,377,131,132,133,553,
    554,555,416,417,418,179,145,180,946,570,
    867,868,869,213,214,215,163,164,165,566,
    567,568,448,449,450,863,530,504,892,883,
    899,900,901,245,246,247,777,778,779,250,
    251,252,856,857,858,859,562,712,893,905,
    908,909,910,624,625,626,592,593,594,282,
    283,284,888,889,890,891,710,711,894,937
};
} // namespace


MapReader1600::MapReader1600()
    : MapReader1500()
    , parse_nospawn(false)
    , fix_spawnareas(false)
{ }

MapReader160A::MapReader160A()
    : MapReader1600()
{
    fix_spawnareas = true;
}

MapReader1610::MapReader1610()
    : MapReader160A()
{
    parse_nospawn = true;
}

void MapReader1600::read_tiles(CMap& map, BinaryFile& mapfile)
{
    for (unsigned short j = 0; j < MAPHEIGHT; j++) {
        for (unsigned short i = 0; i < MAPWIDTH; i++) {
            for (unsigned short k = 0; k < MAPLAYERS; k++) {
                //This converts from 1.6 tileset to 1.7 tileset
                short iTile = (short)mapfile.read_i32();

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

            for (short k = MAPLAYERS - 1; k >= 0; k--) {
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

            map.objectdata[i][j].iType = (short)mapfile.read_i32();
            if (map.objectdata[i][j].iType == 6)
                map.objectdata[i][j].iType = -1;

            map.objectdata[i][j].fHidden = false;

            if (map.objectdata[i][j].iType == 1) {
                for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
                    map.objectdata[i][j].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
            }

            map.warpdata[i][j].direction = (WarpEnterDirection)mapfile.read_i32();
            map.warpdata[i][j].connection = (short)mapfile.read_i32();
            map.warpdata[i][j].id = (short)mapfile.read_i32();

            if (parse_nospawn) {
                map.nospawn[0][i][j] = mapfile.read_i32() == 0 ? false : true;

                for (short iType = 1; iType < NUMSPAWNAREATYPES; iType++)
                    map.nospawn[iType][i][j] = map.nospawn[0][i][j];
            }
        }
    }
}

void MapReader1600::read_eyecandy(CMap& map, BinaryFile& mapfile)
{
    //Read in eyecandy to use
    map.eyecandy[2] = (short)mapfile.read_i32();
}

void MapReader1600::read_warp_exits(CMap& map, BinaryFile& mapfile)
{
    map.maxConnection = 0;

    map.numwarpexits = (short)mapfile.read_i32();
    for (unsigned short i = 0; i < map.numwarpexits && i < MAXWARPS; i++) {
        map.warpexits[i].direction = (WarpExitDirection)mapfile.read_i32();
        map.warpexits[i].connection = (short)mapfile.read_i32();
        map.warpexits[i].id = (short)mapfile.read_i32();
        map.warpexits[i].x = (short)mapfile.read_i32();
        map.warpexits[i].y = (short)mapfile.read_i32();

        map.warpexits[i].lockx = (short)mapfile.read_i32();
        map.warpexits[i].locky = (short)mapfile.read_i32();

        map.warpexits[i].warpx = (short)mapfile.read_i32();
        map.warpexits[i].warpy = (short)mapfile.read_i32();
        map.warpexits[i].numblocks = (short)mapfile.read_i32();

        if (map.warpexits[i].connection > map.maxConnection)
            map.maxConnection = map.warpexits[i].connection;
    }

    //Ignore any more warps than the max
    for (unsigned short i = 0; i < map.numwarpexits - MAXWARPS; i++) {
        for (unsigned short j = 0; j < 10; j++)
            mapfile.read_i32();
    }

    if (map.numwarpexits > MAXWARPS)
        map.numwarpexits = MAXWARPS;
}

bool MapReader1600::read_spawn_areas(CMap& map, BinaryFile& mapfile)
{
    //Read spawn areas
    map.totalspawnsize[0] = 0;
    map.numspawnareas[0] = (short)mapfile.read_i32();

    if (map.numspawnareas[0] > MAXSPAWNAREAS) {
        std::cout << std::endl << " ERROR: Number of spawn areas (" << map.numspawnareas[0]
                  << ") was greater than max allowed (" << MAXSPAWNAREAS << ')'
                  << std::endl;
        return false;
    }

    //Read the only spawn area definition in the file
    for (int m = 0; m < map.numspawnareas[0]; m++) {
        map.spawnareas[0][m].left = (short)mapfile.read_i32();
        map.spawnareas[0][m].top = (short)mapfile.read_i32();
        map.spawnareas[0][m].width = (short)mapfile.read_i32();
        map.spawnareas[0][m].height = (short)mapfile.read_i32();
        map.spawnareas[0][m].size = (short)mapfile.read_i32();

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

bool MapReader1600::read_draw_areas(CMap& map, BinaryFile& mapfile)
{
    //Read draw areas (foreground tiles drawing optimization)
    map.numdrawareas = (short)mapfile.read_i32();

    if (map.numdrawareas > MAXDRAWAREAS) {
        std::cout << std::endl << " ERROR: Number of draw areas (" << map.numdrawareas
                  << ") was greater than max allowed (" << MAXDRAWAREAS << ')'
                  << std::endl;
        return false;
    }

    //Load rects to help optimize drawing the foreground
    for (int m = 0; m < map.numdrawareas; m++) {
        map.drawareas[m].x = (Sint16)mapfile.read_i32();
        map.drawareas[m].y = (Sint16)mapfile.read_i32();
        map.drawareas[m].w = (Uint16)mapfile.read_i32();
        map.drawareas[m].h = (Uint16)mapfile.read_i32();
    }

    return true;
}

bool MapReader1600::load(CMap& map, BinaryFile& mapfile, ReadType readtype)
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

bool MapReader1610::load(CMap& map, BinaryFile& mapfile, ReadType readtype)
{
    if (!MapReader160A::load(map, mapfile, readtype))
        return false;

    return read_draw_areas(map, mapfile);
}
