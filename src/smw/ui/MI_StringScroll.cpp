#include "MI_StringScroll.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern short LookupTeamID(short id);

StringScrollElement::StringScrollElement(const std::string& text)
    : str(text)
{}

void StringScrollElement::draw(int x, int y, unsigned max_w) {
    rm->menu_font_large.drawChopRight(x + 28, y + 5, max_w, str.c_str());
}



MI_StringScroll::MI_StringScroll(gfxSprite * nspr, short x, short y, short width, short numlines)
    : UI_Control(x, y)
    , spr(nspr)
    , iNumLines(numlines)
    , iSelectedLine(-1)
    , iIndex(0)
    , iWidth(width)
    , iOffset(0)
    , iTopStop((numlines - 1) >> 1)
    , iBottomStop(0)
    , sTitle("")
    , iAcceptCode(MENU_CODE_NONE)
    , iCancelCode(MENU_CODE_NONE)
{}

MI_StringScroll::~MI_StringScroll()
{}

void MI_StringScroll::Add(const std::string& item)
{
    items.push_back(StringScrollElement(item));

    current = items.begin();
    iIndex = 0;

    iBottomStop = items.size() - iNumLines + iTopStop;
}

void MI_StringScroll::ClearItems()
{
    items.clear();
    current = items.begin();
    iIndex = 0;
    iBottomStop = 0;
    iSelectedLine = -1;
    iOffset = 0;
}

void MI_StringScroll::SetAcceptCode(const MenuCodeEnum code) {
    iAcceptCode = code;
}

void MI_StringScroll::SetCancelCode(const MenuCodeEnum code) {
    iCancelCode = code;
}

void MI_StringScroll::Activate() {
    iSelectedLine = 0;
}

void MI_StringScroll::Deactivate() {
    iSelectedLine = -1;
}

unsigned short MI_StringScroll::CurrentIndex() const {
    return iIndex;
}

MenuCodeEnum MI_StringScroll::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_StringScroll::SendInput(CPlayerInput * playerInput)
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
            if (neighborControls[2])
                return MENU_CODE_UNSELECT_ITEM;

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
            if (neighborControls[3])
                return MENU_CODE_UNSELECT_ITEM;

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
            return iAcceptCode;
        }

        if (playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            return iCancelCode;
        }
    }

    return MENU_CODE_NONE;
}


void MI_StringScroll::Update()
{}

void MI_StringScroll::Draw()
{
    if (!fShow)
        return;

    rm->menu_dialog.draw(ix, iy, 0, 0, iWidth - 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix + iWidth - 16, iy, 496, 0, 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix, iy + iNumLines * 32 + 32, 0, 464, iWidth - 16, 16);
    rm->menu_dialog.draw(ix + iWidth - 16, iy + iNumLines * 32 + 32, 496, 464, 16, 16);

    rm->menu_font_large.drawCentered(ix + (iWidth >> 1), iy + 5, sTitle.c_str());

    //Draw each filter field
    for (short iLine = 0; iLine < iNumLines && (unsigned short)iLine < items.size(); iLine++) {
        short iLineWidth = iWidth - 32;
        short iHalfLineWidth = iLineWidth >> 1;
        spr->draw(ix + 16, iy + 32 + iLine * 32, 0, (iSelectedLine == iLine ? 32 : 0), iHalfLineWidth, 32);
        spr->draw(ix + 16 + iHalfLineWidth, iy + 32 + iLine * 32, 512 - iLineWidth + iHalfLineWidth, (iSelectedLine == iLine ? 32 : 0), iLineWidth - iHalfLineWidth, 32);

        items[iOffset + iLine].draw(ix, 32 + iy + iLine * 32, iWidth - 104);
    }
}

bool MI_StringScroll::MoveNext()
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

bool MI_StringScroll::MovePrev()
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

    return true;
}
