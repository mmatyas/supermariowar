#include "MI_SelectField.h"

#include "input.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "gfx/gfxSprite.h"
#include "ui/MI_Image.h"

extern CResourceManager* rm;


template<typename T>
MI_SelectField<T>::MI_SelectField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent)
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
MI_SelectField<T>::MI_SelectField(const MI_SelectField<T>& other)
    : UI_Control(other)
    , m_spr(other.m_spr)
    , m_name(other.m_name)
    , mcItemChangedCode(other.mcItemChangedCode)
    , mcControlSelectedCode(other.mcControlSelectedCode)
    , m_autoAdvance(other.m_autoAdvance)
    , m_wraps(other.m_wraps)
    , m_fastScroll(other.m_fastScroll)
    , m_adjustmentY(other.m_adjustmentY)
    , m_width(other.m_width)
    , m_indent(other.m_indent)
    , m_items(other.m_items)
    , m_goodRandomIndices(other.m_goodRandomIndices)
    , m_outputPtr(other.m_outputPtr)
{
    setCurrentIndex(other.m_index);

    miModifyImageLeft = std::make_unique<MI_Image>(m_spr, ix + m_indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = std::make_unique<MI_Image>(m_spr, ix + m_width - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);
}


template<typename T>
void MI_SelectField<T>::Update()
{
    miModifyImageRight->Update();
    miModifyImageLeft->Update();
}


template<typename T>
void MI_SelectField<T>::Draw()
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
MenuCodeEnum MI_SelectField<T>::Modify(bool modify)
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
MenuCodeEnum MI_SelectField<T>::SendInput(CPlayerInput * playerInput)
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
MenuCodeEnum MI_SelectField<T>::MouseClick(short iMouseX, short iMouseY) {
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
void MI_SelectField<T>::Refresh()
{
    if (m_outputPtr)
        setCurrentValue(*m_outputPtr);
}


template<typename T>
T MI_SelectField<T>::randomValue() const
{
    const size_t idx = m_goodRandomIndices.at(RANDOM_INT(m_goodRandomIndices.size()));
    return m_items.at(idx).value;
}


template<typename T>
bool MI_SelectField<T>::setCurrentValue(T value)
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
bool MI_SelectField<T>::setCurrentIndex(size_t index)
{
    if (index < m_items.size()) {
        m_index = index;
        updateOutput();
        return true;
    }
    return false;
}


template<typename T>
void MI_SelectField<T>::updateOutput() const
{
    if (m_outputPtr)
        *m_outputPtr = currentValue();
}


template<typename T>
SF_ListItem<T>& MI_SelectField<T>::add(std::string name, T value, bool goodRandom)
{
    size_t new_idx = m_items.size();
    m_items.emplace_back(std::move(name), std::move(value));
    m_index = 0;

    if (goodRandom)
        m_goodRandomIndices.push_back(new_idx);

    return m_items.back();
}


template<typename T>
void MI_SelectField<T>::clear()
{
    m_items.clear();
    m_goodRandomIndices.clear();
}


template<typename T>
void MI_SelectField<T>::hideItem(T value, bool hide)
{
    for (Item& item : m_items) {
        if (item.value == value)
            item.hidden = hide;
    }
    if (currentItem().hidden)
        moveNext() || movePrev();
}


template<typename T>
void MI_SelectField<T>::hideAllItems(bool hide)
{
    for (Item& item : m_items) {
        item.hidden = hide;
    }
}


template<typename T>
bool MI_SelectField<T>::moveNext()
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
bool MI_SelectField<T>::movePrev()
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
bool MI_SelectField<T>::moveRandom()
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

template class MI_SelectField<bool>;
template class MI_SelectField<short>;

#include "MatchTypes.h"
template class MI_SelectField<MatchType>;

#include "EyecandyStyles.h"
template class MI_SelectField<AwardStyle>;
template class MI_SelectField<ScoreboardStyle>;
template class MI_SelectField<SpawnStyle>;

#include "GameplayStyles.h"
template class MI_SelectField<TeamCollisionStyle>;
template class MI_SelectField<TournamentControlStyle>;
