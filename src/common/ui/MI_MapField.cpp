#include "MI_MapField.h"

#include "Game.h"
#include "input.h"
#include "MapList.h"
#include "ObjectContainer.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

#include <cstring>

extern MapList *maplist;
extern CResourceManager* rm;
extern CObjectContainer objectcontainer[3];

/**************************************
 * MI_MapField Class
 **************************************/

MI_MapField::MI_MapField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent, bool showtags)
    : MI_MapPreview(nspr, x, y, width, indent)
    , szName(std::move(name))
    , sSearchString("")
    , iSearchStringTimer(0)
    , fShowtags(showtags)
{
    miModifyImageLeft = std::make_unique<MI_Image>(nspr, m_pos.x + indent - 26, m_pos.y + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->setVisible(false);

    miModifyImageRight = std::make_unique<MI_Image>(nspr, m_pos.x + iWidth - 16, m_pos.y + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->setVisible(false);

    if (fShowtags) {
        iSlideListOut = (iWidth - 352) >> 1;
        iSlideListOutGoal = iSlideListOut;
    } else {
        iSlideListOut = 0;
        iSlideListOutGoal = iSlideListOut;
    }
}

MenuCodeEnum MI_MapField::Modify(bool modify)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    miModifyImageLeft->setVisible(modify);
    miModifyImageRight->setVisible(modify);

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
            miModifyImageLeft->setVisible(false);
            miModifyImageRight->setVisible(false);

            fModifying = false;
            return MENU_CODE_UNSELECT_ITEM;
        }

        if (iPlayer == 0 && iPressedKey > 0) {
            if ((iPressedKey >= SDLK_a && iPressedKey <= SDLK_z) ||
                    (iPressedKey >= SDLK_0 && iPressedKey <= SDLK_9) ||
                    iPressedKey == SDLK_MINUS || iPressedKey == SDLK_EQUALS) {
                short iOldIndex = maplist->GetCurrent()->second.iIndex;

                //maplist->startswith((char)playerInput->iPressedKey);

                sSearchString += (char)iPressedKey;
                iSearchStringTimer = 10;

                if (!maplist->startswith(sSearchString.c_str())) {
                    sSearchString = "";
                    iSearchStringTimer = 0;
                }

                if (iOldIndex != maplist->GetCurrent()->second.iIndex) {
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
    short iOldIndex = maplist->GetCurrent()->second.iIndex;
    maplist->random(true);

    if (iOldIndex != maplist->GetCurrent()->second.iIndex) {
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
    if (!m_visible)
        return;

    //Draw the select field background
    spr->draw(m_pos.x, m_pos.y, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
    spr->draw(m_pos.x + iIndent - 16, m_pos.y, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(m_pos.x + iIndent + 16, m_pos.y, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(m_pos.x + 16, m_pos.y + 5, iIndent - 8, szName.c_str());
    rm->menu_font_large.drawChopRight(m_pos.x + iIndent + 8, m_pos.y + 5, iWidth - iIndent - 24, szMapName);

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
        miModifyImageLeft->GetPositionAndSize(x, y, w, h);

        if (iMouseX >= x && iMouseX < x + w &&
                iMouseY >= y && iMouseY < y + h) {
            if (MovePrev(false))
                return MENU_CODE_MAP_CHANGED;
        }

        miModifyImageRight->GetPositionAndSize(x, y, w, h);

        if (iMouseX >= x && iMouseX < x + w &&
                iMouseY >= y && iMouseY < y + h) {
            if (MoveNext(false))
                return MENU_CODE_MAP_CHANGED;
        }
    }

    //Otherwise just check to see if we clicked on the whole control
    if (iMouseX >= m_pos.x && iMouseX < m_pos.x + iWidth && iMouseY >= m_pos.y && iMouseY < m_pos.y + 32)
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

    short iOldIndex = maplist->GetCurrent()->second.iIndex;
    for (int k = 0; k < numadvance; k++) {
        if (fNext)
            maplist->next(true);
        else
            maplist->prev(true);
    }

    if (iOldIndex != maplist->GetCurrent()->second.iIndex) {
        LoadCurrentMap();
        return true;
    }

    return false;
}

void MI_MapField::SetDimensions(short width, short indent)
{
    MI_MapPreview::SetDimensions(width, indent);

    miModifyImageLeft->SetPosition(m_pos.x + indent - 26, m_pos.y + 4);
    miModifyImageRight->SetPosition(m_pos.x + iWidth - 16, m_pos.y + 4);

    if (fShowtags) {
        //iSlideListOut = (iWidth - 352) >> 1;
        iSlideListOut = ((int)(iWidth - App::screenWidth * 0.55)) >> 1;
        iSlideListOutGoal = iSlideListOut;
    }
}
