#include "NetRoomMenu.h"

#include "net.h"
#include "ResourceManager.h"

extern CResourceManager* rm;

UI_NetRoomMenu::UI_NetRoomMenu() : UI_Menu()
{
    miNetRoomName = new MI_Text("Room name here", 40, 50, 0, 2, 0);
    miNetRoomPlayerName[0] = new MI_Text("P1 name here", 40, 80, 0, 2, 0);
    miNetRoomPlayerName[1] = new MI_Text("P2 name here", 40, 140, 0, 2, 0);
    miNetRoomPlayerName[2] = new MI_Text("P3 name here", 40, 200, 0, 2, 0);
    miNetRoomPlayerName[3] = new MI_Text("P4 name here", 40, 260, 0, 2, 0);

    //AddNonControl(miNetRoomName);
    for (short p = 0; p < 4; p++)
        AddNonControl(miNetRoomPlayerName[p]);

    miNetRoomStartButton = new MI_Button(&rm->spr_selectfield, 320, 300, "(waiting)", 320 - 40, 1);
    miNetRoomStartButton->SetCode(MENU_CODE_TO_NET_ROOM_START_IN_PROGRESS);

    miNetRoomMessages = new MI_ChatMessageBox(20, 432 - 100, 640 - 2 * 26, 1);
    miNetRoomMapPlaceholder = new MI_ChatMessageBox(320, 70, 320 - 40, 5);

    miNetRoomMessageField = new MI_TextField(&rm->menu_plain_field, 26, 432, "Say", 464 - 36, 60);
    miNetRoomMessageField->SetData(netplay.mychatmessage, NET_MAX_CHAT_MSG_LENGTH);

    miNetRoomSendButton = new MI_Button(&rm->spr_selectfield, 464, 432, "Send", 80, 1);
    miNetRoomSendButton->SetCode(MENU_CODE_NET_CHAT_SEND);

    miNetRoomBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miNetRoomBackButton->SetCode(MENU_CODE_TO_NET_LOBBY_MENU);

    AddNonControl(miNetRoomMessages);
    AddNonControl(miNetRoomMapPlaceholder);

    // TODO: if IAmTheRoomHost
    // else disable

    AddControl(miNetRoomStartButton, miNetRoomMessageField, miNetRoomMessageField, miNetRoomMessageField, miNetRoomMessageField);
    AddControl(miNetRoomMessageField, miNetRoomStartButton, miNetRoomStartButton, miNetRoomBackButton, miNetRoomSendButton);
    AddControl(miNetRoomSendButton, miNetRoomStartButton, miNetRoomStartButton, miNetRoomMessageField, miNetRoomBackButton);
    AddControl(miNetRoomBackButton, miNetRoomStartButton, miNetRoomStartButton, miNetRoomSendButton, miNetRoomMessageField);

    miNetRoomStartingDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miNetRoomStartingDialogText = new MI_Text("Starting...", 320, 240 - 40, 0, 2, 1);

    miNetRoomStartingDialogImage->Show(false);
    miNetRoomStartingDialogText->Show(false);

    AddNonControl(miNetRoomStartingDialogImage);
    AddNonControl(miNetRoomStartingDialogText);

    miNetRoomLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miNetRoomRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miNetRoomHeaderText = new MI_Text("Multiplayer Room Menu", 320, 5, 0, 2, 1);

    AddNonControl(miNetRoomLeftHeaderBar);
    AddNonControl(miNetRoomRightHeaderBar);
    AddNonControl(miNetRoomHeaderText);

    SetHeadControl(miNetRoomMessageField);
    SetCancelCode(MENU_CODE_TO_NET_LOBBY_MENU);
};

UI_NetRoomMenu::~UI_NetRoomMenu() {
}

void UI_NetRoomMenu::Refresh()
{
    miNetRoomHeaderText->SetText(netplay.currentRoom.name);

    for (uint8_t p = 0; p < 4; p++)
        miNetRoomPlayerName[p]->SetText(netplay.currentRoom.playerNames[p]);

    if (netplay.theHostIsMe && netplay.currentRoom.playerCount() > 1)
        miNetRoomStartButton->SetName("Start");
    else
        miNetRoomStartButton->SetName("(waiting)");
}

void UI_NetRoomMenu::StartInProgress()
{
    if (netplay.theHostIsMe && netplay.currentRoom.playerCount() > 1) {
        netplay.client.local_gamehost.sendStartRoomMessage();
        netplay.operationInProgress = true;

        miNetRoomStartingDialogImage->Show(true);
        miNetRoomStartingDialogText->Show(true);
        RememberCurrent();

        SetHeadControl(miNetRoomStartingDialogText);
        //SetCancelCode(MENU_CODE_NET_START_ABORT);
        //SetCancelCode(MENU_CODE_NONE);
        SetCancelCode(MENU_CODE_TO_NET_ROOM_MENU);
        ResetMenu();
    }
}

void UI_NetRoomMenu::Restore()
{
    miNetRoomStartingDialogImage->Show(false);
    miNetRoomStartingDialogText->Show(false);
    /*if (netplay.client.theHostIsMe)
        miNetRoomStartButton->SetName("Start");
    else
        miNetRoomStartButton->SetName("(waiting)");*/

    SetHeadControl(miNetRoomMessageField);
    SetCancelCode(MENU_CODE_TO_NET_LOBBY_MENU);

    // Temp.: Set dummy values to room fields.
        netplay.currentRoom.name[0] = '\0';
        netplay.currentRoom.playerNames[0][0] = '\0';
        netplay.currentRoom.playerNames[1][0] = '\0';
        netplay.currentRoom.playerNames[2][0] = '\0';
        netplay.currentRoom.playerNames[3][0] = '\0';
}
