#include "Helpers.h"

#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "path.h"
#include "ResourceManager.h"
#include "world.h"
#include "WorldTourStop.h"

#include "SDL_image.h"

extern SDL_Surface* blitdest;
extern CMap* g_map;
extern MapList* maplist;
extern CGameValues game_values;
extern gfxSprite spr_largedialog;


short adjustedForeground(short fgSprite, short bgSprite)
{
    if (fgSprite >= WORLD_FOREGROUND_STAGE_OFFSET)
        return fgSprite;

    const short iPathStyle = fgSprite / WORLD_PATH_SPRITE_SET_SIZE;
    fgSprite %= WORLD_PATH_SPRITE_SET_SIZE;

           // Convert already adjusted sprites back to their "base" sprite
    if (fgSprite >= 11 && fgSprite <= 18)
        fgSprite = 2 - (fgSprite % 2);
    else if (fgSprite >= 7 && fgSprite <= 10)
        fgSprite -= 4;

    const short iBackgroundSprite = bgSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE;
    if (iBackgroundSprite == 1)
        return fgSprite + iPathStyle * WORLD_PATH_SPRITE_SET_SIZE;

    if (fgSprite == 2 && (iBackgroundSprite == 12 || iBackgroundSprite == 20 || iBackgroundSprite == 23 || iBackgroundSprite == 24 || iBackgroundSprite == 27 || iBackgroundSprite == 32 || iBackgroundSprite == 36 || iBackgroundSprite == 37 || iBackgroundSprite == 40)) {
        fgSprite = 14;
    } else if (fgSprite == 2 && (iBackgroundSprite == 13 || iBackgroundSprite == 21 || iBackgroundSprite == 22 || iBackgroundSprite == 25 || iBackgroundSprite == 26 || iBackgroundSprite == 33 || iBackgroundSprite == 34 || iBackgroundSprite == 35 || iBackgroundSprite == 41)) {
        fgSprite = 16;
    } else if (fgSprite == 1 && (iBackgroundSprite == 14 || iBackgroundSprite == 18 || iBackgroundSprite == 19 || iBackgroundSprite == 26 || iBackgroundSprite == 27 || iBackgroundSprite == 31 || iBackgroundSprite == 34 || iBackgroundSprite == 37 || iBackgroundSprite == 39)) {
        fgSprite = 13;
    } else if (fgSprite == 1 && (iBackgroundSprite == 15 || iBackgroundSprite == 16 || iBackgroundSprite == 17 || iBackgroundSprite == 24 || iBackgroundSprite == 25 || iBackgroundSprite == 30 || iBackgroundSprite == 35 || iBackgroundSprite == 36 || iBackgroundSprite == 38)) {
        fgSprite = 15;
    } else if (fgSprite == 2 && (iBackgroundSprite == 28 || iBackgroundSprite == 38 || iBackgroundSprite == 39 || iBackgroundSprite == 43)) {
        fgSprite = 12;
    } else if (fgSprite == 1 && (iBackgroundSprite == 28 || iBackgroundSprite == 40 || iBackgroundSprite == 41 || iBackgroundSprite == 42)) {
        fgSprite = 11;
    } else {
        if (fgSprite == 1 || fgSprite == 2)
            fgSprite += 16;
        else if (fgSprite >= 3 && fgSprite <= 6)
            fgSprite += 4;
    }

    return fgSprite + iPathStyle * WORLD_PATH_SPRITE_SET_SIZE;
}


bool updateForeground(WorldMapTile& tile)
{
    short newForeground = adjustedForeground(tile.iForegroundSprite, tile.iBackgroundSprite);
    if (tile.iForegroundSprite != newForeground) {
        tile.iForegroundSprite = newForeground;
        return true;
    }
    return false;
}


bool updateCoastline(WorldMap& world, Vec2s center)
{
    short firstX= center.x == 0 ? 0 : center.x - 1;
    short lastX = center.x == world.w() - 1 ? world.w() - 1 : center.x + 1;

    short firstY = center.y == 0 ? 0 : center.y - 1;
    short lastY = center.y == world.h() - 1 ? world.h() - 1 : center.y + 1;

    bool changed = false;
    for (short y = firstY; y <= lastY; y++) {
        for (short x = firstX; x <= lastX; x++) {
            WorldMapTile& tile = world.getTiles().at(x, y);
            changed |= autoSetTile(world, {x, y});
            changed |= updateForeground(tile);
        }
    }

    return changed;
}


bool autoSetTile(WorldMap& world, Vec2s center)
{
    // TODO: Bounds check
    WorldMapTile& centerTile = world.getTiles().at(center.x, center.y);

    // Don't need to do anything if this tile is solid
    if (centerTile.iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE == 1)
        return false;

    bool iTile[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    short iNeighborIndex = 0;

    short iNeighborStyle[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    for (short y = center.y - 1; y <= center.y + 1; y++) {
        for (short x = center.x - 1; x <= center.x + 1; x++) {
            if (x == center.x && y == center.y)
                continue;

            if (0 <= y && y < world.h() && 0 <= x && x < world.w()) {
                short iBackgroundSprite = world.getTiles().at(x, y).iBackgroundSprite;

                if (iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE == 1) {
                    iTile[iNeighborIndex] = true;
                    iNeighborStyle[iBackgroundSprite / WORLD_BACKGROUND_SPRITE_SET_SIZE]++;
                }
            }

            iNeighborIndex++;
        }
    }

    short iMaxStyle = 0;
    short iTileStyleOffset = 0;
    for (short iStyle = 0; iStyle < 10; iStyle++) {
        if (iNeighborStyle[iStyle] > iMaxStyle) {
            iMaxStyle = iNeighborStyle[iStyle];
            iTileStyleOffset = iStyle * WORLD_BACKGROUND_SPRITE_SET_SIZE;
        }
    }

    short iNewTile = 0;
    if (iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 44;
    } else if (iTile[0] && !iTile[1] && !iTile[3] && iTile[4] && iTile[5] && !iTile[6]) {
        iNewTile = iTileStyleOffset + 30;
    } else if (!iTile[1] && iTile[2] && iTile[3] && !iTile[4] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 31;
    } else if (iTile[1] && !iTile[3] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 32;
    } else if (iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && !iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 33;
    } else if (!iTile[1] && iTile[2] && iTile[3] && !iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 34;
    } else if (iTile[0] && !iTile[1] && !iTile[3] && iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 35;
    } else if (iTile[1] && !iTile[3] && iTile[4] && iTile[5] && !iTile[6]) {
        iNewTile = iTileStyleOffset + 36;
    } else if (iTile[1] && iTile[3] && !iTile[4] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 37;
    } else if (iTile[0] && !iTile[1] && iTile[2] && !iTile[4] && iTile[7]) {
        iNewTile = iTileStyleOffset + 45;
    } else if (iTile[2] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 46;
    } else if (iTile[0] && !iTile[3] && iTile[5] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 47;
    } else if (iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && iTile[5]) {
        iNewTile = iTileStyleOffset + 48;
    } else if (iTile[1] && iTile[3] && iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 28;
    } else if (iTile[1] && !iTile[3] && iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 38;
    } else if (iTile[1] && iTile[3] && !iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 39;
    } else if (iTile[1] && iTile[3] && iTile[4] && !iTile[6]) {
        iNewTile = iTileStyleOffset + 40;
    } else if (!iTile[1] && iTile[3] && iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 41;
    } else if (!iTile[1] && iTile[3] && iTile[4] && !iTile[6]) {
        iNewTile = iTileStyleOffset + 42;
    } else if (iTile[1] && !iTile[3] && !iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 43;
    } else if (iTile[0] && !iTile[1] && iTile[4]) {
        iNewTile = iTileStyleOffset + 16;
    } else if (iTile[4] && iTile[5] && !iTile[6]) {
        iNewTile = iTileStyleOffset + 17;
    } else if (iTile[3] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 18;
    } else if (!iTile[1] && iTile[2] && iTile[3]) {
        iNewTile = iTileStyleOffset + 19;
    } else if (iTile[1] && !iTile[4] && iTile[7]) {
        iNewTile = iTileStyleOffset + 20;
    } else if (iTile[2] && !iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 21;
    } else if (iTile[0] && !iTile[3] && iTile[6]) {
        iNewTile = iTileStyleOffset + 22;
    } else if (iTile[1] && !iTile[3] && iTile[5]) {
        iNewTile = iTileStyleOffset + 23;
    } else if (iTile[0] && !iTile[1] && iTile[2]) {
        iNewTile = iTileStyleOffset + 6;
    } else if (iTile[5] && !iTile[6] && iTile[7]) {
        iNewTile = iTileStyleOffset + 7;
    } else if (iTile[0] && !iTile[3] && iTile[5]) {
        iNewTile = iTileStyleOffset + 8;
    } else if (iTile[2] && !iTile[4] && iTile[7]) {
        iNewTile = iTileStyleOffset + 9;
    } else if (iTile[1] && iTile[4]) {
        iNewTile = iTileStyleOffset + 24;
    } else if (iTile[4] && iTile[6]) {
        iNewTile = iTileStyleOffset + 25;
    } else if (iTile[3] && iTile[6]) {
        iNewTile = iTileStyleOffset + 26;
    } else if (iTile[1] && iTile[3]) {
        iNewTile = iTileStyleOffset + 27;
    } else if (iTile[2] && iTile[5]) {
        iNewTile = iTileStyleOffset + 10;
    } else if (iTile[0] && iTile[7]) {
        iNewTile = iTileStyleOffset + 11;
    } else if (iTile[1]) {
        iNewTile = iTileStyleOffset + 12;
    } else if (iTile[6]) {
        iNewTile = iTileStyleOffset + 13;
    } else if (iTile[3]) {
        iNewTile = iTileStyleOffset + 14;
    } else if (iTile[4]) {
        iNewTile = iTileStyleOffset + 15;
    } else if (iTile[2]) {
        iNewTile = iTileStyleOffset + 2;
    } else if (iTile[0]) {
        iNewTile = iTileStyleOffset + 3;
    } else if (iTile[5]) {
        iNewTile = iTileStyleOffset + 4;
    } else if (iTile[7]) {
        iNewTile = iTileStyleOffset + 5;
    } else {
        iNewTile = iTileStyleOffset + 0;
    }

    if (centerTile.iBackgroundSprite != iNewTile) {
        centerTile.iBackgroundSprite = iNewTile;
        return true;
    }

    return false;
}


bool setTileFgWithConnection(WorldMapTile& tile, short newTileId)
{
    const bool changed = tile.iForegroundSprite != newTileId;
    if (tile.iForegroundSprite != newTileId) {
        tile.iForegroundSprite = newTileId;

        if (WORLD_BRIDGE_SPRITE_OFFSET <= newTileId && newTileId <= WORLD_BRIDGE_SPRITE_OFFSET + 3)
            tile.iConnectionType = newTileId - WORLD_BRIDGE_SPRITE_OFFSET + 12;
    }
    return changed;
}


void DisplayStageDetails(short iStageId, short iMouseX, short iMouseY, CResourceManager& rm)
{
    static SDL_Surface* sMapThumbnail = nullptr;
    static std::string sCachedMapPath;

    TourStop* ts = game_values.tourstops[iStageId];

    // If we're pointing to a new stage or no stage at all
    if (ts->iStageType == 1) {
        if (sMapThumbnail) {
            SDL_FreeSurface(sMapThumbnail);
            sMapThumbnail = NULL;
        }
        sCachedMapPath.clear();
    } else {
        if (!ts->pszMapFile.empty() && ts->pszMapFile != sCachedMapPath) {
            if (sMapThumbnail) {
                SDL_FreeSurface(sMapThumbnail);
                sMapThumbnail = NULL;
            }

            if (maplist->findexact(ts->pszMapFile.c_str(), false)) {
                g_map->loadMap(maplist->currentFilename(), read_type_preview);
                sMapThumbnail = g_map->createThumbnailSurface(true);
            } else {  // otherwise show a unknown map icon
                sMapThumbnail = IMG_Load(convertPath("gfx/leveleditor/leveleditor_mapnotfound.png").c_str());
            }

            sCachedMapPath = ts->pszMapFile;
        }
    }

    short iMode = ts->iMode;
    if (ts->iStageType == 1)
        iMode = 24;
    else if (ts->iMode >= 1000)
        iMode = ts->iMode - 975;  // Convert mode id from map file to internal id for special stage modes

           // Make sure we're displaying it on the screen
    if (iMouseX > 408)
        iMouseX = 408;

    if (iMode < GAMEMODE_LAST || (iMode >= 25 && iMode <= 27)) {
        if (iMouseY > 248)
            iMouseY = 248;

        spr_largedialog.draw(iMouseX, iMouseY, 0, 0, 116, 116);
        spr_largedialog.draw(iMouseX + 116, iMouseY, 140, 0, 116, 116);
        spr_largedialog.draw(iMouseX, iMouseY + 116, 0, 108, 116, 116);
        spr_largedialog.draw(iMouseX + 116, iMouseY + 116, 140, 108, 116, 116);
    } else if (iMode == 24) {
        // Make sure we're displaying it on the screen
        if (iMouseY > 392)
            iMouseY = 392;

        spr_largedialog.draw(iMouseX, iMouseY, 0, 0, 116, 44);
        spr_largedialog.draw(iMouseX + 116, iMouseY, 140, 0, 116, 44);
        spr_largedialog.draw(iMouseX, iMouseY + 44, 0, 180, 116, 44);
        spr_largedialog.draw(iMouseX + 116, iMouseY + 44, 140, 180, 116, 44);
    }

    rm.menu_mode_large.draw(iMouseX + 16, iMouseY + 16, iMode << 5, 0, 32, 32);

    rm.menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 16, 164, ts->szName);

    char szPrint[128];
    if (iMode != 24) {
        sprintf(szPrint, "Goal: %d", ts->iGoal);
        rm.menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 34, 164, szPrint);

        sprintf(szPrint, "Points: %d", ts->iPoints);
        rm.menu_font_small.drawChopRight(iMouseX + 16, iMouseY + 176, 100, szPrint);

        sprintf(szPrint, "End: %s", ts->fEndStage ? "Yes" : "No");
        rm.menu_font_small.drawChopRight(iMouseX + 126, iMouseY + 176, 80, szPrint);

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            WorldStageBonus* wsb = &ts->wsbBonuses[iBonus];
            rm.spr_worlditemsplace.draw(iMouseX + iBonus * 20 + 16, iMouseY + 194, wsb->iWinnerPlace * 20, 0, 20, 20);

            short iBonusIcon = wsb->iBonus;
            gfxSprite* spr_icon = iBonusIcon < NUM_POWERUPS ? &rm.spr_storedpowerupsmall : &rm.spr_worlditemssmall;
            spr_icon->draw(iMouseX + iBonus * 20 + 18, iMouseY + 196, (iBonusIcon < NUM_POWERUPS ? iBonusIcon : iBonusIcon - NUM_POWERUPS) << 4, 0, 16, 16);
        }

        if (sMapThumbnail) {
            SDL_Rect rSrc = { 0, 0, 160, 120 };
            SDL_Rect rDst = { iMouseX + 16, iMouseY + 52, 160, 120 };

            SDL_BlitSurface(sMapThumbnail, &rSrc, blitdest, &rDst);
        }
    } else {
        sprintf(szPrint, "Sort: %s", ts->iBonusType == 0 ? "Fixed" : "Random");
        rm.menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 34, 164, szPrint);

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            short iBonusIcon = ts->wsbBonuses[iBonus].iBonus;
            gfxSprite* spr_icon = NULL;
            short iSrcX = 0, iSrcY = 0;

            if (iBonusIcon < NUM_POWERUPS) {
                spr_icon = &rm.spr_storedpowerupsmall;
                iSrcX = iBonusIcon << 4;
                iSrcY = 0;
            } else if (iBonusIcon < NUM_POWERUPS + NUM_WORLD_POWERUPS) {
                spr_icon = &rm.spr_worlditemssmall;
                iSrcX = (iBonusIcon - NUM_POWERUPS) << 4;
                iSrcY = 0;
            } else if (iBonusIcon < NUM_POWERUPS + NUM_WORLD_POWERUPS + 10) {
                spr_icon = &rm.spr_worlditemssmall;
                iSrcX = (iBonusIcon - NUM_POWERUPS - NUM_WORLD_POWERUPS) << 4;
                iSrcY = 16;
            } else {
                spr_icon = &rm.spr_worlditemssmall;
                iSrcX = (iBonusIcon - NUM_POWERUPS - NUM_WORLD_POWERUPS - 10) << 4;
                iSrcY = 32;
            }

            spr_icon->draw(iMouseX + iBonus * 20 + 18, iMouseY + 52, iSrcX, iSrcY, 16, 16);
        }
    }
}
