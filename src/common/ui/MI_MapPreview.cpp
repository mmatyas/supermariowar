#include "MI_MapPreview.h"

#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "objecthazard.h"
#include "ResourceManager.h"

#include <cstring>

extern CGameValues game_values;
extern CResourceManager* rm;

extern CMap* g_map;
extern MapList *maplist;

extern SDL_Surface* blitdest;
extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];
extern void LoadCurrentMapBackground();
extern void LoadMapHazards(bool fPreview);

#ifndef __EMSCRIPTEN__
    inline void smallDelay() { SDL_Delay(10); }
#else
    inline void smallDelay() {}
#endif

MI_MapPreview::MI_MapPreview(gfxSprite * nspr, short x, short y, short width, short indent)
    : UI_Control(x, y)
    , spr(nspr)
    , iWidth(width)
    , iIndent(indent)
    , iSlideListOut(0)
{
    surfaceMapBackground = SDL_CreateRGBSurface(0, smw->ScreenWidth/2, smw->ScreenHeight/2, 16, 0, 0, 0, 0);
    surfaceMapBlockLayer = SDL_CreateRGBSurface(0, smw->ScreenWidth/2, smw->ScreenHeight/2, 16, 0, 0, 0, 0);
    surfaceMapForeground = SDL_CreateRGBSurface(0, smw->ScreenWidth/2, smw->ScreenHeight/2, 16, 0, 0, 0, 0);
    LoadCurrentMap();

    rectDst.x = x + 16;
    rectDst.y = y + 44;
    rectDst.w = smw->ScreenWidth/2;
    rectDst.h = smw->ScreenHeight/2;

    memset(szMapName, 0, sizeof(szMapName));
}

MI_MapPreview::~MI_MapPreview() {}

void MI_MapPreview::Update()
{
    //Update hazards
    noncolcontainer.update();

    objectcontainer[1].update();
    objectcontainer[1].cleandeadobjects();

    g_map->updatePlatforms();
}

void MI_MapPreview::Draw()
{
    if (!fShow)
        return;

    short iMapBoxX = ix + (iWidth >> 1) - 176 - iSlideListOut;

    //Draw the background for the map preview
    rm->menu_dialog.draw(iMapBoxX, iy + 30, 0, 0, 336, 254);
    rm->menu_dialog.draw(iMapBoxX + 336, iy + 30, 496, 0, 16, 254);
    rm->menu_dialog.draw(iMapBoxX, iy + 284, 0, 464, 336, 16);
    rm->menu_dialog.draw(iMapBoxX + 336, iy + 284, 496, 464, 16, 16);

    rectDst.x = iMapBoxX + 16;

    SDL_BlitSurface(surfaceMapBackground, NULL, blitdest, &rectDst);

    g_map->drawPlatforms(rectDst.x, rectDst.y, 0);

    SDL_BlitSurface(surfaceMapBlockLayer, NULL, blitdest, &rectDst);

    g_map->drawPlatforms(rectDst.x, rectDst.y, 1);

    //Draw map hazards
    for (short i = 0; i < objectcontainer[1].list_end; i++) {
        CObject * object = objectcontainer[1].list[i];
        ObjectType type = object->getObjectType();

        if (type == object_orbithazard) {
            ((OMO_OrbitHazard*)object)->draw(rectDst.x, rectDst.y);
        } else if (type == object_pathhazard) {
            ((OMO_StraightPathHazard*)object)->draw(rectDst.x, rectDst.y);
        } else if (type == object_flamecannon) {
            ((IO_FlameCannon*)object)->draw(rectDst.x, rectDst.y);
        } else if (type == object_moving) {
            IO_MovingObject * movingobject = (IO_MovingObject *) object;

            if (movingobject->getMovingObjectType() == movingobject_bulletbill) {
                ((MO_BulletBill*)movingobject)->draw(rectDst.x, rectDst.y);
            } else if (movingobject->getMovingObjectType() == movingobject_pirhanaplant) {
                ((MO_PirhanaPlant*)movingobject)->draw(rectDst.x, rectDst.y);
            }
        }
    }

    g_map->drawPlatforms(rectDst.x, rectDst.y, 2);

    if (game_values.toplayer)
        SDL_BlitSurface(surfaceMapForeground, NULL, blitdest, &rectDst);

    g_map->drawPlatforms(rectDst.x, rectDst.y, 3);
    g_map->drawPlatforms(rectDst.x, rectDst.y, 4);
}

void MI_MapPreview::LoadCurrentMap()
{
    strncpy(szMapName, maplist->currentShortmapname(), 255);
    szMapName[255] = 0;

    LoadMap(maplist->currentFilename());
}

void MI_MapPreview::LoadMap(const char * szMapPath)
{
    g_map->loadMap(szMapPath, read_type_preview);
    smallDelay(); //Sleeps to help the music from skipping

    LoadCurrentMapBackground();
    smallDelay();

    g_map->preDrawPreviewBackground(&rm->spr_background, surfaceMapBackground, false);
    smallDelay();

    g_map->preDrawPreviewBlocks(surfaceMapBlockLayer, false);
    smallDelay();

    g_map->preDrawPreviewMapItems(surfaceMapBackground, false);
    smallDelay();

    g_map->preDrawPreviewForeground(surfaceMapForeground, false);
    smallDelay();

    g_map->preDrawPreviewWarps(game_values.toplayer ? surfaceMapForeground : surfaceMapBackground, false);
    smallDelay();

    LoadMapHazards(true);
}

bool MI_MapPreview::SetMap(const char * paramSzMapName, bool fWorld)
{
    bool fFound = maplist->findexact(paramSzMapName, fWorld);
    LoadCurrentMap();

    return fFound;
}

void MI_MapPreview::SetSpecialMap(const char * mapName, const char * szMapPath)
{
    strncpy(szMapName, mapName, 255);
    szMapName[255] = 0;

    LoadMap(szMapPath);
}

const std::string MI_MapPreview::GetMapFilePath()
{
    return maplist->currentFilename();
}

void MI_MapPreview::SetDimensions(short width, short indent)
{
    iWidth = width;
    iIndent = indent;
}
