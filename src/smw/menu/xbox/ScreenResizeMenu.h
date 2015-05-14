#ifndef MENU_SCREENRESIZE_H
#define MENU_SCREENRESIZE_H

#include "uimenu.h"
#include "uicontrol.h"

/*
    Description.
*/

class UI_ScreenResizeMenu : public UI_Menu
{
public:
    UI_ScreenResizeMenu();
    ~UI_ScreenResizeMenu();

private:
    MI_ScreenResize * miScreenResize;
};

#endif // MENU_SCREENRESIZE_H
