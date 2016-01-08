#ifndef UI_POWERUP_SLIDER_H
#define UI_POWERUP_SLIDER_H

#include "ui/MI_SliderField.h"
#include "gfx/gfxSprite.h"

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

#endif // UI_POWERUP_SLIDER_H
