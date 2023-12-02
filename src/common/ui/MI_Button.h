#pragma once

#include "uicontrol.h"

#include <functional>
#include <memory>
#include <string>

class gfxSprite;


class MI_Button : public UI_Control {
public:
    MI_Button(gfxSprite* nspr, short x, short y, std::string name, short width, TextAlign align = TextAlign::LEFT);

    void Draw() override;
    MenuCodeEnum Modify(bool fModify) override;
    MenuCodeEnum SendInput(CPlayerInput* playerInput) override;
    MenuCodeEnum MouseClick(short iMouseX, short iMouseY) override;

    void SetName(std::string name);
    void SetCode(MenuCodeEnum code) {
        menuCode = code;
    }
    void SetImage(gfxSprite* nsprImage, short x, short y, short w, short h);

    void SetOnPress(std::function<void()>&&);

protected:
    gfxSprite* spr = nullptr;
    std::string szName;

    short iWidth = 0;
    short iIndent = 0;
    TextAlign m_text_align = TextAlign::LEFT;

    MenuCodeEnum menuCode = MENU_CODE_NONE;

    gfxSprite* sprImage = nullptr;
    short iImageSrcX = 0;
    short iImageSrcY = 0;
    short iImageW = 0;
    short iImageH = 0;

    short iTextW = 0;

    short iAdjustmentY = 0;
    short iHalfWidth = 0;

    std::function<void()> onPressFn = [](){}; // do nothing
};
