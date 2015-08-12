#include "MapReader.h"

#include "map.h"
#include "FileIO.h"
#include "TilesetManager.h"

#include <iostream>

extern CTilesetManager* g_tilesetmanager;
extern const char * g_szBackgroundConversion[26];
extern short g_iTileTypeConversion[NUMTILETYPES];

using namespace std;

void MapLoader1800::read_autofilters(CMap& map, FILE* mapfile)
{
    // TODO: handle read fail
    int iAutoFilterValues[NUM_AUTO_FILTERS + 1];
    ReadIntChunk(iAutoFilterValues, NUM_AUTO_FILTERS + 1, mapfile);

    for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
        map.fAutoFilter[iFilter] = iAutoFilterValues[iFilter] > 0;
}

void MapLoader1800::read_tileset(FILE* mapfile)
{
    //Load tileset information

    short iNumTilesets = ReadInt(mapfile);

    TilesetTranslation * translation = new TilesetTranslation[iNumTilesets];

    iMaxTilesetID = 0; //Figure out how big the translation array needs to be
    for (short iTileset = 0; iTileset < iNumTilesets; iTileset++) {
        short iTilesetID = ReadInt(mapfile);
        translation[iTileset].iID = iTilesetID;

        if (iTilesetID > iMaxTilesetID)
            iMaxTilesetID = iTilesetID;

        ReadString(translation[iTileset].szName, 128, mapfile);
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

void MapLoader1800::read_tiles(CMap& map, FILE* mapfile)
{
    //2. load map data

    unsigned short i, j, k;
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            for (k = 0; k < MAPLAYERS; k++) {
                TilesetTile * tile = &map.mapdata[i][j][k];
                tile->iID = ReadByteAsShort(mapfile);
                tile->iCol = ReadByteAsShort(mapfile);
                tile->iRow = ReadByteAsShort(mapfile);

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

            map.objectdata[i][j].iType = ReadByteAsShort(mapfile);
            map.objectdata[i][j].fHidden = ReadBool(mapfile);
        }
    }
}

void MapLoader1800::read_switches(CMap& map, FILE* mapfile)
{
    // Read the on/off switch state of the four colors (turned on or off)
    for (short iSwitch = 0; iSwitch < 4; iSwitch++)
        map.iSwitches[iSwitch] = (short)ReadInt(mapfile);
}

void MapLoader1800::read_items(CMap& map, FILE* mapfile)
{
    //Load map items (like carryable spikes and springs)
    map.iNumMapItems = ReadInt(mapfile);

    for (short j = 0; j < map.iNumMapItems; j++) {
        map.mapitems[j].itype = ReadInt(mapfile);
        map.mapitems[j].ix = ReadInt(mapfile);
        map.mapitems[j].iy = ReadInt(mapfile);
    }
}

void MapLoader1800::read_hazards(CMap& map, FILE* mapfile)
{
    //Load map hazards (like fireball strings, rotodiscs, pirhana plants)
    map.iNumMapHazards = ReadInt(mapfile);

    for (short iMapHazard = 0; iMapHazard < map.iNumMapHazards; iMapHazard++) {
        map.maphazards[iMapHazard].itype = ReadInt(mapfile);
        map.maphazards[iMapHazard].ix = ReadInt(mapfile);
        map.maphazards[iMapHazard].iy = ReadInt(mapfile);

        for (short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
            map.maphazards[iMapHazard].iparam[iParam] = ReadInt(mapfile);

        for (short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
            map.maphazards[iMapHazard].dparam[iParam] = ReadFloat(mapfile);
    }
}

void MapLoader1802::read_eyecandy(CMap& map, FILE* mapfile)
{
    //Read in eyecandy to use
    //For all layers if the map format supports it
    map.eyecandy[0] = (short)ReadInt(mapfile);
    map.eyecandy[1] = (short)ReadInt(mapfile);
    map.eyecandy[2] = (short)ReadInt(mapfile);
}

void MapLoader1800::read_warp_locations(CMap& map, FILE* mapfile)
{
    for (unsigned short j = 0; j < MAPHEIGHT; j++) {
        for (unsigned short i = 0; i < MAPWIDTH; i++) {
            TileType iType = (TileType)ReadInt(mapfile);

            if (iType >= 0 && iType < NUMTILETYPES) {
                map.mapdatatop[i][j].iType = iType;
                map.mapdatatop[i][j].iFlags = g_iTileTypeConversion[iType];
            } else {
                map.mapdatatop[i][j].iType = tile_nonsolid;
                map.mapdatatop[i][j].iFlags = tile_flag_nonsolid;
            }

            map.warpdata[i][j].direction = (WarpEnterDirection)ReadInt(mapfile);
            map.warpdata[i][j].connection = (short)ReadInt(mapfile);
            map.warpdata[i][j].id = (short)ReadInt(mapfile);

            for (short sType = 0; sType < NUMSPAWNAREATYPES; sType++)
                map.nospawn[sType][i][j] = ReadBool(mapfile);

        }
    }
}

void MapLoader1800::read_switchable_blocks(CMap& map, FILE* mapfile)
{
    //Read switch block state data
    int iNumSwitchBlockData = ReadInt(mapfile);
    for (short iBlock = 0; iBlock < iNumSwitchBlockData; iBlock++) {
        short iCol = ReadByteAsShort(mapfile);
        short iRow = ReadByteAsShort(mapfile);

        map.objectdata[iCol][iRow].iSettings[0] = ReadByteAsShort(mapfile);
    }
}

bool MapLoader1800::read_spawn_areas(CMap& map, FILE* mapfile)
{
    for (unsigned short i = 0; i < NUMSPAWNAREATYPES; i++) {
        map.totalspawnsize[i] = 0;
        map.numspawnareas[i] = (short)ReadInt(mapfile);

        if (map.numspawnareas[i] > MAXSPAWNAREAS) {
            cout << endl << " ERROR: Number of spawn areas (" << map.numspawnareas[i]
                 << ") was greater than max allowed (" << MAXSPAWNAREAS << ')'
                 << endl;
            return false;
        }

        for (int m = 0; m < map.numspawnareas[i]; m++) {
            map.spawnareas[i][m].left = (short)ReadInt(mapfile);
            map.spawnareas[i][m].top = (short)ReadInt(mapfile);
            map.spawnareas[i][m].width = (short)ReadInt(mapfile);
            map.spawnareas[i][m].height = (short)ReadInt(mapfile);
            map.spawnareas[i][m].size = (short)ReadInt(mapfile);

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

void MapLoader1800::read_extra_tiledata(CMap& map, FILE* mapfile)
{
    int iNumExtendedDataBlocks = ReadInt(mapfile);

    for (short iBlock = 0; iBlock < iNumExtendedDataBlocks; iBlock++) {
        short iCol = ReadByteAsShort(mapfile);
        short iRow = ReadByteAsShort(mapfile);

        short iNumSettings = ReadByteAsShort(mapfile);
        for (short iSetting = 0; iSetting < iNumSettings; iSetting++)
            map.objectdata[iCol][iRow].iSettings[iSetting] = ReadByteAsShort(mapfile);
    }
}

void MapLoader1800::read_gamemode_settings(CMap& map, FILE* mapfile)
{
    //read mode item locations like flags and race goals
    map.iNumRaceGoals = (short)ReadInt(mapfile);
    for (unsigned short j = 0; j < map.iNumRaceGoals; j++) {
        map.racegoallocations[j].x = (short)ReadInt(mapfile);
        map.racegoallocations[j].y = (short)ReadInt(mapfile);
    }

    map.iNumFlagBases = (short)ReadInt(mapfile);
    for (unsigned short j = 0; j < map.iNumFlagBases; j++) {
        map.flagbaselocations[j].x = (short)ReadInt(mapfile);
        map.flagbaselocations[j].y = (short)ReadInt(mapfile);
    }
}

bool MapLoader1800::load(CMap& map, FILE* mapfile/*, const char* filename*/, ReadType readtype)
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

    map.loadPlatforms(mapfile, readtype == read_type_preview, version, translationid,
        tilesetwidths, tilesetheights, iMaxTilesetID);

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
