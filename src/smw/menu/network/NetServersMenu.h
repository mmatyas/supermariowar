#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_NetworkListScroll;
class MI_Text;
class MI_TextField;


class UI_NetServersMenu : public UI_Menu {
public:
    UI_NetServersMenu();
    ~UI_NetServersMenu();

    void Refresh();
    void OpenServerList();
    void ConnectInProgress();
    void Restore();
    void RefreshScroll();

private:
    MI_Button* miNetServersSelectButton = nullptr;
    MI_Text* miNetServersSelectedHostText = nullptr;
    MI_Button* miNetServersConnectButton = nullptr;
    MI_Button* miNetServersAddRemoveButton = nullptr;
    MI_TextField* miNetServersNicknameField = nullptr;
    MI_Button* miNetServersBackButton = nullptr;

    MI_NetworkListScroll* miNetServersScroll = nullptr;

    MI_Image* miNetServersConnectingDialogImage = nullptr;
    MI_Text* miNetServersConnectingDialogText = nullptr;

    MI_Image* miNetServersLeftHeaderBar = nullptr;
    MI_Image* miNetServersRightHeaderBar = nullptr;
    MI_Text* miNetServersHeaderText = nullptr;
};
