#include "uicontrol.h"

#include "GameValues.h"
#include "map.h" // MI_MapField
#include "MapList.h" // MI_MapField
#include "ResourceManager.h"
#include "objecthazard.h" // MI_MapField

#include <cmath>

// MI_MapField
extern SDL_Surface* blitdest;
extern CMap* g_map;
extern MapList *maplist;
extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];
extern void LoadCurrentMapBackground();
extern void LoadMapHazards(bool fPreview);

extern CGameValues game_values;
extern CResourceManager* rm;

#ifndef __EMSCRIPTEN__
    inline void smallDelay() { SDL_Delay(10); }
#else
    inline void smallDelay() {}
#endif


UI_Control::UI_Control(short x, short y)
{
    fSelected = false;
    fModifying = false;
    fAutoModify = false;
    fDisable = false;

    ix = x;
    iy = y;

    fShow = true;

    for (int iNeighbor = 0; iNeighbor < 4; iNeighbor++)
        neighborControls[iNeighbor] = NULL;

    uiMenu = NULL;

    iControllingTeam = -1;
}

UI_Control& UI_Control::operator= (const UI_Control& other)
{
    if (this != &other) {
        fSelected = other.fSelected;
        fModifying = other.fModifying;
        fAutoModify = other.fAutoModify;
        fDisable = other.fDisable;

        ix = other.ix;
        iy = other.iy;

        fShow = other.fShow;

        for (int iNeighbor = 0; iNeighbor < 4; iNeighbor++)
            neighborControls[iNeighbor] = NULL;

        uiMenu = NULL;
        iControllingTeam = other.iControllingTeam;
    }
    return *this;
}

UI_Control::UI_Control(const UI_Control& other)
{
    *this = other;
}

/**************************************
 * MI_IPField Class
 **************************************/
MI_IPField::MI_IPField(gfxSprite * nspr, short x, short y) :
    UI_Control(x, y)
{
    spr = nspr;

    iSelectedDigit = 0;

    for (short k = 0; k < 12; k++)
        values[k] = 0;

    iDigitPosition[0] = 16;
    iDigitPosition[1] = 34;
    iDigitPosition[2] = 52;

    iDigitPosition[3] = 80;
    iDigitPosition[4] = 98;
    iDigitPosition[5] = 116;

    iDigitPosition[6] = 144;
    iDigitPosition[7] = 162;
    iDigitPosition[8] = 180;

    iDigitPosition[9] = 208;
    iDigitPosition[10] = 226;
    iDigitPosition[11] = 244;

    miModifyImage = new MI_Image(nspr, ix + iDigitPosition[0] - 22, iy - 6, 0, 102, 60, 61, 4, 1, 8);
    miModifyImage->Show(false);
}

MI_IPField::~MI_IPField()
{
    delete miModifyImage;
}

char * MI_IPField::GetValue()
{
    sprintf(szValue, "%d%d%d.%d%d%d.%d%d%d.%d%d%d",
            values[0], values[1], values[2],
            values[3], values[4], values[5],
            values[6], values[7], values[8],
            values[9], values[10], values[11]);

    return szValue;
}

MenuCodeEnum MI_IPField::Modify(bool modify)
{
    miModifyImage->Show(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_IPField::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
            if (++iSelectedDigit > 11)
                iSelectedDigit = 0;

            MoveImage();
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed) {
            if (--iSelectedDigit < 0)
                iSelectedDigit = 11;

            MoveImage();
        }

        if (playerInput->outputControls[iPlayer].menu_up.fPressed) {
            values[iSelectedDigit]++;
            AssignHostAddress();
        }

        if (playerInput->outputControls[iPlayer].menu_down.fPressed) {
            values[iSelectedDigit]--;
            AssignHostAddress();
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImage->Show(false);
            fModifying = false;
            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}

void MI_IPField::AssignHostAddress()
{
    if (iSelectedDigit == 0 || iSelectedDigit == 3 || iSelectedDigit == 6 || iSelectedDigit == 9) {
        if (values[iSelectedDigit] > 2)
            values[iSelectedDigit] = 0;
        else if (values[iSelectedDigit] < 0)
            values[iSelectedDigit] = 2;
    } else {
        if (values[iSelectedDigit] > 9)
            values[iSelectedDigit] = 0;
        else if (values[iSelectedDigit] < 0)
            values[iSelectedDigit] = 9;
    }
}

void MI_IPField::MoveImage()
{
    miModifyImage->SetPosition(ix + iDigitPosition[iSelectedDigit] - 22, iy - 6);
}

void MI_IPField::Update()
{
    miModifyImage->Update();
}

void MI_IPField::Draw()
{
    if (!fShow)
        return;

    if (fSelected)
        spr->draw(ix, iy, 0, 51, 278, 51);
    else
        spr->draw(ix, iy, 0, 0, 278, 51);

    miModifyImage->Draw();

    for (int iSection = 0; iSection < 12; iSection += 3) {
        if (values[iSection] != 0)
            rm->menu_font_large.drawf(ix + iDigitPosition[iSection], iy + 12, "%i", values[iSection]);

        if (values[iSection] != 0 || values[iSection + 1] != 0)
            rm->menu_font_large.drawf(ix + iDigitPosition[iSection + 1], iy + 12, "%i", values[iSection + 1]);

        rm->menu_font_large.drawf(ix + iDigitPosition[iSection + 2], iy + 12, "%i", values[iSection + 2]);
    }
}

MenuCodeEnum MI_IPField::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    if (iMouseX >= ix && iMouseX < ix + 278 && iMouseY >= iy && iMouseY < iy + 51) {
        return MENU_CODE_CLICKED;
    }

    return MENU_CODE_NONE;
}

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

/**************************************
 * MI_ImageSelectField Class
 **************************************/

MI_ImageSelectField::MI_ImageSelectField(gfxSprite * nspr, gfxSprite * nspr_image, short x, short y, const char * name, short width, short indent, short imageHeight, short imageWidth) :
    MI_SelectField(nspr, x, y, name, width, indent)
{
    spr_image = nspr_image;

    iImageWidth = imageWidth;
    iImageHeight = imageHeight;
}

MI_ImageSelectField::MI_ImageSelectField(const MI_ImageSelectField& other)
    : MI_SelectField(other)
{
    spr_image = other.spr_image;
    iImageWidth = other.iImageWidth;
    iImageHeight = other.iImageHeight;
}

MI_ImageSelectField::~MI_ImageSelectField()
{}

void MI_ImageSelectField::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

    if (!items.empty()) {
        rm->menu_font_large.drawChopRight(ix + iIndent + iImageWidth + 10, iy + 5, iWidth - iIndent - 24, (*current)->sName.c_str());
    }

    spr_image->draw(ix + iIndent + 8, iy + 16 - (iImageHeight >> 1), ((*current)->iIconOverride >= 0 ? (*current)->iIconOverride : (*current)->iValue) * iImageWidth, 0, iImageWidth, iImageHeight);

    miModifyImageRight->Draw();
    miModifyImageLeft->Draw();
}


/**************************************
 * MI_SliderField Class
 **************************************/

MI_SliderField::MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, const char * name, short width, short indent1, short indent2) :
    MI_SelectField(nspr, x, y, name, width, indent1)
{
    iIndent2 = indent2;
    sprSlider = nsprSlider;

    SetPosition(x, y);
}

MI_SliderField::~MI_SliderField()
{}

void MI_SliderField::SetPosition(short x, short y)
{
    MI_SelectField::SetPosition(x, y);
    miModifyImageLeft->SetPosition(ix + iIndent - 26, iy + 4);
    miModifyImageRight->SetPosition(ix + iWidth - 16, iy + 4);
}

void MI_SliderField::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

    if (!items.empty()) {
        rm->menu_font_large.drawChopRight(ix + iIndent2 + 16, iy + 5, iWidth - iIndent2 - 24, (*current)->sName.c_str());
    }

    short iSpacing = (iIndent2 - iIndent - 20) / ((short)items.size() - 1);
    short iSpot = 0;

    for (unsigned int index = 0; index < items.size(); index++) {
        if (index < items.size() - 1)
            sprSlider->draw(ix + iIndent + iSpot + 16, iy + 10, 0, 0, iSpacing, 13);
        else
            sprSlider->draw(ix + iIndent + iSpot + 16, iy + 10, 164, 0, 4, 13);

        iSpot += iSpacing;
    }

    sprSlider->draw(ix + iIndent + (iIndex * iSpacing) + 14, iy + 8, 168, 0, 8, 16);

    if (current != items.begin() || !fNoWrap)
        miModifyImageLeft->Draw();

    if (current != --items.end() || !fNoWrap)
        miModifyImageRight->Draw();
}

MenuCodeEnum MI_SliderField::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_scrollfast.fPressed) {
            if (iIndex == 0)
                while (MoveNext());
            else
                while (MovePrev());

            return mcItemChangedCode;
        }
    }

    return MI_SelectField::SendInput(playerInput);
}


/**************************************
 * MI_PowerupSlider Class
 **************************************/

MI_PowerupSlider::MI_PowerupSlider(gfxSprite * nspr, gfxSprite * nsprSlider, gfxSprite * nsprPowerup, short x, short y, short width, short powerupIndex) :
    MI_SliderField(nspr, nsprSlider, x, y, "", width, 0, 0)
{
    sprPowerup = nsprPowerup;
    iPowerupIndex = powerupIndex;

    miModifyImageLeft->SetPosition(ix + 25, iy + 4);
    miModifyImageRight->SetPosition(ix + iWidth - 12, iy + 4);

    iHalfWidth = (width - 38) >> 1;
}

MI_PowerupSlider::~MI_PowerupSlider()
{}

void MI_PowerupSlider::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix + 38, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iHalfWidth, 32);
    spr->draw(ix + 38 + iHalfWidth, iy, 550 - iWidth + iHalfWidth, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iHalfWidth - 38, 32);

    short iSpacing = (iWidth - 100) / ((short)items.size() - 1);
    short iSpot = 0;

    for (unsigned int index = 0; index < items.size(); index++) {
        if (index < items.size() - 1)
            sprSlider->draw(ix + iSpot + 56, iy + 10, 0, 0, iSpacing, 13);
        else
            sprSlider->draw(ix + iSpot + 56, iy + 10, 164, 0, 4, 13);

        iSpot += iSpacing;
    }

    sprSlider->draw(ix + (iIndex * iSpacing) + 54, iy + 8, 168, 0, 8, 16);

    sprSlider->draw(ix + iWidth - 34, iy + 8, iIndex * 16, 16, 16, 16);

    sprPowerup->draw(ix, iy, iPowerupIndex * 32, 0, 32, 32);

    if (current != items.begin() || !fNoWrap)
        miModifyImageLeft->Draw();

    if (current != --items.end() || !fNoWrap)
        miModifyImageRight->Draw();
}

/**************************************
 * MI_FrenzyModeOptions Class
 **************************************/

//Rearrange display of powerups
short iFrenzyCardPositionMap[NUMFRENZYCARDS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

MI_FrenzyModeOptions::MI_FrenzyModeOptions(short x, short y, short width, short numlines) :
    UI_Control(x, y)
{
    iWidth = width;
    iNumLines = numlines;

    iIndex = 0;
    iOffset = 0;

    iTopStop = ((iNumLines - 1) >> 1) + 3;  // Plus 3 for the 3 fields at the top
    iBottomStop = ((NUMFRENZYCARDS + 1) >> 1) - iNumLines + iTopStop;

    mMenu = new UI_Menu();

    miQuantityField = new MI_SelectField(&rm->spr_selectfield, 120, 40, "Limit", 400, 180);
    miQuantityField->Add("Single Powerup", 0, "", false, false);
    miQuantityField->Add("1 Powerup", 1, "", false, false);
    miQuantityField->Add("2 Powerups", 2, "", false, false);
    miQuantityField->Add("3 Powerups", 3, "", false, false);
    miQuantityField->Add("4 Powerups", 4, "", false, false);
    miQuantityField->Add("5 Powerups", 5, "", false, false);
    miQuantityField->Add("# Players - 1", 6, "", false, false);
    miQuantityField->Add("# Players", 7, "", false, false);
    miQuantityField->Add("# Players + 1", 8, "", false, false, false);
    miQuantityField->Add("# Players + 2", 9, "", false, false, false);
    miQuantityField->Add("# Players + 3", 10, "", false, false, false);
    miQuantityField->SetData(&game_values.gamemodemenusettings.frenzy.quantity, NULL, NULL);
    miQuantityField->SetKey(game_values.gamemodemenusettings.frenzy.quantity);

    miRateField = new MI_SelectField(&rm->spr_selectfield, 120, 80, "Rate", 400, 180);
    miRateField->Add("Instant", 0, "", false, false);
    miRateField->Add("1 Second", 62, "", false, false);
    miRateField->Add("2 Seconds", 124, "", false, false);
    miRateField->Add("3 Seconds", 186, "", false, false);
    miRateField->Add("5 Seconds", 310, "", false, false);
    miRateField->Add("10 Seconds", 620, "", false, false, false);
    miRateField->Add("15 Seconds", 930, "", false, false, false);
    miRateField->Add("20 Seconds", 1240, "", false, false, false);
    miRateField->Add("25 Seconds", 1550, "", false, false, false);
    miRateField->Add("30 Seconds", 1860, "", false, false, false);
    miRateField->SetData(&game_values.gamemodemenusettings.frenzy.rate, NULL, NULL);
    miRateField->SetKey(game_values.gamemodemenusettings.frenzy.rate);

    miStoredShellsField = new MI_SelectField(&rm->spr_selectfield, 120, 120, "Store Shells", 400, 180);
    miStoredShellsField->Add("Off", 0, "", false, false);
    miStoredShellsField->Add("On", 1, "", true, false);
    miStoredShellsField->SetData(NULL, NULL, &game_values.gamemodemenusettings.frenzy.storedshells);
    miStoredShellsField->SetKey(game_values.gamemodemenusettings.frenzy.storedshells ? 1 : 0);
    miStoredShellsField->SetAutoAdvance(true);

    short iPowerupMap[] = {8, 5, 11, 17, 19, 21, 23, 24, 25, 20, 9, 16, 10, 22, 12, 13, 14, 15, 27};
    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        miPowerupSlider[iPowerup] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->spr_storedpoweruplarge, iPowerup < 10 ? 65 : 330, 0, 245, iPowerupMap[iPowerup]);
        miPowerupSlider[iPowerup]->Add("", 0, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 1, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 2, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 3, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 4, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 5, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 6, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 7, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 8, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 9, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 10, "", false, false);
        miPowerupSlider[iPowerup]->SetNoWrap(true);
        miPowerupSlider[iPowerup]->SetData(&game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup], NULL, NULL);
        miPowerupSlider[iPowerup]->SetKey(game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup]);
    }

    miBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miUpArrow = new MI_Image(&rm->menu_verticalarrows, 310, 162, 20, 0, 20, 20, 1, 4, 8);
    miDownArrow = new MI_Image(&rm->menu_verticalarrows, 310, 402, 0, 0, 20, 20, 1, 4, 8);
    miUpArrow->Show(false);

    mMenu->AddControl(miQuantityField, NULL, miRateField, NULL, NULL);
    mMenu->AddControl(miRateField, miQuantityField, miStoredShellsField, NULL, NULL);
    mMenu->AddControl(miStoredShellsField, miRateField, miPowerupSlider[0], NULL, NULL);

    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        UI_Control * upcontrol = NULL;
        if (iPowerup == 0)
            upcontrol = miStoredShellsField;
        else
            upcontrol = miPowerupSlider[iPowerup - 2];

        UI_Control * lDownControl1 = NULL;
        if (iPowerup >= NUMFRENZYCARDS - 2)
            lDownControl1 = miBackButton;
        else
            lDownControl1 = miPowerupSlider[iPowerup + 2];

        UI_Control * rightcontrol = NULL;
        if (iPowerup + 1 < NUMFRENZYCARDS)
            rightcontrol = miPowerupSlider[iPowerup + 1];
        else
            rightcontrol = miBackButton;

        mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, lDownControl1, NULL, rightcontrol);

        if (++iPowerup < NUMFRENZYCARDS) {
            upcontrol = NULL;
            if (iPowerup == 1)
                upcontrol = miStoredShellsField;
            else
                upcontrol = miPowerupSlider[iPowerup - 2];

            UI_Control * lDownControl2 = NULL;
            if (iPowerup >= NUMFRENZYCARDS - 2)
                lDownControl2 = miBackButton;
            else
                lDownControl2 = miPowerupSlider[iPowerup + 2];

            mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, lDownControl2, miPowerupSlider[iPowerup - 1], NULL);
        }
    }

    //Setup positions and visible powerups
    SetupPowerupFields();

    mMenu->AddNonControl(miUpArrow);
    mMenu->AddNonControl(miDownArrow);

    mMenu->AddControl(miBackButton, miPowerupSlider[NUMFRENZYCARDS - 1], NULL, miPowerupSlider[NUMFRENZYCARDS - 1], NULL);

    mMenu->SetHeadControl(miQuantityField);
    mMenu->SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);
}

MI_FrenzyModeOptions::~MI_FrenzyModeOptions()
{
    delete mMenu;
}

void MI_FrenzyModeOptions::SetupPowerupFields()
{
    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        short iPosition = iFrenzyCardPositionMap[iPowerup];
        MI_PowerupSlider * slider = miPowerupSlider[iPosition];

        if ((iPosition >> 1) < iOffset || (iPosition >> 1) >= iOffset + iNumLines)
            slider->Show(false);
        else {
            slider->Show(true);
            slider->SetPosition(ix + (iPosition % 2) * 295, iy + 118 + 38 * (iPosition / 2 - iOffset));
        }
    }
}

MenuCodeEnum MI_FrenzyModeOptions::Modify(bool modify)
{
    mMenu->ResetMenu();
    iOffset = 0;
    iIndex = 0;
    SetupPowerupFields();
    AdjustDisplayArrows();

    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_FrenzyModeOptions::SendInput(CPlayerInput * playerInput)
{
    UI_Control * prevControl = mMenu->GetCurrentControl();

    MenuCodeEnum ret = mMenu->SendInput(playerInput);

    UI_Control * nextControl = mMenu->GetCurrentControl();

    if (MENU_CODE_CANCEL_INPUT == ret) {
        fModifying = false;
        return MENU_CODE_UNSELECT_ITEM;
    } else if (MENU_CODE_NEIGHBOR_UP == ret) {
        if (prevControl != miBackButton)
            MovePrev();
    } else if (MENU_CODE_NEIGHBOR_DOWN == ret) {
        if (nextControl != miBackButton || prevControl == miPowerupSlider[NUMFRENZYCARDS - 2])
            MoveNext();
    }

    return ret;
}

void MI_FrenzyModeOptions::Update()
{
    mMenu->Update();
}

void MI_FrenzyModeOptions::Draw()
{
    if (!fShow)
        return;

    mMenu->Draw();
}

void MI_FrenzyModeOptions::MoveNext()
{
    iIndex++;

    if (iIndex > iTopStop && iIndex <= iBottomStop) {
        iOffset++;
        SetupPowerupFields();
    }

    AdjustDisplayArrows();
}

void MI_FrenzyModeOptions::MovePrev()
{
    iIndex--;

    if (iIndex >= iTopStop && iIndex < iBottomStop) {
        iOffset--;
        SetupPowerupFields();
    }

    AdjustDisplayArrows();
}

void MI_FrenzyModeOptions::SetRandomGameModeSettings()
{
    game_values.gamemodesettings.frenzy.quantity = miQuantityField->GetRandomShortValue();
    game_values.gamemodesettings.frenzy.rate = miRateField->GetRandomShortValue();
    game_values.gamemodesettings.frenzy.storedshells = miStoredShellsField->GetRandomBoolValue();

    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
        game_values.gamemodesettings.frenzy.powerupweight[iPowerup] = miPowerupSlider[iPowerup]->GetRandomShortValue();
}

void MI_FrenzyModeOptions::AdjustDisplayArrows()
{
    if (iIndex > iTopStop)
        miUpArrow->Show(true);
    else
        miUpArrow->Show(false);

    if (iIndex < iBottomStop)
        miDownArrow->Show(true);
    else
        miDownArrow->Show(false);
}

MenuCodeEnum MI_FrenzyModeOptions::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    //Loop through all controls to see if one was clicked on
    MenuCodeEnum ret = mMenu->MouseClick(iMouseX, iMouseY);

    if (ret == MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS)
        return ret;

    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        MI_PowerupSlider * slider = miPowerupSlider[iPowerup];

        if (slider == mMenu->GetCurrentControl()) {
            iIndex = (iPowerup >> 1) + 3;

            if (iIndex <= iTopStop)
                iOffset = 0;
            else if (iIndex >= iBottomStop)
                iOffset = iBottomStop - iTopStop;
            else
                iOffset = iIndex - iTopStop;

            SetupPowerupFields();
            AdjustDisplayArrows();

            return ret;
        }
    }

    iOffset = 0;
    iIndex = 0;
    SetupPowerupFields();
    AdjustDisplayArrows();

    return ret;
}

void MI_FrenzyModeOptions::Refresh()
{
    mMenu->Refresh();
}

/**************************************
 * MI_Button Class
 **************************************/

MI_Button::MI_Button(gfxSprite * nspr, short x, short y, const char * name, short width, short justified) :
    UI_Control(x, y)
{
    spr = nspr;

    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iWidth = width;
    iTextJustified = justified;
    fSelected = false;
    menuCode = MENU_CODE_NONE;

    sprImage = NULL;
    iImageSrcX = 0;
    iImageSrcY = 0;
    iImageW = 0;
    iImageH = 0;

    iTextW = (short)rm->menu_font_large.getWidth(name);

    iAdjustmentY = width > 256 ? 0 : 128;
    iHalfWidth = width >> 1;
}

MenuCodeEnum MI_Button::Modify(bool)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    return menuCode;
}

MenuCodeEnum MI_Button::SendInput(CPlayerInput *)
{
    //If input is being sent, that means the button is selected i.e. clicked
    return menuCode;
}

void MI_Button::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iHalfWidth, 32);
    spr->draw(ix + iHalfWidth, iy, 512 - iWidth + iHalfWidth, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iHalfWidth, 32);

    if (0 == iTextJustified) {
        rm->menu_font_large.drawChopRight(ix + 16 + (iImageW > 0 ? iImageW + 2 : 0), iy + 5, iWidth - 32, szName);

        if (sprImage)
            sprImage->draw(ix + 16, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
    } else if (1 == iTextJustified) {
        rm->menu_font_large.drawCentered(ix + ((iWidth + (iImageW > 0 ? iImageW + 2 : 0)) >> 1), iy + 5, szName);

        if (sprImage)
            sprImage->draw(ix + (iWidth >> 1) - ((iTextW + iImageW) >> 1) - 1, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
    } else {
        rm->menu_font_large.drawRightJustified(ix + iWidth - 16, iy + 5, szName);

        if (sprImage)
            sprImage->draw(ix + iWidth - 18 - iTextW - iImageW, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
    }
}

void MI_Button::SetName(const char * name)
{
    delete [] szName;
    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iTextW = (short)rm->menu_font_large.getWidth(name);
}

void MI_Button::SetImage(gfxSprite * nsprImage, short x, short y, short w, short h)
{
    sprImage = nsprImage;
    iImageSrcX = x;
    iImageSrcY = y;
    iImageW = w;
    iImageH = h;
}

MenuCodeEnum MI_Button::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    if (iMouseX >= ix && iMouseX < ix + iWidth && iMouseY >= iy && iMouseY < iy + 32) {
        return menuCode;
    }

    return MENU_CODE_NONE;
}

/**************************************
 * MI_Image Class
 **************************************/
MI_Image::MI_Image(gfxSprite * nspr, short x, short y, short srcx, short srcy, short w, short h, short numxframes, short numyframes, short speed) :
    UI_Control(x, y)
{
    spr = nspr;
    isrcx = srcx;
    isrcy = srcy;
    iw = w;
    ih = h;

    iNumXFrames = numxframes;
    iNumYFrames = numyframes;
    iSpeed = speed;

    iTimer = 0;
    iXFrame = srcx;
    iYFrame = srcy;

    fPulse = false;
    iPulseValue = 0;
    fPulseOut = true;
    iPulseDelay = 0;

    fSwirl = false;
    dSwirlRadius = 0.0f;
    dSwirlAngle = 0.0f;
    dSwirlRadiusSpeed = 0.0f;
    dSwirlAngleSpeed = 0.0f;

    fBlink = false;
    iBlinkInterval = 0;
    iBlinkCounter = 0;
    fBlinkShow = true;
}

MI_Image::~MI_Image()
{}

void MI_Image::Update()
{
    if (!fShow)
        return;

    if (iSpeed > 0 && ++iTimer >= iSpeed) {
        iTimer = 0;
        iXFrame += iw;

        if (iXFrame >= iNumXFrames * iw + isrcx) {
            iXFrame = isrcx;
            iYFrame += ih;

            if (iYFrame >= iNumYFrames * ih + isrcy) {
                iYFrame = isrcy;
            }
        }
    }

    if (fPulse) {
        if (++iPulseDelay >= 3) {
            iPulseDelay = 0;

            if (fPulseOut) {
                if (++iPulseValue >= 10) {
                    fPulseOut = false;
                }
            } else {
                if (--iPulseValue <= 0) {
                    fPulseOut = true;
                }
            }
        }
    }

    if (fSwirl) {
        dSwirlRadius -= dSwirlRadiusSpeed;

        if (dSwirlRadius <= 0.0f) {
            fSwirl = false;
        } else {
            dSwirlAngle += dSwirlAngleSpeed;
        }
    }

    if (fBlink) {
        if (++iBlinkCounter > iBlinkInterval) {
            fBlinkShow = !fBlinkShow;
            iBlinkCounter = 0;
        }
    }
}

void MI_Image::Draw()
{
    if (!fShow || (fBlink && !fBlinkShow))
        return;

    short iXOffset = 0;
    short iYOffset = 0;

    if (fSwirl) {
        iXOffset = (short)(dSwirlRadius * cos(dSwirlAngle));
        iYOffset = (short)(dSwirlRadius * sin(dSwirlAngle));
    }

    if (fPulse)
        spr->drawStretch(ix - iPulseValue + iXOffset, iy - iPulseValue + iYOffset, iw + (iPulseValue << 1), ih + (iPulseValue << 1), iXFrame, iYFrame, iw, ih);
    else
        spr->draw(ix + iXOffset, iy + iYOffset, iXFrame, iYFrame, iw, ih);
}


/**************************************
 * MI_Text Class
 **************************************/

MI_Text::MI_Text(const char * text, short x, short y, short w, short size, short justified) :
    UI_Control(x, y)
{
    szText = new char[strlen(text) + 1];
    strcpy(szText, text);

    iw = w;
    iJustified = justified;

    if (size == 0)
        font = &rm->menu_font_small;
    else
        font = &rm->menu_font_large;
}

MI_Text::~MI_Text()
{}

void MI_Text::SetText(const char * text)
{
    delete [] szText;
    szText = new char[strlen(text) + 1];
    strcpy(szText, text);
}

void MI_Text::Draw()
{
    if (!fShow)
        return;

    if (iJustified == 0 && iw == 0)
        font->draw(ix, iy, szText);
    else if (iJustified == 0)
        font->drawChopRight(ix, iy, iw, szText);
    else if (iJustified == 1)
        font->drawCentered(ix, iy, szText);
    else if (iJustified == 2)
        font->drawRightJustified(ix, iy, szText);
}

/**************************************
 * MI_ScoreText Class
 **************************************/

MI_ScoreText::MI_ScoreText(short x, short y) :
    UI_Control(x, y)
{
    iScore = 0;

    iDigitLeftSrcX = 0;
    iDigitMiddleSrcX = 0;
    iDigitRightSrcX = 0;

    iDigitLeftDstX = 0;
    iDigitMiddleDstX = 0;
    iDigitRightDstX = 0;
}

void MI_ScoreText::Draw()
{
    if (!fShow)
        return;

    rm->spr_scoretext.draw(iDigitRightDstX, iy, iDigitRightSrcX, 0, 16, 16);

    if (iDigitLeftSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, iy, iDigitMiddleSrcX, 0, 16, 16);
        rm->spr_scoretext.draw(iDigitLeftDstX, iy, iDigitLeftSrcX, 0, 16, 16);
    } else if (iDigitMiddleSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, iy, iDigitMiddleSrcX, 0, 16, 16);
    }
}

void MI_ScoreText::SetScore(short sScore)
{
    short iDigits = sScore;
    while (iDigits > 999)
        iDigits -= 1000;

    iDigitLeftSrcX = iDigits / 100 * 16;
    iDigitMiddleSrcX = iDigits % 100 / 10 * 16;
    iDigitRightSrcX = iDigits % 10 * 16;

    if (iDigitLeftSrcX == 0) {
        if (iDigitMiddleSrcX == 0) {
            iDigitRightDstX = ix - 8;
        } else {
            iDigitMiddleDstX = ix - 16;
            iDigitRightDstX = ix;
        }
    } else {
        iDigitLeftDstX = ix - 24;
        iDigitMiddleDstX = ix - 8;
        iDigitRightDstX = ix + 8;
    }
}


/**************************************
 * MI_TextField Class
 **************************************/

MI_TextField::MI_TextField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent) :
    UI_Control(x, y)
{
    spr = nspr;

    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iWidth = width;
    iIndent = indent;

    mcItemChangedCode = MENU_CODE_NONE;
    mcControlSelectedCode = MENU_CODE_NONE;

    szValue = NULL;

    miModifyCursor = new MI_Image(nspr, ix + indent, iy + 4, 136, 64, 15, 24, 4, 1, 8);
    miModifyCursor->SetBlink(true, 20);
    miModifyCursor->Show(false);

    iAdjustmentY = width > 256 ? 0 : 128;

    iMaxChars = 0;
    iNumChars = 0;
    iCursorIndex = 0;

    szTempValue = NULL;

    iStringWidth = 0;
    iAllowedWidth = iWidth - iIndent - 24;
}

MI_TextField::~MI_TextField()
{
    delete miModifyCursor;
    delete [] szName;
    delete [] szTempValue;
}

void MI_TextField::SetTitle(char * name)
{
    delete [] szName;
    szName = new char[strlen(name) + 1];
    strcpy(szName, name);
}

MenuCodeEnum MI_TextField::Modify(bool modify)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    if (MENU_CODE_NONE != mcControlSelectedCode)
        return mcControlSelectedCode;

    miModifyCursor->Show(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

short number_key_map[10] = {41, 33, 64, 35, 36, 37, 94, 38, 42, 40};

MenuCodeEnum MI_TextField::SendInput(CPlayerInput * playerInput)
{
#if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
    const Uint8 * keystate = SDL_GetKeyboardState(NULL);
#else
    Uint8 * keystate = SDL_GetKeyState(NULL);
#endif

    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyCursor->Show(false);

            fModifying = false;

            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    if (!szValue || iNumChars >= iMaxChars)
        return MENU_CODE_NONE;

    // TODO: check string conversion
    //Watch for characters typed in including delete and backspace
    SDL_KEYTYPE key = playerInput->iPressedKey;
    if ((key >= SDLK_a && key <= SDLK_z) || key == SDLK_SPACE || (key >= SDLK_0 && key <= SDLK_9) || key == SDLK_EQUALS ||
            key == SDLK_MINUS || key == SDLK_BACKQUOTE || (key >= SDLK_LEFTBRACKET && key <= SDLK_RIGHTBRACKET) ||
            key == SDLK_SEMICOLON || key == SDLK_QUOTE || key == SDLK_COMMA || key == SDLK_PERIOD || key == SDLK_SLASH) {
        if (iNumChars < iMaxChars - 1) {
            //Take care of holding shift to shift the pressed key to another character
            if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) {
                if (key >= SDLK_a && key <= SDLK_z) {
                    key -= 32;
                } else if (key >= SDLK_0 && key <= SDLK_9) {
                    key = number_key_map[key - 48];
                } else if (key == SDLK_MINUS) {
                    key = SDLK_UNDERSCORE;
                } else if (key == SDLK_EQUALS) {
                    key = SDLK_PLUS;
                } else if (key == SDLK_BACKQUOTE) {
                    key = 126;
                } else if (key >= SDLK_LEFTBRACKET && key <= SDLK_RIGHTBRACKET) {
                    key += 32;
                } else if (key == SDLK_SEMICOLON) {
                    key = SDLK_COLON;
                } else if (key == SDLK_QUOTE) {
                    key = SDLK_QUOTEDBL;
                } else if (key == SDLK_COMMA) {
                    key = SDLK_LESS;
                } else if (key == SDLK_PERIOD) {
                    key = SDLK_GREATER;
                } else if (key == SDLK_SLASH) {
                    key = SDLK_QUESTION;
                }
            }

            //Check to see if this is an allowed character for this field
            bool fAllowed = true;
            for (short iIndex = 0; iIndex < 32 && szDisallowedChars[iIndex] != 0; iIndex++) {
                if (szDisallowedChars[iIndex] == key) {
                    fAllowed = false;
                    break;
                }
            }

            //If it is an allowed character, then add it to the field
            if (fAllowed) {
                iNumChars++;

                for (short iCopy = iNumChars - 1; iCopy >= iCursorIndex; iCopy--) {
                    szValue[iCopy + 1] = szValue[iCopy];
                }

                szValue[iCursorIndex++] = (char)key;

                UpdateCursor();
                return mcItemChangedCode;
            }
        }
    } else if (key == SDLK_BACKSPACE) {
        if (iCursorIndex > 0) {
            iCursorIndex--;
            iNumChars--;
            for (short iCopy = iCursorIndex; iCopy < iNumChars; iCopy++) {
                szValue[iCopy] = szValue[iCopy + 1];
            }
            szValue[iNumChars] = 0;

            UpdateCursor();
            return mcItemChangedCode;
        }
    } else if (key == SDLK_DELETE) {
        if (iCursorIndex < iNumChars) {
            for (short iCopy = iCursorIndex; iCopy < iNumChars; iCopy++) {
                szValue[iCopy] = szValue[iCopy + 1];
            }

            szValue[--iNumChars] = 0;

            UpdateCursor();
            return mcItemChangedCode;
        }
    } else if (key == SDLK_LEFT) {
        if (iCursorIndex > 0) {
            iCursorIndex--;
            UpdateCursor();
        }
    } else if (key == SDLK_RIGHT) {
        if (iCursorIndex < iNumChars) {
            iCursorIndex++;
            UpdateCursor();
        }
    }

    return MENU_CODE_NONE;
}


void MI_TextField::Update()
{
    miModifyCursor->Update();
}

void MI_TextField::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

    if (szValue) {
        if (iStringWidth <= iAllowedWidth || !fModifying) {
            rm->menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iAllowedWidth, szValue);
        } else {
            rm->menu_font_large.drawChopLeft(ix + iWidth - 16, iy + 5, iAllowedWidth, szTempValue);
        }
    }

    miModifyCursor->Draw();
}

void MI_TextField::SetData(char * data, short maxchars)
{
    iMaxChars = maxchars;
    szValue = data;
    iCursorIndex = strlen(szValue);
    iNumChars = iCursorIndex;

    if (szTempValue)
        delete [] szTempValue;

    szTempValue = new char[iMaxChars];

    UpdateCursor();
}

MenuCodeEnum MI_TextField::MouseClick(short iMouseX, short iMouseY)
{
    if (!szValue || fDisable)
        return MENU_CODE_NONE;

    //If we are modifying this control, see if we clicked on a next/prev button
    if (fModifying) {
        //Move cursor to index in string where clicked
        short iPixelCount = 0;
        char szChar[2];
        szChar[1] = 0;
        for (short iChar = 0; iChar < iNumChars; iChar++) {
            szChar[0] = szValue[iChar];
            iPixelCount += rm->menu_font_large.getWidth(szChar);

            if (iPixelCount >= iMouseX - (ix + iIndent + 8)) {
                iCursorIndex = iChar;
                UpdateCursor();
                return MENU_CODE_NONE;
            }
        }
    }

    //Otherwise just check to see if we clicked on the whole control
    if (iMouseX >= ix && iMouseX < ix + iWidth && iMouseY >= iy && iMouseY < iy + 32) {
        iCursorIndex = strlen(szValue);
        UpdateCursor();
        return MENU_CODE_CLICKED;
    }

    //Otherwise this control wasn't clicked at all
    return MENU_CODE_NONE;
}

void MI_TextField::Refresh()
{
    //Look at destination string and update control based on that value
    if (!szValue)
        return;

    SetData(szValue, iMaxChars);
}

void MI_TextField::UpdateCursor()
{
    if (!szValue)
        return;

    strncpy(szTempValue, szValue, iCursorIndex);
    szTempValue[iCursorIndex] = 0;

    iStringWidth = rm->menu_font_large.getWidth(szTempValue);
    if (iStringWidth <= iAllowedWidth) {
        miModifyCursor->SetPosition(ix + iIndent + 10 + iStringWidth, iy + 4);
    } else {
        miModifyCursor->SetPosition(ix + iIndent + 10 + iAllowedWidth, iy + 4);
    }
}

void MI_TextField::SetDisallowedChars(const char * chars)
{
    strncpy(szDisallowedChars, chars, 31);
    szDisallowedChars[31] = 0;
}


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
