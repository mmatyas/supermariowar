#ifndef MENU_NETROOM_H
#define MENU_NETROOM_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"
#include "ui/MI_NetRoomTeamSelect.h"

/*
    Description.
*/

class UI_NetRoomMenu : public UI_Menu
{
public:
    UI_NetRoomMenu();
    ~UI_NetRoomMenu();

    void Refresh();
    void StartInProgress();
    void Restore();

    void SetPreviewMapPath(std::string&);

private:
    MI_Text * miNetRoomName;
    MI_Text * miNetRoomPlayerName[4];
    MI_NetRoomTeamSelect* miSkinSelector[4];

    MI_ChatMessageBox * miNetRoomMessages;
    MI_TextField * miNetRoomMessageField;

    MI_MapPreview * miNetRoomMapPreview;

    MI_Button * miNetRoomStartButton;
    MI_Button * miNetRoomSendButton;
    MI_Button * miNetRoomBackButton;

    MI_Image * miNetRoomStartingDialogImage;
    MI_Text * miNetRoomStartingDialogText;

    MI_Image * miNetRoomLeftHeaderBar;
    MI_Image * miNetRoomRightHeaderBar;
    MI_Text * miNetRoomHeaderText;
};

#endif // MENU_NETROOM_H
