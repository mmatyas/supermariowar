#include "MI_SelectField.h"

#include "input.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

extern CResourceManager* rm;

#include <cstring>

/**************************************
 * MI_SelectField Class
 **************************************/

MI_SelectField::MI_SelectField(gfxSprite * nspr, short x, short y, std::string name, short width, short indent)
    : UI_Control(x, y)
    , spr(nspr)
    , szName(std::move(name))
    , iWidth(width)
    , iIndent(indent)
    , iAdjustmentY(width > 256 ? 0 : 128)
{
    miModifyImageLeft = std::make_unique<MI_Image>(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = std::make_unique<MI_Image>(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);
}

MI_SelectField::MI_SelectField(const MI_SelectField& other)
    : UI_Control(other)
{
    spr = other.spr;
    szName = other.szName;

    SetData(other.iValue, other.sValue, other.fValue);

    for (unsigned i = 0; i < other.items.size(); i++)
        items.push_back(new SF_ListItem(*other.items[i]));

    SetIndex(other.iIndex); // sets current and index

    for (unsigned i = 0; i < other.goodRandomItems.size(); i++)
        goodRandomItems.push_back(new SF_ListItem(*other.goodRandomItems[i]));

    iWidth = other.iWidth;
    iIndent = other.iIndent;

    miModifyImageLeft = std::make_unique<MI_Image>(spr, ix + iIndent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = std::make_unique<MI_Image>(spr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
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
    for (SF_ListItem* item : items) {
        delete item;
    }
}

void MI_SelectField::SetTitle(std::string name)
{
    szName = std::move(name);
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

void MI_SelectField::Add(std::string name, short ivalue)
{
    Add(std::move(name), ivalue, false);
}

void MI_SelectField::Add(std::string name, short ivalue, bool fhidden, bool fGoodRandom, short iIconOverride)
{
    SF_ListItem * item = new SF_ListItem(name, ivalue, fhidden, iIconOverride);
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
        rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName.c_str());

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
        miModifyImageLeft->GetPositionAndSize(x, y, w, h);

        if (iMouseX >= x && iMouseX < x + w &&
                iMouseY >= y && iMouseY < y + h) {
            if (MovePrev()) {
                if (mcItemChangedCode == MENU_CODE_NONE)
                    return MENU_CODE_CLICKED;

                return mcItemChangedCode;
            }
        }

        miModifyImageRight->GetPositionAndSize(x, y, w, h);

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


template<typename T>
MI_SelectFieldDyn<T>::MI_SelectFieldDyn(gfxSprite* nspr, short x, short y, std::string name, short width, short indent)
    : UI_Control(x, y)
    , m_spr(nspr)
    , m_name(std::move(name))
    , m_adjustmentY(width > 256 ? 0 : 128)
    , m_width(width)
    , m_indent(indent)
{
    miModifyImageLeft = std::make_unique<MI_Image>(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = std::make_unique<MI_Image>(nspr, ix + width - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);
}


template<typename T>
void MI_SelectFieldDyn<T>::Update()
{
    miModifyImageRight->Update();
    miModifyImageLeft->Update();
}


template<typename T>
void MI_SelectFieldDyn<T>::Draw()
{
    if (!fShow)
        return;

    if (m_indent == 0) {
        short iHalfWidth = m_width / 2;
        m_spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + m_adjustmentY, iHalfWidth, 32);
        m_spr->draw(ix + iHalfWidth, iy, 512 - iHalfWidth, (fSelected ? 32 : 0) + m_adjustmentY, m_width - iHalfWidth, 32);
    } else {
        m_spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + m_adjustmentY, m_indent - 16, 32);
        m_spr->draw(ix + m_indent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
        m_spr->draw(ix + m_indent + 16, iy, 528 - m_width + m_indent, (fSelected ? 32 : 0) + m_adjustmentY, m_width - m_indent - 16, 32);
    }

    if (m_indent> 0)
        rm->menu_font_large.drawChopRight(ix + 16, iy + 5, m_indent - 8, m_name.c_str());

    if (!m_items.empty()) {
        const short indent = (m_indent > 0) ? m_indent : 8;
        rm->menu_font_large.drawChopRight(ix + indent + 8, iy + 5, m_width - indent - 24, currentItem().name.c_str());
    }

    const bool drawLeft = m_index > 0;
    if (m_wraps || drawLeft)
        miModifyImageLeft->Draw();

    const bool drawRight = (m_index + 1) < m_items.size();
    if (m_wraps || drawRight)
        miModifyImageRight->Draw();
}


template<typename T>
MenuCodeEnum MI_SelectFieldDyn<T>::Modify(bool modify)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    if (m_autoAdvance && modify) {
        moveNext();
        return mcItemChangedCode;
    }

    if (MENU_CODE_NONE != mcControlSelectedCode)
        return mcControlSelectedCode;

    miModifyImageLeft->Show(modify);
    miModifyImageRight->Show(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}


template<typename T>
MenuCodeEnum MI_SelectFieldDyn<T>::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        short iNumMoves = 1;
        if (m_fastScroll && playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
            iNumMoves = 10;

        if (playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed) {
            bool fMoved = false;

            for (short iMove = 0; iMove < iNumMoves; iMove++)
                fMoved |= moveNext();

            if (fMoved)
                return mcItemChangedCode;
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed) {
            bool fMoved = false;

            for (short iMove = 0; iMove < iNumMoves; iMove++)
                fMoved |= movePrev();

            if (fMoved)
                return mcItemChangedCode;
        }

        if (playerInput->outputControls[iPlayer].menu_random.fPressed) {
            if (moveRandom())
                return mcItemChangedCode;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImageLeft->Show(false);
            miModifyImageRight->Show(false);

            fModifying = false;

            updateOutput();

            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}


template<typename T>
MenuCodeEnum MI_SelectFieldDyn<T>::MouseClick(short iMouseX, short iMouseY) {
    if (fDisable)
        return MENU_CODE_NONE;

    //If we are modifying this control, see if we clicked on a next/prev button
    if (fModifying) {
        short x, y, w, h;

        miModifyImageLeft->GetPositionAndSize(x, y, w, h);
        if (x <= iMouseX && iMouseX < x + w && y <= iMouseY && iMouseY < y + h) {
            if (movePrev()) {
                return mcItemChangedCode == MENU_CODE_NONE
                    ? MENU_CODE_CLICKED
                    : mcItemChangedCode;
            }
        }

        miModifyImageRight->GetPositionAndSize(x, y, w, h);
        if (x <= iMouseX && iMouseX < x + w && y <= iMouseY && iMouseY < y + h) {
            if (moveNext()) {
                return mcItemChangedCode == MENU_CODE_NONE
                    ? MENU_CODE_CLICKED
                    : mcItemChangedCode;
            }
        }
    }

    //Otherwise just check to see if we clicked on the whole control
    if (ix <= iMouseX && iMouseX < ix + m_width && iy <= iMouseY && iMouseY < iy + 32)
        return MENU_CODE_CLICKED;

    //Otherwise this control wasn't clicked at all
    return MENU_CODE_NONE;
}


template<typename T>
void MI_SelectFieldDyn<T>::Refresh()
{
    if (m_outputPtr)
        setCurrentValue(*m_outputPtr);
}


template<typename T>
bool MI_SelectFieldDyn<T>::setCurrentValue(T value)
{
    for (m_index = 0; m_index < m_items.size(); m_index++) {
        if (m_items[m_index].value == value) {
            updateOutput();
            return true;
        }
    }
    return false;
}


template<typename T>
bool MI_SelectFieldDyn<T>::setCurrentIndex(size_t index)
{
    if (m_index < m_items.size()) {
        m_index = index;
        updateOutput();
        return true;
    }
    return false;
}


template<typename T>
void MI_SelectFieldDyn<T>::updateOutput() const
{
    if (m_outputPtr)
        *m_outputPtr = currentValue();
}


template<typename T>
void MI_SelectFieldDyn<T>::add(std::string name, T value, bool hidden, bool goodRandom)
{
    size_t new_idx = m_items.size();
    m_items.emplace_back(std::move(name), std::move(value), hidden, -1);
    m_index = 0;

    if (goodRandom)
        m_goodRandomIndices.push_back(new_idx);
}


template<typename T>
void MI_SelectFieldDyn<T>::clear()
{
    m_items.clear();
    m_goodRandomIndices.clear();
}


template<typename T>
void MI_SelectFieldDyn<T>::hideItem(T value, bool hide)
{
    for (Item& item : m_items) {
        if (item.value == value)
            item.hidden = hide;
    }
    if (currentItem().hidden)
        moveNext() || movePrev();
}


template<typename T>
void MI_SelectFieldDyn<T>::hideAllItems(bool hide)
{
    for (Item& item : m_items) {
        item.hidden = hide;
    }
}


template<typename T>
bool MI_SelectFieldDyn<T>::moveNext()
{
    if (m_items.empty())
        return false;

    // stop when detecting a restart or a loop
    const size_t stop_idx = m_wraps ? m_index : 0;

    while (true) {
        m_index = (m_index + 1) % m_items.size();
        if (m_index == stop_idx)
            break;
        if (!currentItem().hidden) {
            updateOutput();
            return true;
        }
    }

    return false;
}


template<typename T>
bool MI_SelectFieldDyn<T>::movePrev()
{
    if (m_items.empty())
        return false;

    // stop when detecting a restart or a loop
    const size_t stop_idx = m_wraps ? m_index : (m_items.size() - 1);

    while (true) {
        m_index = (m_index + m_items.size() - 1) % m_items.size();
        if (m_index == stop_idx)
            break;
        if (!currentItem().hidden) {
            updateOutput();
            return true;
        }
    }

    return false;
}


template<typename T>
bool MI_SelectFieldDyn<T>::moveRandom()
{
    std::vector<size_t> valid_indices;
    valid_indices.reserve(m_items.size());

    for (size_t idx = 0; idx < m_items.size(); idx++) {
        if (!m_items[idx].hidden && idx != m_index)
            valid_indices.push_back(idx);
    }
    if (valid_indices.size() <= 1)
        return false;

    m_index = valid_indices[RANDOM_INT(valid_indices.size())];
    updateOutput();
    return true;
}


// Actual typed classes
template class MI_SelectFieldDyn<bool>;
template class MI_SelectFieldDyn<short>;
