#ifndef UICONTROL_H
#define UICONTROL_H

#include "input.h"
#include "ObjectContainer.h"
#include "RandomNumberGenerator.h"
#include "uimenu.h"

#include <array>
#include <cassert>
#include <functional>

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
        if (fDisable)
            return MENU_CODE_UNSELECT_ITEM;

        fModifying = modify;
        return MENU_CODE_MODIFY_ACCEPTED;
    }

    void SetAutoModify(bool autoModify) {
        fAutoModify = autoModify;
    }
    bool IsAutoModify() const {
        return fAutoModify;
    }

    void SetPosition(short x, short y) {
        ix = x;
        iy = y;
    }

    void SetNeighbor(short iNeighbor, UI_Control * uiControl) {
        assert(iNeighbor < 4);
        neighborControls[iNeighbor] = uiControl;
    }
    UI_Control * GetNeighbor(short iNeighbor) {
        return neighborControls[iNeighbor];
    }

    void Show(bool show) {
        fShow = show;
    }
    bool IsVisible() const {
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
    short ix, iy;

    bool fSelected = false;
    bool fModifying = false;
    bool fAutoModify = false;
    bool fDisable = false;
    bool fShow = true;

    std::array<UI_Control*, 4> neighborControls;

    UI_Menu* uiMenu = nullptr;
    short iControllingTeam = -1;
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


class MI_Button : public UI_Control
{
public:

    MI_Button(gfxSprite * nspr, short x, short y, const char * name, short width, short justified);
    ~MI_Button();

    MenuCodeEnum Modify(bool fModify);

    void Draw();
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

    void SetName(const char * name);
    void SetCode(MenuCodeEnum code) {
        menuCode = code;
    }
    void SetImage(gfxSprite * nsprImage, short x, short y, short w, short h);

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    void SetOnPress(std::function<void()>&&);

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

    std::function<void()> onPressFn;
};

#endif // UICONTROL_H
