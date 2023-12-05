#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_NetworkListScroll;
class MI_Text;
class MI_TextField;

/*
    Description.
*/

class UI_NetLobbyMenu : public UI_Menu {
public:
    UI_NetLobbyMenu();
    ~UI_NetLobbyMenu();

    void JoinInProgress();
    void AbortJoin();
    void Restore();

private:
    MI_NetworkListScroll* miNetLobbyScroll = nullptr;

    MI_Button* miNetLobbyNewRoomButton = nullptr;
    MI_TextField* miNetLobbyFilterField = nullptr;
    MI_Button* miNetLobbyRefreshButton = nullptr;
    MI_Button* miNetLobbyBackButton = nullptr;

    MI_Image* miNetLobbyJoiningDialogImage = nullptr;
    MI_Text* miNetLobbyJoiningDialogText = nullptr;

    MI_Image* miNetLobbyLeftHeaderBar = nullptr;
    MI_Image* miNetLobbyRightHeaderBar = nullptr;
    MI_Text* miNetLobbyHeaderText = nullptr;
};
