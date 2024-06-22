#include "MapReader.h"

#include "GameValues.h"
#include "map.h"
#include "MapReaderConstants.h"
#include "FileIO.h"
#include "TilesetManager.h"

extern CTilesetManager* g_tilesetmanager;


namespace {
const short g_iMusicCategoryConversion[26] {0, 3, 8, 5, 1, 9, 3, 4, 10, 8, 1, 0, 9, 0, 0, 7, 4, 1, 1, 6, 4, 7, 6, 3, 0, 4};
} // namespace

void MapReader1500::read_autofilters(CMap& map, BinaryFile& mapfile)
{
    for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
        map.fAutoFilter[iFilter] = false;
}

void MapReader1500::read_tiles(CMap& map, BinaryFile& mapfile)
{
    unsigned short i, j;

    // 2. load map data
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            //Read everything into layer 1
            short iTileID = (short)mapfile.read_i32();

            TilesetTile * tile = &map.mapdata[i][j][1];
            tile->iID = g_tilesetmanager->classicTilesetIndex();
            tile->iCol = iTileID % 32;
            tile->iRow = iTileID / 32;

            TileType iType = g_tilesetmanager->classicTileset().tileType(tile->iCol, tile->iRow);
            map.mapdatatop[i][j].iType = iType;

            map.mapdata[i][j][0].iID = TILESETNONE;
            map.mapdata[i][j][2].iID = TILESETNONE;
            map.mapdata[i][j][3].iID = TILESETNONE;
        }
    }

    // 3. load objects data
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            map.objectdata[i][j].iType = (short)mapfile.read_i32();
            if (map.objectdata[i][j].iType == 6)
                map.objectdata[i][j].iType = -1;

            map.objectdata[i][j].fHidden = false;

            if (map.objectdata[i][j].iType == 1) {
                for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
                    map.objectdata[i][j].iSettings[iSetting] = defaultPowerupSetting(0, iSetting);
            }
        }
    }
}

void MapReader1500::read_background(CMap& map, BinaryFile& mapfile)
{
    // Read old background IDs and convert that to a background filename
    map.backgroundID = (short)mapfile.read_i32();
    map.szBackgroundFile = g_szBackgroundConversion[map.backgroundID];
}

void MapReader1500::read_music_category(CMap& map, BinaryFile& mapfile)
{
    map.musicCategoryID = g_iMusicCategoryConversion[map.backgroundID];
}

bool MapReader1500::load(CMap& map, BinaryFile& mapfile, ReadType readtype)
{
    read_autofilters(map, mapfile);

    if (readtype == read_type_summary)
        return true;

    map.clearPlatforms();

    //Reset position of read cursor
    mapfile.rewind();

    //clear map (we won't be reading in all the layers so it needs to be cleared)
    map.clearMap();

    read_tiles(map, mapfile);
    read_background(map, mapfile);
    read_music_category(map, mapfile);

    //All 1.5 maps will use cloud eyecandy
    map.eyecandy[2] = 1;

    for (short iSwitch = 0; iSwitch < 4; iSwitch++)
        map.iSwitches[iSwitch] = 0;

    return true;
}
