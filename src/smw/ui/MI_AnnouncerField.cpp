#include "MI_AnnouncerField.h"

#include "FileList.h"
#include "input.h"
#include "path.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

extern CResourceManager* rm;


MI_AnnouncerField::MI_AnnouncerField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent, SimpleFileList * pList)
    : UI_Control(x, y)
    , spr(nspr)
    , szName(std::move(name))
    , iWidth(width)
    , iIndent(indent)
    , list(pList)
{
    UpdateName();

    miModifyImageLeft = std::make_unique<MI_Image>(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->Show(false);

    miModifyImageRight = std::make_unique<MI_Image>(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->Show(false);
}

MI_AnnouncerField::~MI_AnnouncerField() = default;

MenuCodeEnum MI_AnnouncerField::Modify(bool modify)
{
    miModifyImageLeft->Show(modify);
    miModifyImageRight->Show(modify);

    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_AnnouncerField::SendInput(CPlayerInput* playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed) {
            list->next();
            UpdateName();
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed) {
            list->prev();
            UpdateName();
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_random.fPressed) {
            list->random();
            UpdateName();
            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImageLeft->Show(false);
            miModifyImageRight->Show(false);

            fModifying = false;

            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}

void MI_AnnouncerField::UpdateName()
{
    szFieldName = GetNameFromFileName(list->currentPath());
}

void MI_AnnouncerField::Update()
{
    miModifyImageRight->Update();
    miModifyImageLeft->Update();
}

void MI_AnnouncerField::Draw()
{
    if (!fShow)
        return;

    //Draw the select field background
    spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName.c_str());
    rm->menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, szFieldName.c_str());

    miModifyImageLeft->Draw();
    miModifyImageRight->Draw();
}
