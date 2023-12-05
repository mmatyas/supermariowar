#include "MI_MapFilterScroll.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern short LookupTeamID(short id);


MI_MapFilterScroll::MI_MapFilterScroll(gfxSprite* nspr, short x, short y, short width, short numlines)
    : UI_Control(x, y)
    , spr(nspr)
    , iNumLines(numlines)
    , iWidth(width)
    , iTopStop((iNumLines - 1) / 2)
{}

void MI_MapFilterScroll::Add(std::string name, short icon)
{
    MFS_ListItem * item = new MFS_ListItem(name, icon, false);
    items.push_back(item);

    if (items.size() >= 1) {
        current = items.begin();
        iIndex = 0;
    }

    iBottomStop = items.size() - iNumLines + iTopStop;
}

MenuCodeEnum MI_MapFilterScroll::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_MapFilterScroll::SendInput(CPlayerInput* playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        //Only allow the controlling team to control the menu (if there is one)
        if (iControllingTeam != -1) {
            if (iControllingTeam != LookupTeamID(iPlayer) || game_values.playercontrol[iPlayer] != 1)
                continue;
        }

        if (playerInput->outputControls[iPlayer].menu_down.fPressed) {
            MoveNext();
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_up.fPressed) {
            MovePrev();
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed) {
            iSelectedColumn = 0;
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
            if (iIndex >= NUM_AUTO_FILTERS)
                iSelectedColumn = 1;

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
            //If the left column is selected, then turn that filter on/off
            if (iSelectedColumn == 0) {
                items[iIndex]->fSelected = !items[iIndex]->fSelected;
                game_values.pfFilters[iIndex] = !game_values.pfFilters[iIndex];
            } else { //otherwise if the right is selected, go into the details of that filter
                game_values.selectedmapfilter = iIndex;
                return MENU_CODE_TO_MAP_FILTER_EDIT;
            }

            return MENU_CODE_NONE;
        }


        if (playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            return MENU_CODE_MAP_FILTER_EXIT;
        }
    }

    return MENU_CODE_NONE;
}


void MI_MapFilterScroll::Update()
{}

void MI_MapFilterScroll::Draw()
{
    if (!fShow)
        return;

           //Draw the background for the map preview
    rm->menu_dialog.draw(ix, iy, 0, 0, iWidth - 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix + iWidth - 16, iy, 496, 0, 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix, iy + iNumLines * 32 + 32, 0, 464, iWidth - 16, 16);
    rm->menu_dialog.draw(ix + iWidth - 16, iy + iNumLines * 32 + 32, 496, 464, 16, 16);

    rm->menu_font_large.drawCentered(ix + (iWidth >> 1), iy + 5, "Map Filters");

           //Draw each filter field
    for (short iLine = 0; iLine < iNumLines && (unsigned short)iLine < items.size(); iLine++) {
        if (iOffset + iLine >= NUM_AUTO_FILTERS) {
            short iHalfLineWidth = (iWidth - 64) >> 1;
            short iLineWidth = iWidth - 64;

            spr->draw(ix + 16, iy + 32 + iLine * 32, 0, (iSelectedLine == iLine  && iSelectedColumn == 0 ? 32 : 0), iHalfLineWidth, 32);
            spr->draw(ix + 16 + iHalfLineWidth, iy + 32 + iLine * 32, 512 - iLineWidth + iHalfLineWidth, (iSelectedLine == iLine && iSelectedColumn == 0 ? 32 : 0), iLineWidth - iHalfLineWidth, 32);

            rm->menu_map_filter.draw(ix + iWidth - 48, iy + 32 + iLine * 32, 48, (iSelectedLine == iLine && iSelectedColumn == 1 ? 32 : 0), 32, 32);
        } else {
            short iHalfLineWidth = (iWidth - 32) >> 1;
            short iLineWidth = iWidth - 32;
            spr->draw(ix + 16, iy + 32 + iLine * 32, 0, (iSelectedLine == iLine && iSelectedColumn == 0 ? 32 : 0), iHalfLineWidth, 32);
            spr->draw(ix + 16 + iHalfLineWidth, iy + 32 + iLine * 32, 512 - iLineWidth + iHalfLineWidth, (iSelectedLine == iLine && iSelectedColumn == 0 ? 32 : 0), iLineWidth - iHalfLineWidth, 32);
        }

        if (items[iOffset + iLine]->fSelected)
            rm->menu_map_filter.draw(ix + 24, iy + 32 + iLine * 32 + 4, 24, 0, 24, 24);

        rm->menu_font_large.drawChopRight(ix + 52, iy + 5 + iLine * 32 + 32, iWidth - 104, items[iOffset + iLine]->sName.c_str());
        rm->spr_map_filter_icons.draw(ix + 28, iy + 32 + iLine * 32 + 8, items[iOffset + iLine]->iIcon % 10 * 16, items[iOffset + iLine]->iIcon / 10 * 16, 16, 16);
    }
}

bool MI_MapFilterScroll::MoveNext()
{
    if (items.empty())
        return false;

    if (iIndex == items.size() - 1)
        return false;

    iIndex++;
    current++;

    if (iIndex > iTopStop && iIndex <= iBottomStop)
        iOffset++;
    else
        iSelectedLine++;

    return true;
}

bool MI_MapFilterScroll::MovePrev()
{
    if (items.empty())
        return false;

    if (iIndex == 0)
        return false;

    iIndex--;
    current--;

    if (iIndex >= iTopStop && iIndex < iBottomStop)
        iOffset--;
    else
        iSelectedLine--;

    if (iIndex < NUM_AUTO_FILTERS)
        iSelectedColumn = 0;

    return true;
}
