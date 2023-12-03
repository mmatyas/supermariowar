#pragma once

#include "uicontrol.h"

#include <memory>
#include <string>

class gfxSprite;
class MI_Image;


class MI_PlaylistField : public UI_Control {
public:
    MI_PlaylistField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent);

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify) override;

    //Updates animations or other events every frame
    void Update() override;

    //Draws every frame
    void Draw() override;

    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput* playerInput) override;

protected:
    gfxSprite* spr = nullptr;

    std::string szName;
    short iWidth = 0;
    short iIndent = 0;

    std::unique_ptr<MI_Image> miModifyImageLeft;
    std::unique_ptr<MI_Image> miModifyImageRight;
};
