#include "MI_MapField.h"

#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "objecthazard.h"
#include "ResourceManager.h"

extern CMap* g_map;
extern MapList *maplist;

extern CGameValues game_values;
extern CResourceManager* rm;

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

/**************************************
 * MI_MapField Class
 **************************************/

MI_MapField::MI_MapField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, bool showtags) :
    UI_Control(x, y)
{
    spr = nspr;

    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iWidth = width;
    iIndent = indent;

    fShowtags = showtags;

    miModifyImageLeft = new MI_Image(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = new MI_Image(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);

	surfaceMapBackground = SDL_CreateRGBSurface(0, smw->ScreenWidth/2, smw->ScreenHeight/2, 16, 0, 0, 0, 0);
    surfaceMapBlockLayer = SDL_CreateRGBSurface(0, smw->ScreenWidth/2, smw->ScreenHeight/2, 16, 0, 0, 0, 0);
    surfaceMapForeground = SDL_CreateRGBSurface(0, smw->ScreenWidth/2, smw->ScreenHeight/2, 16, 0, 0, 0, 0);
    LoadCurrentMap();

    rectDst.x = x + 16;
    rectDst.y = y + 44;
    rectDst.w = smw->ScreenWidth/2;
	rectDst.h = smw->ScreenHeight/2;

    if (fShowtags) {
        iSlideListOut = (iWidth - 352) >> 1;
        iSlideListOutGoal = iSlideListOut;
    } else {
        iSlideListOut = 0;
        iSlideListOutGoal = iSlideListOut;
    }

    sSearchString = "";
    iSearchStringTimer = 0;
}

MI_MapField::~MI_MapField()
{
    delete miModifyImageLeft;
    delete miModifyImageRight;
}

MenuCodeEnum MI_MapField::Modify(bool modify)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    miModifyImageLeft->Show(modify);
    miModifyImageRight->Show(modify);

    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_MapField::SendInput(CPlayerInput * playerInput)
{
    short iPressedKey = playerInput->iPressedKey;

    /*
    if (playerInput->iPressedKey > 0)
    {
        else if (playerInput->iPressedKey == SDLK_PAGEUP)
        {
            short iOldIndex = maplist->GetCurrent()->second->iIndex;
            maplist->prev(true);

            if (iOldIndex != maplist->GetCurrent()->second->iIndex)
            {
                LoadCurrentMap();
                return MENU_CODE_MAP_CHANGED;
            }

            return MENU_CODE_NONE;
        }
        else if (playerInput->iPressedKey == SDLK_PAGEDOWN)
        {
            short iOldIndex = maplist->GetCurrent()->second->iIndex;
            maplist->next(true);

            if (iOldIndex != maplist->GetCurrent()->second->iIndex)
            {
                LoadCurrentMap();
                return MENU_CODE_MAP_CHANGED;
            }

            return MENU_CODE_NONE;
        }
    }
    */

    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed) {
            if (MoveNext(playerInput->outputControls[iPlayer].menu_scrollfast.fDown))
                return MENU_CODE_MAP_CHANGED;

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed) {
            if (MovePrev(playerInput->outputControls[iPlayer].menu_scrollfast.fDown))
                return MENU_CODE_MAP_CHANGED;

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_random.fPressed) {
            return ChooseRandomMap();
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImageLeft->Show(false);
            miModifyImageRight->Show(false);

            fModifying = false;
            return MENU_CODE_UNSELECT_ITEM;
        }

        if (iPlayer == 0 && iPressedKey > 0) {
            if ((iPressedKey >= SDLK_a && iPressedKey <= SDLK_z) ||
                    (iPressedKey >= SDLK_0 && iPressedKey <= SDLK_9) ||
                    iPressedKey == SDLK_MINUS || iPressedKey == SDLK_EQUALS) {
                short iOldIndex = maplist->GetCurrent()->second->iIndex;

                //maplist->startswith((char)playerInput->iPressedKey);

                sSearchString += (char)iPressedKey;
                iSearchStringTimer = 10;

                if (!maplist->startswith(sSearchString.c_str())) {
                    sSearchString = "";
                    iSearchStringTimer = 0;
                }

                if (iOldIndex != maplist->GetCurrent()->second->iIndex) {
                    LoadCurrentMap();
                    return MENU_CODE_MAP_CHANGED;
                }

                return MENU_CODE_NONE;
            }
        }
    }

    return MENU_CODE_NONE;
}

MenuCodeEnum MI_MapField::ChooseRandomMap()
{
    short iOldIndex = maplist->GetCurrent()->second->iIndex;
    maplist->random(true);

    if (iOldIndex != maplist->GetCurrent()->second->iIndex) {
        LoadCurrentMap();
        return MENU_CODE_MAP_CHANGED;
    }

    return MENU_CODE_NONE;
}

void MI_MapField::Update()
{
    //Empty out the search string after a certain time
    if (iSearchStringTimer > 0) {
        if (--iSearchStringTimer == 0) {
            sSearchString = "";
        }
    }

    if (iSlideListOut != iSlideListOutGoal) {
        if (iSlideListOutGoal > iSlideListOut) {
            iSlideListOut += 4;

            if (iSlideListOut > iSlideListOutGoal)
                iSlideListOut = iSlideListOutGoal;
        } else if (iSlideListOutGoal < iSlideListOut) {
            iSlideListOut -= 4;

            if (iSlideListOut < iSlideListOutGoal)
                iSlideListOut = iSlideListOutGoal;
        }
    }

    //Update hazards
    noncolcontainer.update();

    objectcontainer[1].update();
    objectcontainer[1].cleandeadobjects();

    miModifyImageRight->Update();
    miModifyImageLeft->Update();

    g_map->updatePlatforms();
}

void MI_MapField::Draw()
{
    if (!fShow)
        return;

    //Draw the select field background
    spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

    short iMapBoxX = ix + (iWidth >> 1) - 176 - iSlideListOut;

    //Draw the background for the map preview
    rm->menu_dialog.draw(iMapBoxX, iy + 30, 0, 0, 336, 254);
    rm->menu_dialog.draw(iMapBoxX + 336, iy + 30, 496, 0, 16, 254);
    rm->menu_dialog.draw(iMapBoxX, iy + 284, 0, 464, 336, 16);
    rm->menu_dialog.draw(iMapBoxX + 336, iy + 284, 496, 464, 16, 16);

    rectDst.x = iMapBoxX + 16;

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);
    rm->menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, szMapName);

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

    miModifyImageLeft->Draw();
    miModifyImageRight->Draw();

    //rm->menu_font_large.draw(rectDst.x, rectDst.y, sSearchString.c_str());
}

void MI_MapField::LoadCurrentMap()
{
    strncpy(szMapName, maplist->currentShortmapname(), 255);
    szMapName[255] = 0;

    LoadMap(maplist->currentFilename());
}

void MI_MapField::LoadMap(const char * szMapPath)
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

bool MI_MapField::SetMap(const char * paramSzMapName, bool fWorld)
{
    bool fFound = maplist->findexact(paramSzMapName, fWorld);
    LoadCurrentMap();

    return fFound;
}

void MI_MapField::SetSpecialMap(const char * mapName, const char * szMapPath)
{
    strncpy(szMapName, mapName, 255);
    szMapName[255] = 0;

    LoadMap(szMapPath);
}

std::string MI_MapField::GetMapFilePath()
{
    return maplist->currentFilename();
}

MenuCodeEnum MI_MapField::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    //If we are modifying this control, see if we clicked on a next/prev button
    if (fModifying) {
        short x, y, w, h;
        miModifyImageLeft->GetPositionAndSize(&x, &y, &w, &h);

        if (iMouseX >= x && iMouseX < x + w &&
                iMouseY >= y && iMouseY < y + h) {
            if (MovePrev(false))
                return MENU_CODE_MAP_CHANGED;
        }

        miModifyImageRight->GetPositionAndSize(&x, &y, &w, &h);

        if (iMouseX >= x && iMouseX < x + w &&
                iMouseY >= y && iMouseY < y + h) {
            if (MoveNext(false))
                return MENU_CODE_MAP_CHANGED;
        }
    }

    //Otherwise just check to see if we clicked on the whole control
    if (iMouseX >= ix && iMouseX < ix + iWidth && iMouseY >= iy && iMouseY < iy + 32)
        return MENU_CODE_CLICKED;

    //Otherwise this control wasn't clicked at all
    return MENU_CODE_NONE;

}

bool MI_MapField::MovePrev(bool fScrollFast)
{
    return Move(false, fScrollFast);
}

bool MI_MapField::MoveNext(bool fScrollFast)
{
    return Move(true, fScrollFast);
}

bool MI_MapField::Move(bool fNext, bool fScrollFast)
{
    int numadvance = 1;
    if (fScrollFast)
        numadvance = 10;

    short iOldIndex = maplist->GetCurrent()->second->iIndex;
    for (int k = 0; k < numadvance; k++) {
        if (fNext)
            maplist->next(true);
        else
            maplist->prev(true);
    }

    if (iOldIndex != maplist->GetCurrent()->second->iIndex) {
        LoadCurrentMap();
        return true;
    }

    return false;
}

void MI_MapField::SetDimensions(short width, short indent)
{
    iWidth = width;
    iIndent = indent;

    miModifyImageLeft->SetPosition(ix + indent - 26, iy + 4);
    miModifyImageRight->SetPosition(ix + iWidth - 16, iy + 4);

    if (fShowtags) {
        //iSlideListOut = (iWidth - 352) >> 1;
        iSlideListOut = ((int)(iWidth - smw->ScreenWidth * 0.55)) >> 1;
        iSlideListOutGoal = iSlideListOut;
    }
}
