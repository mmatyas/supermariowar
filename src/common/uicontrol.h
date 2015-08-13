#ifndef UICONTROL_H
#define UICONTROL_H

#include "input.h"
#include "ObjectContainer.h"
#include "RandomNumberGenerator.h"
#include "uimenu.h"

class UI_Menu;

class UI_Control
{
public:
    UI_Control(short x, short y);
    UI_Control(const UI_Control&);
    UI_Control & operator= (const UI_Control&);
    virtual ~UI_Control() {}

    virtual void Update() {}
    virtual void Draw() {}

    virtual MenuCodeEnum SendInput(CPlayerInput *) {
        return MENU_CODE_NONE;
    }

    bool Select(bool select) {
        fSelected = select;

        if (fSelected && fAutoModify)
            Modify(true);

        return fModifying;
    }

    virtual MenuCodeEnum Modify(bool modify) {
        fModifying = modify;
        return MENU_CODE_MODIFY_ACCEPTED;
    }

    void SetAutoModify(bool autoModify) {
        fAutoModify = autoModify;
    }
    bool IsAutoModify() {
        return fAutoModify;
    }

    void SetPosition(short x, short y) {
        ix = x;
        iy = y;
    }

    void SetNeighbor(short iNeighbor, UI_Control * uiControl) {
        neighborControls[iNeighbor] = uiControl;
    }
    UI_Control * GetNeighbor(short iNeighbor) {
        return neighborControls[iNeighbor];
    }

    void Show(bool show) {
        fShow = show;
    }
    bool IsVisible() {
        return fShow;
    }

    void SetMenuParent(UI_Menu * menu) {
        uiMenu = menu;
    }

    bool IsModifying() {
        return fModifying;
    }

    void SetControllingTeam(short teamid) {
        iControllingTeam = teamid;
    }

    virtual MenuCodeEnum MouseClick(short iMouseX, short iMouseY) {
        return MENU_CODE_NONE;
    }

    virtual void Refresh() {}

    virtual void Disable(bool disable) {
        fDisable = disable;
    }

protected:

    bool fSelected;
    bool fModifying;
    bool fAutoModify;
    bool fDisable;

    short ix, iy;

    UI_Control * neighborControls[4];

    bool fShow;

    UI_Menu * uiMenu;

    short iControllingTeam;
};

/*********************************************************
 *    Non Selectable Controls (Text and Images)
 *********************************************************/
class MI_Image : public UI_Control
{
public:
    MI_Image(gfxSprite * nspr, short x, short y, short srcx, short srcy, short w, short h, short numxframes, short numyframes, short speed);
    virtual ~MI_Image();

    void Update();
    void Draw();

    void SetPosition(short x, short y) {
        ix = x;
        iy = y;
    }
    void SetAnimationSpeed(short speed) {
        iSpeed = speed;
    }
    void SetImage(short srcx, short srcy, short w, short h) {
        isrcx = srcx;
        isrcy = srcy;
        iw = w;
        ih = h;
        iXFrame = srcx;
        iYFrame = srcy;
    }
    void SetImageSource(gfxSprite * nspr) {
        spr = nspr;
    }

    void SetPulse(bool pulse) {
        fPulse = pulse;
    }
    void SetSwirl(bool swirl, float radius, float angle, float radiusSpeed, float angleSpeed) {
        fSwirl = swirl;
        dSwirlRadius = radius;
        dSwirlAngle = angle;
        dSwirlRadiusSpeed = radiusSpeed;
        dSwirlAngleSpeed = angleSpeed;
    }
    void StopSwirl() {
        SetSwirl(false, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    void SetBlink(bool blink, short interval) {
        fBlink = blink;
        iBlinkInterval = interval;
    }

    bool IsSwirling() {
        return fSwirl;
    }

    void GetPositionAndSize(short * x, short * y, short * w, short * h) {
        *x = ix;
        *y = iy;
        *w = iw;
        *h = ih;
    }

private:
    gfxSprite * spr;

    short iNumXFrames, iNumYFrames;
    short isrcx, isrcy;
    short iw, ih;
    short iSpeed, iTimer;
    short iXFrame, iYFrame;

    bool fPulse;
    short iPulseValue, iPulseDelay;
    bool fPulseOut;

    bool fSwirl;
    float dSwirlRadius;
    float dSwirlAngle;
    float dSwirlRadiusSpeed;
    float dSwirlAngleSpeed;

    bool fBlink;
    short iBlinkInterval;
    short iBlinkCounter;
    bool fBlinkShow;
};

class MI_Text : public UI_Control
{
public:
    MI_Text(const char * text, short x, short y, short w, short size, short justified);
    virtual ~MI_Text();

    void SetText(const char * text);
    void Draw();

private:
    char * szText;
    short iw;
    short iJustified;
    gfxFont * font;
};

class MI_ScoreText : public UI_Control
{
public:
    MI_ScoreText(short x, short y);
    virtual ~MI_ScoreText() {}

    void Draw();
    void SetScore(short iScore);

private:
    short iScore;
    short iDigitLeftSrcX, iDigitMiddleSrcX, iDigitRightSrcX;
    short iDigitLeftDstX, iDigitMiddleDstX, iDigitRightDstX;

};
/*********************************************************
 *    Selectable Controls (Buttons, Fields)
 *********************************************************/
class MI_IPField : public UI_Control
{

public:

    MI_IPField(gfxSprite * nspr, short x, short y);
    virtual ~MI_IPField();

    char * GetValue();

    void Update();
    void Draw();

    MenuCodeEnum SendInput(CPlayerInput * playerInput);
    MenuCodeEnum Modify(bool modify);

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

private:

    void AssignHostAddress();
    void MoveImage();

    gfxSprite * spr;

    int iSelectedDigit;

    int values[12];

    char szValue[16];
    short iDigitPosition[12];

    MI_Image * miModifyImage;
};

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

class MI_ImageSelectField : public MI_SelectField
{
public:

    MI_ImageSelectField(gfxSprite * nspr, gfxSprite * nspr_image, short x, short y, const char * name, short width, short indent, short imageHeight, short imageWidth);
    MI_ImageSelectField(const MI_ImageSelectField&);
    virtual ~MI_ImageSelectField();

    void Draw();

private:

    gfxSprite * spr_image;
    short iImageWidth, iImageHeight;
};

class MI_Button : public UI_Control
{
public:

    MI_Button(gfxSprite * nspr, short x, short y, const char * name, short width, short justified);
    ~MI_Button() {}

    MenuCodeEnum Modify(bool fModify);

    void Draw();
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

    void SetName(const char * name);
    void SetCode(MenuCodeEnum code) {
        menuCode = code;
    }
    void SetImage(gfxSprite * nsprImage, short x, short y, short w, short h);

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

protected:

    gfxSprite * spr;
    char * szName;

    short iWidth, iIndent;
    short iTextJustified;

    MenuCodeEnum menuCode;

    gfxSprite * sprImage;
    short iImageSrcX;
    short iImageSrcY;
    short iImageW;
    short iImageH;

    short iTextW;

    short iAdjustmentY;
    short iHalfWidth;

};

class MI_SliderField : public MI_SelectField
{
public:

    MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, const char * name, short width, short indent1, short indent2);
    virtual ~MI_SliderField();

    void SetPosition(short x, short y);

    //Draws every frame
    virtual void Draw();
    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

protected:

    gfxSprite * sprSlider;
    short iIndent2;

};

class MI_PowerupSlider : public MI_SliderField
{
public:

    MI_PowerupSlider(gfxSprite * nspr, gfxSprite * nsprSlider, gfxSprite * nsprPowerup, short x, short y, short width, short powerupIndex);
    virtual ~MI_PowerupSlider();

    //Draws every frame
    void Draw();

protected:

    gfxSprite * sprPowerup;
    short iPowerupIndex;
    short iHalfWidth;
};

class MI_FrenzyModeOptions : public UI_Control
{
public:

    MI_FrenzyModeOptions(short x, short y, short width, short numlines);
    virtual ~MI_FrenzyModeOptions();

    MenuCodeEnum Modify(bool modify);
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

    void Update();
    void Draw();

    void MoveNext();
    void MovePrev();

    void SetRandomGameModeSettings();

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    void Refresh();

private:

    void SetupPowerupFields();
    void AdjustDisplayArrows();

    short iIndex, iOffset;
    short iTopStop, iBottomStop;
    short iNumLines;
    short iWidth;

    UI_Menu * mMenu;

    MI_SelectField * miQuantityField;
    MI_SelectField * miRateField;
    MI_SelectField * miStoredShellsField;
    MI_PowerupSlider * miPowerupSlider[NUMFRENZYCARDS];
    MI_Button * miBackButton;

    MI_Image * miUpArrow;
    MI_Image * miDownArrow;
};

class MI_TextField : public UI_Control
{
public:

    MI_TextField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent);
    virtual ~MI_TextField();

    void SetTitle(char * name);

    //Gets the values of the currently selected item
    char * GetValue() {
        return szValue;
    }

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify);

    void Clear() {
        szValue[0] = 0;
        iCursorIndex = 0;
        iNumChars = 1;
    }

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
    void SetData(char * value, short maxchars);

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    void Refresh();

    void SetDisallowedChars(const char * chars);

protected:

    void UpdateCursor();

    short iCursorIndex;
    short iNumChars, iMaxChars;

    gfxSprite * spr;
    char * szName;

    char * szValue;

    short iWidth, iIndent;

    MI_Image * miModifyCursor;

    MenuCodeEnum mcItemChangedCode;
    MenuCodeEnum mcControlSelectedCode;

    short iAdjustmentY;
    char * szTempValue;

    short iStringWidth, iAllowedWidth;

    char szDisallowedChars[32];
};


class MI_MapField : public UI_Control
{
public:

    MI_MapField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, bool showtags);
    virtual ~MI_MapField();

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify);

    //Updates animations or other events every frame
    void Update();

    //Draws every frame
    void Draw();

    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

    void AdjustIndicators();

    void LoadCurrentMap();
    void LoadMap(const char * szMapPath);

    bool SetMap(const char * szMapName, bool fWorld);
    void SetSpecialMap(const char * szMapName, const char * szMapPath);

    const char * GetMapName() { return szMapName; }
    std::string GetMapFilePath();

    MenuCodeEnum ChooseRandomMap();

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    bool MovePrev(bool fScrollFast);
    bool MoveNext(bool fScrollFast);

    void SetDimensions(short iWidth, short iIndent);

protected:

    bool Move(bool fNext, bool fScrollFast);

    gfxSprite * spr;

    SDL_Surface * surfaceMapBackground;
    SDL_Surface * surfaceMapBlockLayer;
    SDL_Surface * surfaceMapForeground;
    SDL_Rect rectDst;

    char * szName;
    char szMapName[256];
    short iWidth, iIndent;

    MI_Image * miModifyImageLeft;
    MI_Image * miModifyImageRight;

    short iSlideListOut;
    short iSlideListOutGoal;

    std::string sSearchString;
    short iSearchStringTimer;

    bool fShowtags;
};

#endif // UICONTROL_H
