#pragma once

#include "uicontrol.h"
#include "RandomNumberGenerator.h"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>


class gfxSprite;
class MI_Image;


template<typename T>
struct SF_ListItemDyn {
    SF_ListItemDyn(std::string&& name, T value)
        : name(std::move(name))
        , value(value)
    {
        static_assert(std::is_trivially_copyable<T>::value, "");
    }

    const std::string name;  //!< Display name
    const T value;
    bool hidden = false;
    short iconOverride = -1;
};


template<typename T>
class MI_SelectFieldDyn : public UI_Control {
    using Item = SF_ListItemDyn<T>;

public:
    MI_SelectFieldDyn(gfxSprite* nspr, short x, short y, std::string name, short width, short indent);
    MI_SelectFieldDyn(const MI_SelectFieldDyn<T>&);
    virtual ~MI_SelectFieldDyn() = default;

    //! Set the displayed name.
    void setTitle(std::string name) {
        m_name = std::move(name);
    }
    //! When this control is selected, advance the value one instead of modifying the control.
    void setAutoAdvance(bool advance) {
        m_autoAdvance = advance;
    }
    void allowWrap(bool wraps) {
        m_wraps = wraps;
    }
    //When the item is changed, this code will be returned from SendInput()
    void setItemChangedCode(MenuCodeEnum code) {
        mcItemChangedCode = code;
    }
    void setControlSelectedCode(MenuCodeEnum code) {
        mcControlSelectedCode = code;
    }
    void allowFastScroll(bool fastscroll) {
        m_fastScroll = fastscroll;
    }
    //! Set where the data of this control is written to (some member of game_values probably)
    void setOutputPtr(T* ptr) {
        m_outputPtr = ptr;
    }

    //! Gets the currently selected item
    const Item& currentItem() const {
        return m_items.at(m_index);
    }
    //! Gets the currently selected item's value
    T currentValue() const {
        return currentItem().value;
    }
    //! Gets a random value, but does not set the control to that value
    T randomValue() const {
        const size_t idx = m_goodRandomIndices.at(RANDOM_INT(m_goodRandomIndices.size()));
        return m_items.at(idx).value;
    }

    //! Set the current selected item based on its value
    bool setCurrentValue(T value);
    //! Sets the currently selected item
    bool setCurrentIndex(size_t index);
    //! Adds an item to the list
    Item& add(std::string name, T value, bool goodRandom = true);
    //! Removes all items
    void clear();
    //! Hides the items containing the specified value
    void hideItem(T value, bool hide);
    //! Hides all items
    void hideAllItems(bool hide);
    //! Updates the value pointed by the output pointer, if it's set
    void updateOutput() const;

public:
    void Update() override;  //! Updates animations or other events every frame
    void Draw() override;  //! Draws every frame
    void Refresh() override;
    MenuCodeEnum Modify(bool modify) override;  //! Called when user selects this control to change it's value
    MenuCodeEnum SendInput(CPlayerInput* playerInput) override;  //! Sends player input to control on every frame
    MenuCodeEnum MouseClick(short iMouseX, short iMouseY) override;

protected:
    gfxSprite* m_spr = nullptr;
    std::string m_name;

    std::unique_ptr<MI_Image> miModifyImageLeft;
    std::unique_ptr<MI_Image> miModifyImageRight;

    MenuCodeEnum mcItemChangedCode = MENU_CODE_NONE;
    MenuCodeEnum mcControlSelectedCode = MENU_CODE_NONE;

    bool m_autoAdvance = false;
    bool m_wraps = true;
    bool m_fastScroll = false;
    short m_adjustmentY = 0;
    short m_width = 0;
    short m_indent = 0;

    std::vector<Item> m_items;
    std::vector<size_t> m_goodRandomIndices;
    size_t m_index = 0;
    T* m_outputPtr = nullptr;

    bool moveNext();
    bool movePrev();
    bool moveRandom();
};
