#include "MapReader.h"

#include "map.h"
#include "movingplatform.h"
#include "FileIO.h"
#include "TilesetManager.h"

#include <iostream>

extern CTilesetManager* g_tilesetmanager;
extern short g_iTileTypeConversion[NUMTILETYPES];

using namespace std;

MapReader1800::MapReader1800()
    : MapReader1702()
    , iMaxTilesetID(-1)
    , translationid(NULL)
    , tilesetwidths(NULL)
    , tilesetheights(NULL)
{
    patch_version = 0;
}

MapReader1801::MapReader1801()
    : MapReader1800()
{
    patch_version = 1;
}

MapReader1802::MapReader1802()
    : MapReader1801()
{
    patch_version = 2;
}

void MapReader1800::read_autofilters(CMap& map, BinaryFile& mapfile)
{
    // TODO: handle read fail
    int32_t iAutoFilterValues[NUM_AUTO_FILTERS + 1];
    mapfile.read_i32_array(iAutoFilterValues, NUM_AUTO_FILTERS + 1);

    for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
        map.fAutoFilter[iFilter] = iAutoFilterValues[iFilter] > 0;
}

void MapReader1800::read_tileset(BinaryFile& mapfile)
{
    //Load tileset information

    short iNumTilesets = (short)mapfile.read_i32();

    TilesetTranslation * translation = new TilesetTranslation[iNumTilesets];

    iMaxTilesetID = 0; //Figure out how big the translation array needs to be
    for (short iTileset = 0; iTileset < iNumTilesets; iTileset++) {
        short iTilesetID = mapfile.read_i32();
        translation[iTileset].iID = iTilesetID;

        if (iTilesetID > iMaxTilesetID)
            iMaxTilesetID = iTilesetID;

        mapfile.read_string_long(translation[iTileset].szName, 128);
    }

    translationid = new short[iMaxTilesetID + 1];
    tilesetwidths = new short[iMaxTilesetID + 1];
    tilesetheights = new short[iMaxTilesetID + 1];

    for (short iTileset = 0; iTileset < iNumTilesets; iTileset++) {
        short iID = translation[iTileset].iID;
        translationid[iID] = g_tilesetmanager->GetIndexFromName(translation[iTileset].szName);

        if (translationid[iID] == TILESETUNKNOWN) {
            tilesetwidths[iID] = 1;
            tilesetheights[iID] = 1;
        } else {
            tilesetwidths[iID] = g_tilesetmanager->GetTileset(translationid[iID])->GetWidth();
            tilesetheights[iID] = g_tilesetmanager->GetTileset(translationid[iID])->GetHeight();
        }
    }

    delete [] translation;
}

void MapReader1800::read_tiles(CMap& map, BinaryFile& mapfile)
{
    //2. load map data

    unsigned short i, j, k;
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            for (k = 0; k < MAPLAYERS; k++) {
                TilesetTile * tile = &map.mapdata[i][j][k];
                tile->iID = mapfile.read_i8();
                tile->iCol = mapfile.read_i8();
                tile->iRow = mapfile.read_i8();

                if (tile->iID >= 0) {
                    if (tile->iID > iMaxTilesetID)
                        tile->iID = 0;

                    //Make sure the column and row we read in is within the bounds of the tileset
                    if (tile->iCol < 0 || tile->iCol >= tilesetwidths[tile->iID])
                        tile->iCol = 0;

                    if (tile->iRow < 0 || tile->iRow >= tilesetheights[tile->iID])
                        tile->iRow = 0;

                    //Convert tileset ids into the current game's tileset's ids
                    tile->iID = translationid[tile->iID];
                }
            }

            map.objectdata[i][j].iType = mapfile.read_i8();
            map.objectdata[i][j].fHidden = mapfile.read_bool();
        }
    }
}

void MapReader1800::read_switches(CMap& map, BinaryFile& mapfile)
{
    // Read the on/off switch state of the four colors (turned on or off)
    for (short iSwitch = 0; iSwitch < 4; iSwitch++)
        map.iSwitches[iSwitch] = (short)mapfile.read_i32();
}

void MapReader1800::read_items(CMap& map, BinaryFile& mapfile)
{
    //Load map items (like carryable spikes and springs)
    map.iNumMapItems = mapfile.read_i32();

    for (short j = 0; j < map.iNumMapItems; j++) {
        map.mapitems[j].itype = mapfile.read_i32();
        map.mapitems[j].ix = mapfile.read_i32();
        map.mapitems[j].iy = mapfile.read_i32();
    }
}

void MapReader1800::read_hazards(CMap& map, BinaryFile& mapfile)
{
    //Load map hazards (like fireball strings, rotodiscs, pirhana plants)
    map.iNumMapHazards = mapfile.read_i32();

    for (short iMapHazard = 0; iMapHazard < map.iNumMapHazards; iMapHazard++) {
        map.maphazards[iMapHazard].itype = mapfile.read_i32();
        map.maphazards[iMapHazard].ix = mapfile.read_i32();
        map.maphazards[iMapHazard].iy = mapfile.read_i32();

        for (short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
            map.maphazards[iMapHazard].iparam[iParam] = mapfile.read_i32();

        for (short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
            map.maphazards[iMapHazard].dparam[iParam] = mapfile.read_float();
    }
}

void MapReader1802::read_eyecandy(CMap& map, BinaryFile& mapfile)
{
    //Read in eyecandy to use
    //For all layers if the map format supports it
    map.eyecandy[0] = (short)mapfile.read_i32();
    map.eyecandy[1] = (short)mapfile.read_i32();
    map.eyecandy[2] = (short)mapfile.read_i32();
}

void MapReader1800::read_warp_locations(CMap& map, BinaryFile& mapfile)
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

            for (short sType = 0; sType < NUMSPAWNAREATYPES; sType++)
                map.nospawn[sType][i][j] = mapfile.read_bool();

        }
    }
}

void MapReader1800::read_switchable_blocks(CMap& map, BinaryFile& mapfile)
{
    //Read switch block state data
    int iNumSwitchBlockData = mapfile.read_i32();
    for (short iBlock = 0; iBlock < iNumSwitchBlockData; iBlock++) {
        short iCol = mapfile.read_i8();
        short iRow = mapfile.read_i8();

        map.objectdata[iCol][iRow].iSettings[0] = mapfile.read_i8();
    }
}

bool MapReader1800::read_spawn_areas(CMap& map, BinaryFile& mapfile)
{
    for (unsigned short i = 0; i < NUMSPAWNAREATYPES; i++) {
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

        //If no spawn areas were identified, then create one big spawn area
        if (map.totalspawnsize[i] == 0) {
            map.numspawnareas[i] = 1;
            map.spawnareas[i][0].left = 0;
            map.spawnareas[i][0].width = 20;
            map.spawnareas[i][0].top = 1;
            map.spawnareas[i][0].height = 12;
            map.spawnareas[i][0].size = 220;
            map.totalspawnsize[i] = 220;
        }
    }

    return true;
}

void MapReader1800::read_extra_tiledata(CMap& map, BinaryFile& mapfile)
{
    int iNumExtendedDataBlocks = mapfile.read_i32();

    for (short iBlock = 0; iBlock < iNumExtendedDataBlocks; iBlock++) {
        short iCol = mapfile.read_i8();
        short iRow = mapfile.read_i8();

        short iNumSettings = mapfile.read_i8();
        for (short iSetting = 0; iSetting < iNumSettings; iSetting++)
            map.objectdata[iCol][iRow].iSettings[iSetting] = mapfile.read_i8();
    }
}

void MapReader1800::read_gamemode_settings(CMap& map, BinaryFile& mapfile)
{
    //read mode item locations like flags and race goals
    map.iNumRaceGoals = (short)mapfile.read_i32();
    for (unsigned short j = 0; j < map.iNumRaceGoals; j++) {
        map.racegoallocations[j].x = (short)mapfile.read_i32();
        map.racegoallocations[j].y = (short)mapfile.read_i32();
    }

    map.iNumFlagBases = (short)mapfile.read_i32();
    for (unsigned short j = 0; j < map.iNumFlagBases; j++) {
        map.flagbaselocations[j].x = (short)mapfile.read_i32();
        map.flagbaselocations[j].y = (short)mapfile.read_i32();
    }
}

void MapReader1800::read_platforms(CMap& map, BinaryFile& mapfile, bool fPreview)
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
        if (patch_version >= 1)
            iDrawLayer = (short)mapfile.read_i32();

        //printf("Layer: %d\n", iDrawLayer);

        short iPathType = 0;
        iPathType = mapfile.read_i32();

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

void MapReader1800::read_platform_tiles(CMap& map, BinaryFile& mapfile,
    short iWidth, short iHeight, TilesetTile**& tiles, MapTile**& types)
{
    for (short iCol = 0; iCol < iWidth; iCol++) {
        tiles[iCol] = new TilesetTile[iHeight];
        types[iCol] = new MapTile[iHeight];

        for (short iRow = 0; iRow < iHeight; iRow++) {
            TilesetTile * tile = &tiles[iCol][iRow];

            tile->iID = mapfile.read_i8();
            tile->iCol = mapfile.read_i8();
            tile->iRow = mapfile.read_i8();

            if (tile->iID >= 0) {
                if (iMaxTilesetID != -1 && tile->iID > iMaxTilesetID)
                    tile->iID = 0;

                //Make sure the column and row we read in is within the bounds of the tileset
                if (tile->iCol < 0 || (tilesetwidths && tile->iCol >= tilesetwidths[tile->iID]))
                    tile->iCol = 0;

                if (tile->iRow < 0 || (tilesetheights && tile->iRow >= tilesetheights[tile->iID]))
                    tile->iRow = 0;

                //Convert tileset ids into the current game's tileset's ids
                if (translationid)
                    tile->iID = translationid[tile->iID];
            }

            TileType iType = (TileType)mapfile.read_i32();

            if (iType >= 0 && iType < NUMTILETYPES) {
                types[iCol][iRow].iType = iType;
                types[iCol][iRow].iFlags = g_iTileTypeConversion[iType];
            } else {
                types[iCol][iRow].iType = tile_nonsolid;
                types[iCol][iRow].iFlags = tile_flag_nonsolid;
            }
        }
    }
}

bool MapReader1800::load(CMap& map, BinaryFile& mapfile/*, const char* filename*/, ReadType readtype)
{
    read_autofilters(map, mapfile);

    if (readtype == read_type_summary)
        return true;

    map.clearPlatforms();

    // FIXME
    /*cout << "loading map " << filename;

    if (readtype == read_type_preview)
        cout << " (preview)";

    cout << " [Version " << version[0] << '.' << version[1] << '.'
         << version[2] << '.' << version[3] << " Map Detected]\n";*/

    read_tileset(mapfile);

    read_tiles(map, mapfile);
    read_background(map, mapfile);
    read_switches(map, mapfile);
    read_platforms(map, mapfile, readtype == read_type_preview);

    //All tiles have been loaded so the translation is no longer needed
    delete [] translationid;
    delete [] tilesetwidths;
    delete [] tilesetheights;

    read_items(map, mapfile);
    read_hazards(map, mapfile);
    read_eyecandy(map, mapfile);
    read_music_category(map, mapfile);
    read_warp_locations(map, mapfile);
    read_switchable_blocks(map, mapfile);

    if (readtype == read_type_preview)
        return true;

    read_warp_exits(map, mapfile);
    if (!read_spawn_areas(map, mapfile))
        return false;

    if (!read_draw_areas(map, mapfile))
        return false;

    read_extra_tiledata(map, mapfile);
    read_gamemode_settings(map, mapfile);

    return true;
}
