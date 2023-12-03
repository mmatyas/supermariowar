#pragma once

#include "uicontrol.h"

#include <array>
#include <memory>
#include <string>

class gfxSprite;
class MI_Image;


class MI_PlayerSelect : public UI_Control {
public:
    MI_PlayerSelect(gfxSprite* nspr, short x, short y, std::string name, short width, short indent);

    void SetImagePosition();

    void Update() override;
    void Draw() override;

    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;
    MenuCodeEnum Modify(bool modify) override;

private:
    gfxSprite* spr = nullptr;

    std::string szName;
    short iWidth = 0;
    short iIndent = 0;

    short iSelectedPlayer = 0;
    std::array<short, 4> iPlayerPosition;

    std::unique_ptr<MI_Image> miModifyImage;
};
