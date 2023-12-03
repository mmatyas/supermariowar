#pragma once

#include "uicontrol.h"

#include <memory>
#include <string>

class gfxSprite;
class MI_Image;
class SimpleFileList;


class MI_AnnouncerField : public UI_Control {
public:
    MI_AnnouncerField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent, SimpleFileList* pList);

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify) override;

    //Updates animations or other events every frame
    void Update() override;

    //Draws every frame
    void Draw() override;

    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;

protected:
    void UpdateName();

    gfxSprite* spr = nullptr;

    std::string szName;
    short iWidth = 0;
    short iIndent = 0;

    std::string szFieldName;

    std::unique_ptr<MI_Image> miModifyImageLeft;
    std::unique_ptr<MI_Image> miModifyImageRight;

    SimpleFileList* list = nullptr;
};
