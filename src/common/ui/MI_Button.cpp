#include "MI_Button.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;


MI_Button::MI_Button(gfxSprite * nspr, short x, short y, std::string name, short width, TextAlignment align)
    : UI_Control(x, y)
    , spr(nspr)
    , szName(std::move(name))
    , iWidth(width)
    , m_text_align(align)
    , iTextW(rm->menu_font_large.getWidth(szName.c_str()))
    , iAdjustmentY(width > 256 ? 0 : 128)
    , iHalfWidth(width / 2)
{}

MenuCodeEnum MI_Button::Modify(bool)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    onPressFn();
    return menuCode;
}

MenuCodeEnum MI_Button::SendInput(CPlayerInput*)
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

    switch (m_text_align) {
        case TextAlignment::LEFT:
            rm->menu_font_large.drawChopRight(ix + 16 + (iImageW > 0 ? iImageW + 2 : 0), iy + 5, iWidth - 32, szName.c_str());
            if (sprImage)
                sprImage->draw(ix + 16, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
            break;
        case TextAlignment::CENTER:
            rm->menu_font_large.drawCentered(ix + ((iWidth + (iImageW > 0 ? iImageW + 2 : 0)) >> 1), iy + 5, szName.c_str());
            if (sprImage)
                sprImage->draw(ix + (iWidth >> 1) - ((iTextW + iImageW) >> 1) - 1, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
            break;
        case TextAlignment::RIGHT:
            rm->menu_font_large.drawRightJustified(ix + iWidth - 16, iy + 5, szName.c_str());
            if (sprImage)
                sprImage->draw(ix + iWidth - 18 - iTextW - iImageW, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
            break;
    }
}

void MI_Button::SetName(std::string name)
{
    szName = std::move(name);
    iTextW = rm->menu_font_large.getWidth(szName.c_str());
}

void MI_Button::SetImage(gfxSprite* nsprImage, short x, short y, short w, short h)
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
    onPressFn = std::move(func);
}
