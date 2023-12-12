#include "PowerupDropRatesMenu.h"

#include "ResourceManager.h"
#include "ui/MI_Image.h"
#include "ui/MI_PowerupSelection.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;


UI_PowerupDropRatesMenu::UI_PowerupDropRatesMenu()
    : UI_Menu()
{
    miPowerupSelection = new MI_PowerupSelection(50, 44, 640, 8);
    miPowerupSelection->SetAutoModify(true);

    miPowerupSelectionLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miPowerupSelectionMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miPowerupSelectionMenuHeaderText = new MI_HeaderText("Item Selection Menu", 320, 5);

    AddControl(miPowerupSelection, NULL, NULL, NULL, NULL);

    AddNonControl(miPowerupSelectionLeftHeaderBar);
    AddNonControl(miPowerupSelectionMenuRightHeaderBar);
    AddNonControl(miPowerupSelectionMenuHeaderText);

    SetHeadControl(miPowerupSelection);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
}
