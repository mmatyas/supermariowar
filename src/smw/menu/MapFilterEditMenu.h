#ifndef MENU_MAPFILTEREDIT_H
#define MENU_MAPFILTEREDIT_H

#include "uimenu.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_MapFilterEditMenu : public UI_Menu
{
public:
    UI_MapFilterEditMenu();
    ~UI_MapFilterEditMenu();

	MI_MapBrowser * miMapBrowser;
};

#endif // MENU_MAPFILTEREDIT_H
