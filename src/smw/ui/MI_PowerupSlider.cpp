#include "MI_PowerupSlider.h"

/**************************************
 * MI_PowerupSlider Class
 **************************************/

MI_PowerupSlider::MI_PowerupSlider(gfxSprite * nspr, gfxSprite * nsprSlider, gfxSprite * nsprPowerup, short x, short y, short width, short powerupIndex) :
    MI_SliderField(nspr, nsprSlider, x, y, "", width, 0, 0)
{
    sprPowerup = nsprPowerup;
    iPowerupIndex = powerupIndex;

    miModifyImageLeft->SetPosition(ix + 25, iy + 4);
    miModifyImageRight->SetPosition(ix + iWidth - 12, iy + 4);

    iHalfWidth = (width - 38) >> 1;
}

MI_PowerupSlider::~MI_PowerupSlider()
{}

void MI_PowerupSlider::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix + 38, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iHalfWidth, 32);
    spr->draw(ix + 38 + iHalfWidth, iy, 550 - iWidth + iHalfWidth, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iHalfWidth - 38, 32);

    short iSpacing = (iWidth - 100) / ((short)items.size() - 1);
    short iSpot = 0;

    for (unsigned int index = 0; index < items.size(); index++) {
        if (index < items.size() - 1)
            sprSlider->draw(ix + iSpot + 56, iy + 10, 0, 0, iSpacing, 13);
        else
            sprSlider->draw(ix + iSpot + 56, iy + 10, 164, 0, 4, 13);

        iSpot += iSpacing;
    }

    sprSlider->draw(ix + (iIndex * iSpacing) + 54, iy + 8, 168, 0, 8, 16);

    sprSlider->draw(ix + iWidth - 34, iy + 8, iIndex * 16, 16, 16, 16);

    sprPowerup->draw(ix, iy, iPowerupIndex * 32, 0, 32, 32);

    if (current != items.begin() || !fNoWrap)
        miModifyImageLeft->Draw();

    if (current != --items.end() || !fNoWrap)
        miModifyImageRight->Draw();
}
