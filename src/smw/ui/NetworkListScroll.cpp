#include "NetworkListScroll.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern short LookupTeamID(short id);


NLS_ListItem::NLS_ListItem()
    : sLeft("")
    , sRight("")
{}

NLS_ListItem::NLS_ListItem(const std::string& left, const std::string& right)
    : sLeft(left)
    , sRight(right)
{}

NLS_ListItem::~NLS_ListItem() {};

/**************************************
 * MI_NetworkListScroll Class
 **************************************/

MI_NetworkListScroll::MI_NetworkListScroll(gfxSprite * nspr, short x, short y, short width,
    short numlines, const std::string& title, MenuCodeEnum acceptCode, MenuCodeEnum cancelCode) :
    UI_Control(x, y)
{
    spr = nspr;
    iWidth = width;
    iNumLines = numlines;

    iSelectedLine = -1;
    iSelectedLineBackup = 0;
    iIndex = 0;
    iRemoteIndex = &iIndex;
    iOffset = 0;

    iTopStop = (iNumLines - 1) >> 1;
    iBottomStop = 0;

    sTitle = title;

    iAcceptCode = acceptCode;
    iCancelCode = cancelCode;
}

MI_NetworkListScroll::~MI_NetworkListScroll()
{}

void MI_NetworkListScroll::Add(const std::string& left, const std::string& right)
{
    NLS_ListItem item(left, right);
    items.push_back(item);

    if (items.size() >= 1)
        iIndex = 0;

    iBottomStop = items.size() - iNumLines + iTopStop;
}

void MI_NetworkListScroll::Clear()
{
    items.clear();
    iIndex = 0;
    iSelectedLine = -1;
    iSelectedLineBackup = 0;
    iOffset = 0;
    *iRemoteIndex = 0;
}

// This function runs when the scroll gets focus.
MenuCodeEnum MI_NetworkListScroll::Modify(bool modify)
{
    fModifying = modify;
    if (iSelectedLine == -1)
        iSelectedLine = iSelectedLineBackup;

    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_NetworkListScroll::SendInput(CPlayerInput * playerInput)
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
            if (neighborControls[2]) {
                iSelectedLineBackup = iSelectedLine;
                iSelectedLine = -1;
                return MENU_CODE_UNSELECT_ITEM;
            }
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
            if (neighborControls[3]) {
                iSelectedLineBackup = iSelectedLine;
                iSelectedLine = -1;
                return MENU_CODE_UNSELECT_ITEM;
            }
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
            if (items.empty())
                return MENU_CODE_NONE;

            printf("Selected index: %d\n", *iRemoteIndex);
            return iAcceptCode;
        }

        if (playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            return iCancelCode;
        }
    }

    return MENU_CODE_NONE;
}

void MI_NetworkListScroll::Update()
{}

void MI_NetworkListScroll::Draw()
{
    if (!fShow)
        return;

    //Draw the background for the map preview
    rm->menu_dialog.draw(ix, iy, 0, 0, iWidth - 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix + iWidth - 16, iy, 496, 0, 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix, iy + iNumLines * 32 + 32, 0, 464, iWidth - 16, 16);
    rm->menu_dialog.draw(ix + iWidth - 16, iy + iNumLines * 32 + 32, 496, 464, 16, 16);

    rm->menu_font_large.drawCentered(ix + (iWidth >> 1), iy + 5, sTitle.c_str());

    //Draw each filter field
    for (short iLine = 0; iLine < iNumLines && (unsigned short)iLine < items.size(); iLine++) {
        short iHalfLineWidth = (iWidth - 32) >> 1;
        short iLineWidth = iWidth - 32;
        spr->draw(ix + 16, iy + 32 + iLine * 32, 0, (iSelectedLine == iLine ? 32 : 0), iHalfLineWidth, 32);
        spr->draw(ix + 16 + iHalfLineWidth, iy + 32 + iLine * 32, 512 - iLineWidth + iHalfLineWidth, (iSelectedLine == iLine ? 32 : 0), iLineWidth - iHalfLineWidth, 32);

        rm->menu_font_large.drawChopRight(ix + 28, iy + 5 + iLine * 32 + 32, iWidth - 104, items[iOffset + iLine].sLeft.c_str());
        rm->menu_font_large.drawRightJustified(ix + 28 + iWidth - 64, iy + 5 + iLine * 32 + 32, items[iOffset + iLine].sRight.c_str());
    }
}

bool MI_NetworkListScroll::MoveNext()
{
    if (items.empty())
        return false;

    if (iIndex == items.size() - 1)
        return false;

    iIndex++;
    *iRemoteIndex = iIndex;

    if (iIndex > iTopStop && iIndex <= iBottomStop)
        iOffset++;
    else
        iSelectedLine++;

    return true;
}

bool MI_NetworkListScroll::MovePrev()
{
    if (items.empty())
        return false;

    if (iIndex == 0)
        return false;

    iIndex--;
    *iRemoteIndex = iIndex;

    if (iIndex >= iTopStop && iIndex < iBottomStop)
        iOffset--;
    else
        iSelectedLine--;

    return true;
}

void MI_NetworkListScroll::RemoteIndex(unsigned short * index)
{
    iRemoteIndex = index;
}
