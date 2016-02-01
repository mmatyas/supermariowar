#include "MapReader.h"

#include "map.h"
#include "movingplatform.h"
#include "FileIO.h"
#include "TilesetManager.h"

#include <cassert>
#include <cstring>
#include <iostream>

extern CTilesetManager* g_tilesetmanager;
extern const char* g_szBackgroundConversion[26];
extern short g_iTileTypeConversion[NUMTILETYPES];
extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

using namespace std;

MapReader1700::MapReader1700()
    : MapReader1600()
{
    patch_version = 0;
}

MapReader1701::MapReader1701()
    : MapReader1700()
{
    patch_version = 1;
}

MapReader1702::MapReader1702()
    : MapReader1701()
{
    patch_version = 2;
}

void MapReader1702::read_autofilters(CMap& map, BinaryFile& mapfile)
{
    int32_t iAutoFilterValues[9];
    mapfile.read_i32_array(iAutoFilterValues, 9);

    for (short iFilter = 0; iFilter < 8; iFilter++)
        map.fAutoFilter[iFilter] = iAutoFilterValues[iFilter] > 0;

    for (short iFilter = 8; iFilter < NUM_AUTO_FILTERS; iFilter++)
        map.fAutoFilter[iFilter] = false;

    //Read density and don't do anything with it at the moment
    //int iDensity = iAutoFilterValues[8];
}

void MapReader1700::read_tiles(CMap& map, BinaryFile& mapfile)
{
    short iClassicTilesetID = g_tilesetmanager->GetIndexFromName("Classic");

    unsigned short i, j, k;
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            for (k = 0; k < MAPLAYERS; k++) {
                short iTileID = (short)mapfile.read_i32();

                TilesetTile * tile = &map.mapdata[i][j][k];

                if (iTileID == TILESETSIZE) {
                    tile->iID = TILESETNONE;
                    tile->iCol = 0;
                    tile->iRow = 0;
                } else {
                    tile->iID = iClassicTilesetID;
                    tile->iCol = iTileID % TILESETWIDTH;
                    tile->iRow = iTileID / TILESETWIDTH;
                }
            }

            map.objectdata[i][j].iType = (short)mapfile.read_i32();
            if (map.objectdata[i][j].iType == 15)
                map.objectdata[i][j].iType = -1;

            map.objectdata[i][j].fHidden = false;

            if (map.objectdata[i][j].iType == 1) {
                for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
                    map.objectdata[i][j].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
            }
        }
    }
}

void MapReader1701::read_background(CMap& map, BinaryFile& mapfile)
{
    //Read in background to use
    mapfile.read_string_long(map.szBackgroundFile, 128);

    for (short iBackground = 0; iBackground < 26; iBackground++) {
        const char * szFindUnderscore = strstr(g_szBackgroundConversion[iBackground], "_");

        // All items must have an underscore in g_szBackgroundConversion
        assert(szFindUnderscore);

        szFindUnderscore++;

        if (!strcmp(szFindUnderscore, map.szBackgroundFile)) {
            assert(strlen(g_szBackgroundConversion[iBackground]) <= 128);
            strcpy(map.szBackgroundFile, g_szBackgroundConversion[iBackground]);
        }
    }
}

void MapReader1702::read_background(CMap& map, BinaryFile& mapfile)
{
    //Read in background to use
    mapfile.read_string_long(map.szBackgroundFile, 128);
}

void MapReader1700::set_preview_switches(CMap& map, BinaryFile& mapfile)
{
    // if it is a preview, for older maps, set the on/off blocks to on by default

    // FIXME
    /*if (iReadType != read_type_preview)
        return;*/

    //Read on/off switches
    for (unsigned short iSwitch = 0; iSwitch < 4; iSwitch++) {
        map.iSwitches[iSwitch] = 1;
    }

    //Set all the on/off blocks correctly
    for (unsigned short j = 0; j < MAPHEIGHT; j++) {
        for (unsigned short i = 0; i < MAPWIDTH; i++) {
            if (map.objectdata[i][j].iType >= 11 && map.objectdata[i][j].iType <= 14) {
                map.objectdata[i][j].iSettings[0] = 1;
            }
        }
    }
}

void MapReader1700::read_switches(CMap& map, BinaryFile& mapfile)
{
    //Read on/off switches
    for (unsigned short iSwitch = 0; iSwitch < 4; iSwitch++) {
        map.iSwitches[iSwitch] = 1 - (short)mapfile.read_i32();
    }

    //Set all the on/off blocks correctly
    for (unsigned short j = 0; j < MAPHEIGHT; j++) {
        for (unsigned short i = 0; i < MAPWIDTH; i++) {
            if (map.objectdata[i][j].iType >= 11 && map.objectdata[i][j].iType <= 14) {
                map.objectdata[i][j].iSettings[0] = map.iSwitches[map.objectdata[i][j].iType - 11];
            }
        }
    }
}

void MapReader1701::read_music_category(CMap& map, BinaryFile& mapfile)
{
    map.musicCategoryID = mapfile.read_i32();
}

void MapReader1700::read_warp_locations(CMap& map, BinaryFile& mapfile)
{
    for (unsigned short j = 0; j < MAPHEIGHT; j++) {
        for (unsigned short i = 0; i < MAPWIDTH; i++) {
            TileType iType = (TileType)mapfile.read_i32();

            if (iType >= 0 && iType < NUMTILETYPES) {
                map.mapdatatop[i][j].iType = iType;
                map.mapdatatop[i][j].iFlags = g_iTileTypeConversion[iType];
            } else {
                map.mapdatatop[i][j].iType = tile_nonsolid;
                map.mapdatatop[i][j].iFlags = tile_flag_nonsolid;
            }

            map.warpdata[i][j].direction = (WarpEnterDirection)mapfile.read_i32();
            map.warpdata[i][j].connection = (short)mapfile.read_i32();
            map.warpdata[i][j].id = (short)mapfile.read_i32();

            for (short sType = 0; sType < 6; sType += 5)
                map.nospawn[sType][i][j] = mapfile.read_i32() == 0 ? false : true;

            //Copy player no spawn areas into team no spawn areas
            for (short sType = 1; sType < 5; sType++)
                map.nospawn[sType][i][j] = map.nospawn[0][i][j];

        }
    }
}

bool MapReader1700::read_spawn_areas(CMap& map, BinaryFile& mapfile)
{
    //Read spawn areas
    for (unsigned short i = 0; i < 6; i += 5) {
        map.totalspawnsize[i] = 0;
        map.numspawnareas[i] = (short)mapfile.read_i32();

        if (map.numspawnareas[i] > MAXSPAWNAREAS) {
            cout << endl << " ERROR: Number of spawn areas (" << map.numspawnareas[i]
                 << ") was greater than max allowed (" << MAXSPAWNAREAS << ')'
                 << endl;
            return false;
        }

        for (int m = 0; m < map.numspawnareas[i]; m++) {
            map.spawnareas[i][m].left = (short)mapfile.read_i32();
            map.spawnareas[i][m].top = (short)mapfile.read_i32();
            map.spawnareas[i][m].width = (short)mapfile.read_i32();
            map.spawnareas[i][m].height = (short)mapfile.read_i32();
            map.spawnareas[i][m].size = (short)mapfile.read_i32();

            map.totalspawnsize[i] += map.spawnareas[i][m].size;
        }
    }

    //Copy player spawn areas to team specific spawn areas
    for (short iType = 1; iType < 5; iType++) {
        map.totalspawnsize[iType] = map.totalspawnsize[0];
        map.numspawnareas[iType] = map.numspawnareas[0];

        for (int m = 0; m < map.numspawnareas[0]; m++) {
            map.spawnareas[iType][m].left = map.spawnareas[0][m].left;
            map.spawnareas[iType][m].top = map.spawnareas[0][m].top;
            map.spawnareas[iType][m].width = map.spawnareas[0][m].width;
            map.spawnareas[iType][m].height = map.spawnareas[0][m].height;
            map.spawnareas[iType][m].size = map.spawnareas[0][m].size;
        }
    }

    return true;
}

void MapReader1700::read_platforms(CMap& map, BinaryFile& mapfile, bool fPreview)
{
    map.clearPlatforms();

    //Load moving platforms
    map.iNumPlatforms = (short)mapfile.read_i32();
    map.platforms = new MovingPlatform*[map.iNumPlatforms];

    for (short iPlatform = 0; iPlatform < map.iNumPlatforms; iPlatform++) {
        short iWidth = (short)mapfile.read_i32();
        short iHeight = (short)mapfile.read_i32();

        TilesetTile ** tiles = new TilesetTile*[iWidth];
        MapTile ** types = new MapTile*[iWidth];

        read_platform_tiles(map, mapfile, iWidth, iHeight, tiles, types);

        short iDrawLayer = 2;
        //printf("Layer: %d\n", iDrawLayer);

        short iPathType = 0;
        //printf("PathType: %d\n", iPathType);

        MovingPlatformPath* path = NULL;
        path = read_platform_path_details(mapfile, iPathType, fPreview);
        if (!path)
            continue;

        MovingPlatform * platform = new MovingPlatform(tiles, types, iWidth, iHeight, iDrawLayer, path, fPreview);
        map.platforms[iPlatform] = platform;
        map.platformdrawlayer[iDrawLayer].push_back(platform);
    }
}

void MapReader1700::read_platform_tiles(CMap& map, BinaryFile& mapfile,
    short iWidth, short iHeight, TilesetTile**& tiles, MapTile**& types)
{
    for (short iCol = 0; iCol < iWidth; iCol++) {
        tiles[iCol] = new TilesetTile[iHeight];
        types[iCol] = new MapTile[iHeight];

        for (short iRow = 0; iRow < iHeight; iRow++) {
            TilesetTile * tile = &tiles[iCol][iRow];

            short iTile = mapfile.read_i32();
            TileType type;

            if (iTile == TILESETSIZE) {
                tile->iID = TILESETNONE;
                tile->iCol = 0;
                tile->iRow = 0;

                type = tile_nonsolid;
            } else {
                tile->iID = g_tilesetmanager->GetClassicTilesetIndex();
                tile->iCol = iTile % TILESETWIDTH;
                tile->iRow = iTile / TILESETWIDTH;

                type = g_tilesetmanager->GetClassicTileset()->GetTileType(tile->iCol, tile->iRow);
            }

            if (type >= 0 && type < NUMTILETYPES) {
                types[iCol][iRow].iType = type;
                types[iCol][iRow].iFlags = g_iTileTypeConversion[type];
            } else {
                map.mapdatatop[iCol][iRow].iType = tile_nonsolid;
                map.mapdatatop[iCol][iRow].iFlags = tile_flag_nonsolid;
            }
        }
    }
}

MovingPlatformPath* MapReader1700::read_platform_path_details(BinaryFile& mapfile, short iPathType, bool fPreview)
{
    MovingPlatformPath* path = NULL;
    if (iPathType == 0) { //segment path
        float fStartX = mapfile.read_float();
        float fStartY = mapfile.read_float();
        float fEndX = mapfile.read_float();
        float fEndY = mapfile.read_float();
        float fVelocity = mapfile.read_float();

        path = new StraightPath(fVelocity, fStartX, fStartY, fEndX, fEndY, fPreview);

        //printf("Read segment path\n");
        //printf("StartX: %.2f StartY:%.2f EndX:%.2f EndY:%.2f Velocity:%.2f\n", fStartX, fStartY, fEndX, fEndY, fVelocity);
    } else if (iPathType == 1) { //continuous path
        float fStartX = mapfile.read_float();
        float fStartY = mapfile.read_float();
        float fAngle = mapfile.read_float();
        float fVelocity = mapfile.read_float();

        path = new StraightPathContinuous(fVelocity, fStartX, fStartY, fAngle, fPreview);

        //printf("Read continuous path\n");
        //printf("StartX: %.2f StartY:%.2f Angle:%.2f Velocity:%.2f\n", fStartX, fStartY, fAngle, fVelocity);
    } else if (iPathType == 2) { //elliptical path
        float fRadiusX = mapfile.read_float();
        float fRadiusY = mapfile.read_float();
        float fCenterX = mapfile.read_float();
        float fCenterY = mapfile.read_float();
        float fAngle = mapfile.read_float();
        float fVelocity = mapfile.read_float();

        path = new EllipsePath(fVelocity, fAngle, fRadiusX, fRadiusY, fCenterX, fCenterY, fPreview);

        //printf("Read elliptical path\n");
        //printf("CenterX: %.2f CenterY:%.2f Angle:%.2f RadiusX: %.2f RadiusY: %.2f Velocity:%.2f\n", fCenterX, fCenterY, fAngle, fRadiusX, fRadiusY, fVelocity);
    }

    return path;
}

bool MapReader1700::load(CMap& map, BinaryFile& mapfile, ReadType readtype)
{
    read_autofilters(map, mapfile);

    if (readtype == read_type_summary)
        return true;

    map.clearPlatforms();

    read_tiles(map, mapfile);
    read_background(map, mapfile);

    if (patch_version >= 1)
        read_switches(map, mapfile);
    else if (readtype != read_type_preview)
        set_preview_switches(map, mapfile);

    if (patch_version >= 2) {
        //short translationid[1] = {g_tilesetmanager->GetIndexFromName("Classic")};
        read_platforms(map, mapfile, readtype == read_type_preview);
    }

    read_eyecandy(map, mapfile);
    read_music_category(map, mapfile);
    read_warp_locations(map, mapfile);

    if (readtype == read_type_preview)
        return true;

    read_warp_exits(map, mapfile);
    read_spawn_areas(map, mapfile);
    if (!read_draw_areas(map, mapfile))
        return false;

    if (patch_version <= 1) {
        //short translationid[1] = {g_tilesetmanager->GetIndexFromName("Classic")};
        read_platforms(map, mapfile, readtype == read_type_preview);
    }

    if (patch_version == 0)
        read_switches(map, mapfile);

    return true;
}
