#include "MI_PacksField.h"

#include "FileList.h"
#include "input.h"


MI_PacksField::MI_PacksField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent, SimpleFileList* pList, MenuCodeEnum code)
    : MI_AnnouncerField(nspr, x, y, std::move(name), width, indent, pList)
    , itemChangedCode(code)
{}


MenuCodeEnum MI_PacksField::SendInput(CPlayerInput* playerInput)
{
    size_t iLastIndex = list->currentIndex();
    MenuCodeEnum code = MI_AnnouncerField::SendInput(playerInput);

    if (MENU_CODE_UNSELECT_ITEM == code)
        return code;

    MenuCodeEnum returnCode = MENU_CODE_NONE;
    if (iLastIndex != list->currentIndex())
        returnCode = itemChangedCode;

    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed)
            return returnCode;

        if (playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed)
            return returnCode;

        if (playerInput->outputControls[iPlayer].menu_random.fPressed)
            return returnCode;
    }

    return MENU_CODE_NONE;
}
