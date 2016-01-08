#include "MI_SliderField.h"

#include "ResourceManager.h"
extern CResourceManager* rm;

/**************************************
 * MI_SliderField Class
 **************************************/

MI_SliderField::MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, const char * name, short width, short indent1, short indent2) :
    MI_SelectField(nspr, x, y, name, width, indent1)
{
    iIndent2 = indent2;
    sprSlider = nsprSlider;

    SetPosition(x, y);
}

MI_SliderField::~MI_SliderField()
{}

void MI_SliderField::SetPosition(short x, short y)
{
    MI_SelectField::SetPosition(x, y);
    miModifyImageLeft->SetPosition(ix + iIndent - 26, iy + 4);
    miModifyImageRight->SetPosition(ix + iWidth - 16, iy + 4);
}

void MI_SliderField::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

    if (!items.empty()) {
        rm->menu_font_large.drawChopRight(ix + iIndent2 + 16, iy + 5, iWidth - iIndent2 - 24, (*current)->sName.c_str());
    }

    short iSpacing = (iIndent2 - iIndent - 20) / ((short)items.size() - 1);
    short iSpot = 0;

    for (unsigned int index = 0; index < items.size(); index++) {
        if (index < items.size() - 1)
            sprSlider->draw(ix + iIndent + iSpot + 16, iy + 10, 0, 0, iSpacing, 13);
        else
            sprSlider->draw(ix + iIndent + iSpot + 16, iy + 10, 164, 0, 4, 13);

        iSpot += iSpacing;
    }

    sprSlider->draw(ix + iIndent + (iIndex * iSpacing) + 14, iy + 8, 168, 0, 8, 16);

    if (current != items.begin() || !fNoWrap)
        miModifyImageLeft->Draw();

    if (current != --items.end() || !fNoWrap)
        miModifyImageRight->Draw();
}

MenuCodeEnum MI_SliderField::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_scrollfast.fPressed) {
            if (iIndex == 0)
                while (MoveNext());
            else
                while (MovePrev());

            return mcItemChangedCode;
        }
    }

    return MI_SelectField::SendInput(playerInput);
}
