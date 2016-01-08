#ifndef UI_SELECT_FIELD_H
#define UI_SELECT_FIELD_H

#include "uicontrol.h"
#include "gfx/gfxSprite.h"

#include <string>

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

    MI_SelectField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent);
    MI_SelectField(const MI_SelectField&);
    virtual ~MI_SelectField();

    void SetTitle(char * name);

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
    bool GetBoolValue() {
        return (*current)->fValue;
    }

    //Gets a random value, but does not set the control to that value
    short GetRandomShortValue() {
        return goodRandomItems[RANDOM_INT(goodRandomItems.size())]->iValue;
    }
    bool GetRandomBoolValue() {
        return goodRandomItems[RANDOM_INT(goodRandomItems.size())]->fValue;
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

    gfxSprite * spr;
    char * szName;

    short * iValue;
    std::string * sValue;
    bool * fValue;

    std::vector<SF_ListItem*> items;
    std::vector<SF_ListItem*>::iterator current;

    std::vector<SF_ListItem*> goodRandomItems;

    short iIndex;

    short iWidth, iIndent;

    MI_Image * miModifyImageLeft;
    MI_Image * miModifyImageRight;

    MenuCodeEnum mcItemChangedCode;
    MenuCodeEnum mcControlSelectedCode;

    bool fAutoAdvance;
    bool fNoWrap;

    short iAdjustmentY;

    bool fFastScroll;
};

#endif // UI_SELECT_FIELD_H
