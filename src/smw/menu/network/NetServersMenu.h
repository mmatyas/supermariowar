#ifndef MENU_NETSERVERS_H
#define MENU_NETSERVERS_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_NetServersMenu : public UI_Menu
{
public:
    UI_NetServersMenu();
    ~UI_NetServersMenu();

    void Refresh();
    void OpenServerList();
    void ConnectInProgress();
    void Restore();
    void RefreshScroll();

private:
    MI_Button * miNetServersSelectButton;
    MI_Text * miNetServersSelectedHostText;
    MI_Button * miNetServersConnectButton;
    MI_Button * miNetServersAddRemoveButton;
    MI_TextField * miNetServersNicknameField;
    MI_Button * miNetServersBackButton;

    MI_NetworkListScroll * miNetServersScroll;

    MI_Image * miNetServersConnectingDialogImage;
    MI_Text * miNetServersConnectingDialogText;

    MI_Image * miNetServersLeftHeaderBar;
    MI_Image * miNetServersRightHeaderBar;
    MI_Text * miNetServersHeaderText;
};

#endif // MENU_NETSERVERS_H
