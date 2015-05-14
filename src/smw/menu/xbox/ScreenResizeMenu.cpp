#include "ScreenResizeMenu.h"

UI_ScreenResizeMenu::UI_ScreenResizeMenu() : UI_Menu()
{
    miScreenResize = new MI_ScreenResize();

    mScreenResizeMenu.AddControl(miScreenResize, NULL, NULL, NULL, NULL);
    mScreenResizeMenu.SetHeadControl(miScreenResize);
    mScreenResizeMenu.SetCancelCode(MENU_CODE_BACK_TO_SCREEN_SETTINGS_MENU);
};

UI_ScreenResizeMenu::~UI_ScreenResizeMenu() {
}
