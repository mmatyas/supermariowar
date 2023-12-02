#include "uicontrol.h"

#include "GameValues.h"
#include "ResourceManager.h"

#include <cmath>
#include <cstring>

extern CGameValues game_values;
extern CResourceManager* rm;

UI_Control::UI_Control(short x, short y)
    : ix(x)
    , iy(y)
{
    neighborControls.fill(nullptr);
}

UI_Control& UI_Control::operator= (const UI_Control& other)
{
    if (this != &other) {
        ix = other.ix;
        iy = other.iy;

        fSelected = other.fSelected;
        fModifying = other.fModifying;
        fAutoModify = other.fAutoModify;
        fDisable = other.fDisable;
        fShow = other.fShow;

        uiMenu = nullptr;
        neighborControls.fill(nullptr);
        iControllingTeam = other.iControllingTeam;
    }
    return *this;
}

UI_Control::UI_Control(const UI_Control& other)
{
    *this = other;
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

    onPressFn = [](){}; // do nothing
}

MI_Button::~MI_Button()
{
    delete [] szName;
}

MenuCodeEnum MI_Button::Modify(bool)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    onPressFn();
    return menuCode;
}

MenuCodeEnum MI_Button::SendInput(CPlayerInput *)
{
    //If input is being sent, that means the button is selected i.e. clicked
    onPressFn();
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

void MI_Button::SetOnPress(std::function<void()>&& func)
{
    onPressFn = func;
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
{
    delete [] szText;
}

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
