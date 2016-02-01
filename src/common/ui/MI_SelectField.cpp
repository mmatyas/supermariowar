#include "MI_SelectField.h"

#include "ResourceManager.h"
extern CResourceManager* rm;

#include <cstring>

/**************************************
 * MI_SelectField Class
 **************************************/

MI_SelectField::MI_SelectField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent) :
    UI_Control(x, y)
{
    spr = nspr;

    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iWidth = width;
    iIndent = indent;

    mcItemChangedCode = MENU_CODE_NONE;
    mcControlSelectedCode = MENU_CODE_NONE;

    fAutoAdvance = false;
    fNoWrap = false;

    iValue = NULL;
    sValue = NULL;
    fValue = NULL;

    iIndex = 0;

    miModifyImageLeft = new MI_Image(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = new MI_Image(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);

    iAdjustmentY = width > 256 ? 0 : 128;

    fFastScroll = false;
}

MI_SelectField::MI_SelectField(const MI_SelectField& other)
    : UI_Control(other)
{
    spr = other.spr;
    szName = new char[strlen(other.szName) + 1];
    strcpy(szName, other.szName);

    SetData(other.iValue, other.sValue, other.fValue);

    for (unsigned i = 0; i < other.items.size(); i++)
        items.push_back(new SF_ListItem(*other.items[i]));

    SetIndex(other.iIndex); // sets current and index

    for (unsigned i = 0; i < other.goodRandomItems.size(); i++)
        goodRandomItems.push_back(new SF_ListItem(*other.goodRandomItems[i]));

    iWidth = other.iWidth;
    iIndent = other.iIndent;

    miModifyImageLeft = new MI_Image(spr, ix + iIndent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = new MI_Image(spr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);

    mcItemChangedCode = other.mcItemChangedCode;
    mcControlSelectedCode = other.mcControlSelectedCode;

    fAutoAdvance = other.fAutoAdvance;
    fNoWrap = other.fNoWrap;
    iAdjustmentY = other.iAdjustmentY;
    fFastScroll = other.fFastScroll;
}

MI_SelectField::~MI_SelectField()
{
    delete [] szName;

    delete miModifyImageLeft;
    delete miModifyImageRight;

    goodRandomItems.clear();

    std::vector<SF_ListItem*>::iterator iterateAll = items.begin(), lim = items.end();

    while (iterateAll != lim) {
        delete *iterateAll;
        iterateAll++;
    }

    items.clear();
}

void MI_SelectField::SetTitle(char * name)
{
    delete [] szName;
    szName = new char[strlen(name) + 1];
    strcpy(szName, name);
}

//Sets current selected item based on the items int value
bool MI_SelectField::SetKey(short iID)
{
    if (items.empty())
        return false;

    std::vector<SF_ListItem*>::iterator search = items.begin();

    iIndex  = 0;
    while (search != items.end()) {
        if ((*search)->iValue == iID) {
            current = search;
            SetValues();
            return true;
        }

        ++search;
        ++iIndex;
    }

    return false;
}

bool MI_SelectField::SetIndex(unsigned short index)
{
    if (index >= items.size())
        return false;

    current = items.begin();

    for (unsigned int iSearch = 0; iSearch < index; iSearch++) {
        ++current;
    }

    iIndex = index;

    SetValues();

    return true;
}

void MI_SelectField::Add(std::string name, short ivalue, std::string svalue, bool fvalue, bool fhidden, bool fGoodRandom, short iIconOverride)
{
    SF_ListItem * item = new SF_ListItem(name, ivalue, svalue, fvalue, fhidden, iIconOverride);
    items.push_back(item);

    if (fGoodRandom)
        goodRandomItems.push_back(item);

    if (items.size() >= 1) {
        current = items.begin();
        iIndex = 0;
    }
}

bool MI_SelectField::HideItem(short iID, bool fhide)
{
    if (items.empty())
        return false;

    std::vector<SF_ListItem*>::iterator search = items.begin(), lim = items.end();

    while (search != lim) {
        if ((*search)->iValue == iID) {
            if (current == search && fhide) {
                if (!MoveNext())
                    MovePrev();
            }

            (*search)->fHidden = fhide;
            return true;
        }

        ++search;
    }

    return false;
}

void MI_SelectField::HideAllItems(bool fHide)
{
    if (items.empty())
        return;

    std::vector<SF_ListItem*>::iterator itr = items.begin();

    while (itr != items.end()) {
        (*itr)->fHidden = fHide;
        ++itr;
    }
}

MenuCodeEnum MI_SelectField::Modify(bool modify)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    if (fAutoAdvance && modify) {
        if (!items.empty()) {
            if (current == --items.end()) {
                current = items.begin();
                iIndex = 0;
            } else {
                ++current;
                ++iIndex;
            }

            //Saves the values to the pointed to values
            SetValues();
        }

        return mcItemChangedCode;
    }

    if (MENU_CODE_NONE != mcControlSelectedCode)
        return mcControlSelectedCode;

    miModifyImageLeft->Show(modify);
    miModifyImageRight->Show(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_SelectField::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        short iNumMoves = 1;
        if (fFastScroll && playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
            iNumMoves = 10;

        if (playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed) {
            bool fMoved = false;

            for (short iMove = 0; iMove < iNumMoves; iMove++)
                fMoved |= MoveNext();

            if (fMoved)
                return mcItemChangedCode;
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed) {
            bool fMoved = false;

            for (short iMove = 0; iMove < iNumMoves; iMove++)
                fMoved |= MovePrev();

            if (fMoved)
                return mcItemChangedCode;
        }

        if (playerInput->outputControls[iPlayer].menu_random.fPressed) {
            if (MoveRandom())
                return mcItemChangedCode;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImageLeft->Show(false);
            miModifyImageRight->Show(false);

            fModifying = false;

            SetValues();

            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}


void MI_SelectField::Update()
{
    miModifyImageRight->Update();
    miModifyImageLeft->Update();
}

void MI_SelectField::Draw()
{
    if (!fShow)
        return;

    if (iIndent == 0) {
        short iHalfWidth = iWidth >> 1;
        spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iHalfWidth, 32);
        spr->draw(ix + iHalfWidth, iy, 512 - iHalfWidth, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iHalfWidth, 32);
    } else {
        spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
        spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
        spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);
    }

    if (iIndent> 0)
        rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

	// RFC
    if (!items.empty()) {
        if (iIndent > 0)
            rm->menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, (*current)->sName.c_str());
        else
            rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iWidth - 32, (*current)->sName.c_str());
    }

    //TODO: invert order
	bool drawLeft = true;

	if ((items.begin() != items.end() && current == items.begin()))
		drawLeft = false;

	if (!fNoWrap || drawLeft)
        miModifyImageLeft->Draw();

    //TODO: invert order

	bool drawRight = true;

		if (items.end() != items.begin()) {

			if (items.begin() != --items.end()) {

				if (current == --items.end())
					drawRight = false;
			}
		}

    if (drawRight || !fNoWrap)
        miModifyImageRight->Draw();
}

void MI_SelectField::SetValues()
{
    if (iValue)
        *iValue = (*current)->iValue;

    if (sValue)
        *sValue = (*current)->sValue;

    if (fValue)
        *fValue = (*current)->fValue;
}

bool MI_SelectField::MoveNext()
{
    if (items.empty())
        return false;

    std::vector<SF_ListItem*>::iterator findNext = current;
    short iFindIndex = iIndex;

    while (true) {
        if (findNext == --items.end()) {
            if (fNoWrap) {
                return false;
            } else {
                findNext = items.begin();
                iFindIndex = 0;
            }
        } else {
            findNext++;
            iFindIndex++;
        }

        if (findNext == current)
            return false;

        if (!(*findNext)->fHidden) {
            current = findNext;
            iIndex = iFindIndex;
            SetValues();
            return true;
        }
    }

    return false;
}

bool MI_SelectField::MovePrev()
{
    if (items.empty())
        return false;

    std::vector<SF_ListItem*>::iterator findPrev = current;
    short iFindIndex = iIndex;

    while (true) {
        if (findPrev == items.begin()) {
            if (fNoWrap) {
                return false;
            } else {
                findPrev = --items.end();
                iFindIndex = (signed short)items.size() - 1;
            }
        } else {
            findPrev--;
            iFindIndex--;
        }

        if (findPrev == current)
            return false;

        if (!(*findPrev)->fHidden) {
            current = findPrev;
            iIndex = iFindIndex;
            SetValues();
            return true;
        }
    }

    return false;
}

bool MI_SelectField::MoveRandom()
{
    //Can't pick a random item from a list of 1
    if (items.size() <= 1)
        return false;

    std::vector<SF_ListItem*>::iterator searchHidden = items.begin();

    short iHiddenCount = 0;
    while (searchHidden != items.end()) {
        if ((*searchHidden)->fHidden) {
            iHiddenCount++;
        }

        ++searchHidden;
    }

    //Can't pick a random item from a list containing only 1 non-hidden item
    if (items.size() - iHiddenCount <= 1)
        return false;

    std::vector<SF_ListItem*>::iterator findRandom = current;
    short iFindIndex = iIndex;

    while (true) {
        int index = RANDOM_INT(items.size());

        for (int k = 0; k < index; k++) {
            if (findRandom == --items.end()) {
                findRandom = items.begin();
                iFindIndex = 0;
            } else {
                ++findRandom;
                ++iFindIndex;
            }
        }

        if (findRandom != current && !(*findRandom)->fHidden) {
            current = findRandom;
            iIndex = iFindIndex;
            SetValues();
            return true;
        }
    }

    SetValues();
    return true;
}

MenuCodeEnum MI_SelectField::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    //If we are modifying this control, see if we clicked on a next/prev button
    if (fModifying) {
        short x, y, w, h;
        miModifyImageLeft->GetPositionAndSize(&x, &y, &w, &h);

        if (iMouseX >= x && iMouseX < x + w &&
                iMouseY >= y && iMouseY < y + h) {
            if (MovePrev()) {
                if (mcItemChangedCode == MENU_CODE_NONE)
                    return MENU_CODE_CLICKED;

                return mcItemChangedCode;
            }
        }

        miModifyImageRight->GetPositionAndSize(&x, &y, &w, &h);

        if (iMouseX >= x && iMouseX < x + w &&
                iMouseY >= y && iMouseY < y + h) {
            if (MoveNext()) {
                if (mcItemChangedCode == MENU_CODE_NONE)
                    return MENU_CODE_CLICKED;

                return mcItemChangedCode;
            }
        }
    }

    //Otherwise just check to see if we clicked on the whole control
    if (iMouseX >= ix && iMouseX < ix + iWidth && iMouseY >= iy && iMouseY < iy + 32)
        return MENU_CODE_CLICKED;

    //Otherwise this control wasn't clicked at all
    return MENU_CODE_NONE;
}

void MI_SelectField::Refresh()
{
    if (iValue)
        SetKey(*iValue);
    else if (fValue)
        SetKey(*fValue ? 1 : 0);
}
