#include "NetLobbyMenu.h"

#include "ResourceManager.h"
#include "net.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_Text.h"
#include "ui/MI_TextField.h"
#include "ui/NetworkListScroll.h"

extern CResourceManager* rm;


UI_NetLobbyMenu::UI_NetLobbyMenu()
    : UI_Menu()
{
    miNetLobbyNewRoomButton = new MI_Button(&rm->spr_selectfield, 320 + 20, 80, "New room", 320 - 30, TextAlign::CENTER);
    miNetLobbyNewRoomButton->SetCode(MENU_CODE_TO_NET_NEW_ROOM_LEVEL_SELECT_MENU);

    miNetLobbyFilterField = new MI_TextField(&rm->menu_plain_field, 320 + 20, 160, "Search", 320 - 30, 90);
    miNetLobbyFilterField->SetData(netplay.roomFilter, NET_MAX_ROOM_NAME_LENGTH);

    miNetLobbyRefreshButton = new MI_Button(&rm->spr_selectfield, 320 + 20, 200, "Refresh", 320 - 30, TextAlign::CENTER);
    miNetLobbyRefreshButton->SetCode(MENU_CODE_TO_NET_LOBBY_MENU);

    miNetLobbyBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miNetLobbyBackButton->SetCode(MENU_CODE_TO_NET_SERVERS_MENU);

    miNetLobbyScroll = new MI_NetworkListScroll(&rm->menu_plain_field, 15, 40, 320, 11, "Rooms", MENU_CODE_NET_JOIN_ROOM_IN_PROGRESS, MENU_CODE_TO_NET_SERVERS_MENU);
    miNetLobbyScroll->RemoteIndex(&netplay.selectedRoomIndex);
    miNetLobbyScroll->SetAutoModify(true);

    // NetClient will add room entries to this list.
    netplay.client.setRoomListUIControl(miNetLobbyScroll);

    miNetLobbyJoiningDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miNetLobbyJoiningDialogText = new MI_HeaderText("Joining...", 320, 240 - 12);

    miNetLobbyJoiningDialogImage->Show(false);
    miNetLobbyJoiningDialogText->Show(false);

    /*for (unsigned iRoom = 0; iRoom < netplay.rooms.size(); iRoom++) {
        Room * room = &netplay.rooms[iRoom];
        //playercount to string
        miNetServersScroll->Add(room->name, "");
    }*/

    miNetLobbyLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miNetLobbyRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miNetLobbyHeaderText = new MI_HeaderText("Multiplayer Lobby Menu", 320, 5);

    AddControl(miNetLobbyScroll, NULL, NULL, NULL, miNetLobbyNewRoomButton);
    AddControl(miNetLobbyNewRoomButton, miNetLobbyBackButton, miNetLobbyFilterField, miNetLobbyScroll, NULL);
    AddControl(miNetLobbyFilterField, miNetLobbyNewRoomButton, miNetLobbyRefreshButton, miNetLobbyScroll, NULL);
    AddControl(miNetLobbyRefreshButton, miNetLobbyFilterField, miNetLobbyBackButton, miNetLobbyScroll, NULL);
    AddControl(miNetLobbyBackButton, miNetLobbyRefreshButton, miNetLobbyNewRoomButton, miNetLobbyScroll, NULL);

    AddNonControl(miNetLobbyJoiningDialogImage);
    AddNonControl(miNetLobbyJoiningDialogText);

    AddNonControl(miNetLobbyLeftHeaderBar);
    AddNonControl(miNetLobbyRightHeaderBar);
    AddNonControl(miNetLobbyHeaderText);

    SetHeadControl(miNetLobbyNewRoomButton);
    SetCancelCode(MENU_CODE_TO_NET_SERVERS_MENU);
};

void UI_NetLobbyMenu::JoinInProgress()
{
    netplay.client.sendJoinRoomMessage();
    netplay.operationInProgress = true;

    miNetLobbyJoiningDialogImage->Show(true);
    miNetLobbyJoiningDialogText->Show(true);

    SetHeadControl(miNetLobbyJoiningDialogText);
    SetCancelCode(MENU_CODE_NET_JOIN_ROOM_ABORT);
    ResetMenu();
}

void UI_NetLobbyMenu::AbortJoin()
{
    netplay.operationInProgress = false;
    Restore();
}

void UI_NetLobbyMenu::Restore()
{
    miNetLobbyJoiningDialogImage->Show(false);
    miNetLobbyJoiningDialogText->Show(false);
    SetHeadControl(miNetLobbyNewRoomButton);
    SetCancelCode(MENU_CODE_TO_NET_SERVERS_MENU);
}
