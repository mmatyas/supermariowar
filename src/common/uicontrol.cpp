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
