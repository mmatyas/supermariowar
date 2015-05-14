#include "MapFilterEditMenu.h"

UI_MapFilterEditMenu::UI_MapFilterEditMenu() : UI_Menu()
{
    miMapBrowser = new MI_MapBrowser();
    miMapBrowser->SetAutoModify(true);

    AddControl(miMapBrowser, NULL, NULL, NULL, NULL);
    SetHeadControl(miMapBrowser);
    SetCancelCode(MENU_CODE_MAP_BROWSER_EXIT);
};

UI_MapFilterEditMenu::~UI_MapFilterEditMenu() {
}
