#ifndef MENU_NET_EDIT_SERVER_LIST_H
#define MENU_NET_EDIT_SERVER_LIST_H

#include "uimenu.h"
#include "uicontrol.h"

/*
    TODO: Description.
*/

class UI_NetEditServersMenu : public UI_Menu
{
public:
    UI_NetEditServersMenu();
    ~UI_NetEditServersMenu();

private:
    MI_Button * miBackButton;

    MI_Image * miLeftHeaderBar;
    MI_Image * miRightHeaderBar;
    MI_Text * miHeaderText;
};

#endif // MENU_NET_EDIT_SERVER_LIST_H
