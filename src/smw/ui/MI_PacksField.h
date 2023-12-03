#pragma once

#include "MI_AnnouncerField.h"


class MI_PacksField : public MI_AnnouncerField {
public:
    MI_PacksField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent, SimpleFileList* pList, MenuCodeEnum code);

    MenuCodeEnum SendInput(CPlayerInput* playerInput) override;

private:
    const MenuCodeEnum itemChangedCode = MENU_CODE_NONE;
};
