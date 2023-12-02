#pragma once

#include "uicontrol.h"

class gfxSprite;


class MI_Image : public UI_Control {
public:
    MI_Image(gfxSprite* nspr,
        short x, short y,
        short srcx, short srcy,
        short w, short h,
        short numxframes, short numyframes, short speed);

    void Update() override;
    void Draw() override;

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

    bool IsSwirling() const {
        return fSwirl;
    }

    void GetPositionAndSize(short& x, short& y, short& w, short& h) const {
        x = ix;
        y = iy;
        w = iw;
        h = ih;
    }

private:
    gfxSprite* spr = nullptr;

    short iNumXFrames = 0;
    short iNumYFrames = 0;
    short isrcx = 0;
    short isrcy = 0;
    short iw = 0;
    short ih = 0;
    short iSpeed = 0;
    short iTimer = 0;
    short iXFrame = 0;
    short iYFrame = 0;

    bool fPulse = false;
    short iPulseValue = 0;
    short iPulseDelay = 0;
    bool fPulseOut = true;

    bool fSwirl = false;
    float dSwirlRadius = 0.f;
    float dSwirlAngle = 0.f;
    float dSwirlRadiusSpeed = 0.f;
    float dSwirlAngleSpeed = 0.f;

    bool fBlink = false;
    short iBlinkInterval = 0;
    short iBlinkCounter = 0;
    bool fBlinkShow = true;
};
