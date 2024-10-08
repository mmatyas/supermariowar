#include "MI_MapPreview.h"

#include "Game.h"
#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "ObjectContainer.h"
#include "objecthazard.h"
#include "ResourceManager.h"
#include "objects/IO_FlameCannon.h"
#include "objects/moving/MO_BulletBill.h"
#include "objects/moving/MO_PirhanaPlant.h"
#include "objects/overmap/WO_OrbitHazard.h"
#include "objects/overmap/WO_StraightPathHazard.h"

#include <cstring>

extern CGameValues game_values;
extern CResourceManager* rm;

extern CMap* g_map;
extern MapList *maplist;

extern SDL_Surface* blitdest;
extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];
extern void LoadCurrentMapBackground();

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
    surfaceMapBackground = SDL_CreateRGBSurface(0, App::screenWidth/2, App::screenHeight/2, 16, 0, 0, 0, 0);
    surfaceMapBlockLayer = SDL_CreateRGBSurface(0, App::screenWidth/2, App::screenHeight/2, 16, 0, 0, 0, 0);
    surfaceMapForeground = SDL_CreateRGBSurface(0, App::screenWidth/2, App::screenHeight/2, 16, 0, 0, 0, 0);
    LoadCurrentMap();

    rectDst.x = x + 16;
    rectDst.y = y + 44;
    rectDst.w = App::screenWidth/2;
    rectDst.h = App::screenHeight/2;

    memset(szMapName, 0, sizeof(szMapName));
}

MI_MapPreview::~MI_MapPreview() {}

void MI_MapPreview::Update()
{
    //Update hazards
    noncolcontainer.update();

    objectcontainer[1].update();
    objectcontainer[1].cleanDeadObjects();

    g_map->updatePlatforms();
}

void MI_MapPreview::Draw()
{
    if (!m_visible)
        return;

    short iMapBoxX = m_pos.x + (iWidth >> 1) - 176 - iSlideListOut;

    //Draw the background for the map preview
    rm->menu_dialog.draw(iMapBoxX, m_pos.y + 30, 0, 0, 336, 254);
    rm->menu_dialog.draw(iMapBoxX + 336, m_pos.y + 30, 496, 0, 16, 254);
    rm->menu_dialog.draw(iMapBoxX, m_pos.y + 284, 0, 464, 336, 16);
    rm->menu_dialog.draw(iMapBoxX + 336, m_pos.y + 284, 496, 464, 16, 16);

    rectDst.x = iMapBoxX + 16;

    SDL_BlitSurface(surfaceMapBackground, NULL, blitdest, &rectDst);

    g_map->drawPlatforms(rectDst.x, rectDst.y, 0);

    SDL_BlitSurface(surfaceMapBlockLayer, NULL, blitdest, &rectDst);

    g_map->drawPlatforms(rectDst.x, rectDst.y, 1);

    //Draw map hazards
    for (const std::unique_ptr<CObject>& obj : objectcontainer[1].list()) {
        if (auto* hazard = dynamic_cast<OMO_OrbitHazard*>(obj.get())) {
            hazard->draw(rectDst.x, rectDst.y);
        } else if (auto* hazard = dynamic_cast<OMO_StraightPathHazard*>(obj.get())) {
            hazard->draw(rectDst.x, rectDst.y);
        } else if (auto* hazard = dynamic_cast<IO_FlameCannon*>(obj.get())) {
            hazard->draw(rectDst.x, rectDst.y);
        } else if (auto* hazard = dynamic_cast<MO_BulletBill*>(obj.get())) {
            hazard->draw(rectDst.x, rectDst.y);
        } else if (auto* hazard = dynamic_cast<MO_PirhanaPlant*>(obj.get())) {
            hazard->draw(rectDst.x, rectDst.y);
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
    strncpy(szMapName, maplist->currentShortmapname().c_str(), 255);
    szMapName[255] = 0;

    LoadMap(maplist->currentFilename());
}

void MI_MapPreview::LoadMap(const std::string& szMapPath)
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

std::string MI_MapPreview::GetMapFilePath() const
{
    return maplist->currentFilename();
}

void MI_MapPreview::SetDimensions(short width, short indent)
{
    iWidth = width;
    iIndent = indent;
}
