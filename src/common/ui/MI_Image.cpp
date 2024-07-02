#include "MI_Image.h"

#include "gfx.h"

#include <cmath>


MI_Image::MI_Image(
        gfxSprite * nspr,
        short x, short y,
        short srcx, short srcy,
        short w, short h,
        short numxframes, short numyframes, short speed)
    : UI_Control(x, y)
    , spr(nspr)
    , iNumXFrames(numxframes)
    , iNumYFrames(numyframes)
    , isrcx(srcx)
    , isrcy(srcy)
    , iw(w)
    , ih(h)
    , iSpeed(speed)
    , iXFrame(srcx)
    , iYFrame(srcy)
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
        spr->drawStretch(m_pos.x - iPulseValue + iXOffset, m_pos.y - iPulseValue + iYOffset, iw + (iPulseValue << 1), ih + (iPulseValue << 1), iXFrame, iYFrame, iw, ih);
    else
        spr->draw(m_pos.x + iXOffset, m_pos.y + iYOffset, iXFrame, iYFrame, iw, ih);
}
