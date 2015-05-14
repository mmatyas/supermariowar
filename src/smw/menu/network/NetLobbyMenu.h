#ifndef MENU_NETLOBBY_H
#define MENU_NETLOBBY_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_NetLobbyMenu : public UI_Menu
{
public:
    UI_NetLobbyMenu();
    ~UI_NetLobbyMenu();

    void JoinInProgress();
    void AbortJoin();
    void Restore();

private:
    MI_NetworkListScroll * miNetLobbyScroll;

    MI_Button * miNetLobbyNewRoomButton;
    MI_TextField * miNetLobbyFilterField;
    MI_Button * miNetLobbyRefreshButton;
    MI_Button * miNetLobbyBackButton;

    MI_Image * miNetLobbyJoiningDialogImage;
    MI_Text * miNetLobbyJoiningDialogText;

    MI_Image * miNetLobbyLeftHeaderBar;
    MI_Image * miNetLobbyRightHeaderBar;
    MI_Text * miNetLobbyHeaderText;
};

#endif // MENU_NETLOBBY_H
