#include "DonutBlock.h"

#include "map.h"
#include "movingplatform.h"
#include "player.h"
#include "TilesetManager.h"

extern CMap* g_map;
extern CTilesetManager* g_tilesetmanager;

B_DonutBlock::B_DonutBlock(gfxSprite *nspr, short x, short y) :
    IO_Block(nspr, x, y)
{
    counter = 0;
    jigglex = 0;
    jigglecounter = 0;
}


void B_DonutBlock::draw()
{
    spr->draw(ix + jigglex, iy);
}


void B_DonutBlock::update()
{
    //If a player is standing on us, jiggle and then fall
    if (state == 0) {
        counter = 0;
        jigglex = 0;
        jigglecounter = 0;
    } else {
        if (++jigglecounter > 1) {
            jigglecounter = 0;

            if (jigglex == 2)
                jigglex = -2;
            else
                jigglex = 2;
        }

        if (++counter > 50) {
            triggerBehavior(state - 1);
        }
    }

    state = 0;
}

bool B_DonutBlock::hittop(CPlayer * player, bool useBehavior)
{
    IO_Block::hittop(player, useBehavior);

    if (useBehavior)
        state = player->globalID + 1;

    return false;
}

void B_DonutBlock::triggerBehavior(short iPlayerId)
{
    //eyecandy[2].add(new EC_FallingObject(&rm->spr_donutblock, ix, iy, 0.0f, 0, 0, 0, 0));

    TilesetTile ** tiledata = new TilesetTile*[1];
    tiledata[0] = new TilesetTile[1];
    tiledata[0][0].iID = g_tilesetmanager->GetClassicTilesetIndex();
    tiledata[0][0].iCol = 29;
    tiledata[0][0].iRow = 15;

    MapTile ** typedata = new MapTile*[1];
    typedata[0] = new MapTile[1];
    typedata[0][0].iType = tile_solid;
    typedata[0][0].iFlags = tile_flag_solid;

    MovingPlatformPath * path = new FallingPath((float)ix + 16.0f, (float)iy + 15.8f);
    MovingPlatform * platform = new MovingPlatform(tiledata, typedata, 1, 1, 2, path, false);
    platform->SetPlayerId(iPlayerId);

    g_map->AddTemporaryPlatform(platform);

    dead = true;
    g_map->blockdata[col][row] = NULL;
    g_map->UpdateTileGap(col, row);
}
