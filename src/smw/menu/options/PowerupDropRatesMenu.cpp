#include "PowerupDropRatesMenu.h"

#include "ResourceManager.h"
extern CResourceManager* rm;

UI_PowerupDropRatesMenu::UI_PowerupDropRatesMenu() : UI_Menu()
{
    miPowerupSelection = new MI_PowerupSelection(50, 44, 640, 8);
    miPowerupSelection->SetAutoModify(true);

    miPowerupSelectionLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miPowerupSelectionMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miPowerupSelectionMenuHeaderText = new MI_Text("Item Selection Menu", 320, 5, 0, 2, 1);

    AddControl(miPowerupSelection, NULL, NULL, NULL, NULL);

    AddNonControl(miPowerupSelectionLeftHeaderBar);
    AddNonControl(miPowerupSelectionMenuRightHeaderBar);
    AddNonControl(miPowerupSelectionMenuHeaderText);

    SetHeadControl(miPowerupSelection);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
    
};

UI_PowerupDropRatesMenu::~UI_PowerupDropRatesMenu() {
}
