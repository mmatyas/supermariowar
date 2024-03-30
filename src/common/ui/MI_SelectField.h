#ifndef UI_SELECT_FIELD_H
#define UI_SELECT_FIELD_H

#include "uicontrol.h"
#include "gfx/gfxSprite.h"
#include "RandomNumberGenerator.h"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>


class MI_Image;

class SF_ListItem
{
public:
    SF_ListItem() {
        sName = "";

        iValue = 0;
        sValue = "";
        fValue = false;
        fHidden = false;
        iIconOverride = -1;
    }

    SF_ListItem(std::string sname, short ivalue, std::string svalue, bool fvalue, bool fhidden, short iiconoverride) {
        sName = sname;

        iValue = ivalue;
        sValue = svalue;
        fValue = fvalue;
        fHidden = fhidden;
        iIconOverride = iiconoverride;
    }

    SF_ListItem(const SF_ListItem& other) {
        sName = other.sName;

        iValue = other.iValue;
        sValue = other.sValue;
        fValue = other.fValue;

        fHidden = other.fHidden;
        iIconOverride = other.iIconOverride;
    }

    ~SF_ListItem() {}

    std::string sName;  //Display name

    short iValue;  //int data
    std::string sValue;  //string data
    bool fValue;  //bool data for toggle controls

    bool fHidden;

    short iIconOverride;
};

class MI_SelectField : public UI_Control
{
public:

    MI_SelectField(gfxSprite * nspr, short x, short y, std::string name, short width, short indent);
    MI_SelectField(const MI_SelectField&);
    virtual ~MI_SelectField();

    void SetTitle(std::string name);

    //When this control is selected, advance the value one instead of modifying the control
    void SetAutoAdvance(bool advance) {
        fAutoAdvance = advance;
    }
    void SetNoWrap(bool nowrap) {
        fNoWrap = nowrap;
    }

    //Gets the values of the currently selected item
    short GetShortValue() {
        return (*current)->iValue;
    }
    std::string GetStringValue() {
        return (*current)->sValue;
    }

    //Gets a random value, but does not set the control to that value
    short GetRandomShortValue() {
        return goodRandomItems[RANDOM_INT(goodRandomItems.size())]->iValue;
    }

    //sets the currently selected item
    bool SetKey(short iID);
    bool SetIndex(unsigned short iPosition);

    //Gets the currently selected item
    SF_ListItem GetValue() {
        return **current;
    }

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify);

    //Adds an item to the list
    void Add(std::string name, short ivalue, std::string svalue, bool fvalue, bool fhidden, bool fGoodRandom = true, short iIconOverride = -1);
    void Clear() {
        items.clear();
    }

    bool HideItem(short iID, bool fhide);
    void HideAllItems(bool fHide);

    //Updates animations or other events every frame
    void Update();

    //Draws every frame
    virtual void Draw();

    //Sends player input to control on every frame
    virtual MenuCodeEnum SendInput(CPlayerInput * playerInput);

    //When the item is changed, this code will be returned from SendInput()
    void SetItemChangedCode(MenuCodeEnum code) {
        mcItemChangedCode = code;
    }
    void SetControlSelectedCode(MenuCodeEnum code) {
        mcControlSelectedCode = code;
    }

    //Set where the data of this control is written to (some member of game_values probably)
    void SetData(short * ivalue, std::string * svalue, bool * fvalue) {
        iValue = ivalue;
        sValue = svalue;
        fValue = fvalue;
    }

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    void Refresh();

    void SetValues();

    void AllowFastScroll(bool fastscroll) {
        fFastScroll = fastscroll;
    }

protected:
    bool MoveNext();
    bool MovePrev();
    bool MoveRandom();

    gfxSprite* spr = nullptr;
    std::string szName;

    short* iValue = nullptr;
    std::string* sValue = nullptr;
    bool* fValue = nullptr;

    std::vector<SF_ListItem*> items;
    std::vector<SF_ListItem*>::iterator current;

    std::vector<SF_ListItem*> goodRandomItems;

    short iIndex = 0;
    short iWidth = 0;
    short iIndent = 0;

    std::unique_ptr<MI_Image> miModifyImageLeft;
    std::unique_ptr<MI_Image> miModifyImageRight;

    MenuCodeEnum mcItemChangedCode = MENU_CODE_NONE;
    MenuCodeEnum mcControlSelectedCode = MENU_CODE_NONE;

    bool fAutoAdvance = false;
    bool fNoWrap = false;

    short iAdjustmentY = 0;

    bool fFastScroll = false;
};


template<typename T>
struct SF_ListItemDyn {
    SF_ListItemDyn(std::string&& name, T value, bool hidden, short iconOverride)
        : name(std::move(name))
        , value(value)
        , hidden(hidden)
        , iconOverride(iconOverride)
    {
        static_assert(std::is_trivially_copyable<T>::value, "");
    }

    /*SF_ListItemDyn(const SF_ListItemDyn& other)
        : name(other.name)
        , value(other.value)
        , hidden(other.hidden)
        , iconOverride(other.iconOverride)
    {}*/

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
    void add(std::string name, T value, bool hidden = false, bool goodRandom = true, short iconOverride = -1);
    //! Removes all items
    void clear();
    //! Hides the items containing the specified value
    void hideItem(T value, bool hide);
    //! Hides all items
    void hideAllItems(bool hide);

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

    void updateOutput() const;
    bool moveNext();
    bool movePrev();
    bool moveRandom();
};


#endif // UI_SELECT_FIELD_H
