#include "MI_MapField.h"

#include "MapList.h"
#include "ResourceManager.h"

#include <cstring>

extern MapList *maplist;
extern CResourceManager* rm;
extern CObjectContainer objectcontainer[3];

/**************************************
 * MI_MapField Class
 **************************************/

MI_MapField::MI_MapField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, bool showtags)
    : MI_MapPreview(nspr, x, y, width, indent)
    , sSearchString("")
    , iSearchStringTimer(0)
    , fShowtags(showtags)
{
    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    miModifyImageLeft = new MI_Image(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = new MI_Image(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);

    if (fShowtags) {
        iSlideListOut = (iWidth - 352) >> 1;
        iSlideListOutGoal = iSlideListOut;
    } else {
        iSlideListOut = 0;
        iSlideListOutGoal = iSlideListOut;
    }
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
    MI_MapPreview::Update();

    miModifyImageRight->Update();
    miModifyImageLeft->Update();
}

void MI_MapField::Draw()
{
    if (!fShow)
        return;

    //Draw the select field background
    spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);
    rm->menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, szMapName);

    MI_MapPreview::Draw();

    miModifyImageLeft->Draw();
    miModifyImageRight->Draw();

    //rm->menu_font_large.draw(rectDst.x, rectDst.y, sSearchString.c_str());
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
    MI_MapPreview::SetDimensions(width, indent);

    miModifyImageLeft->SetPosition(ix + indent - 26, iy + 4);
    miModifyImageRight->SetPosition(ix + iWidth - 16, iy + 4);

    if (fShowtags) {
        //iSlideListOut = (iWidth - 352) >> 1;
        iSlideListOut = ((int)(iWidth - smw->ScreenWidth * 0.55)) >> 1;
        iSlideListOutGoal = iSlideListOut;
    }
}
