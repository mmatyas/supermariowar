#include "NetRoomMenu.h"

#include "GameValues.h"
#include "net.h"
#include "path.h"
#include "ResourceManager.h"

#include <sstream>

extern CResourceManager* rm;
extern CGameValues game_values;

extern short LookupTeamID(short id);

UI_NetRoomMenu::UI_NetRoomMenu() : UI_Menu()
{
    miNetRoomName = new MI_Text("" /* Room name here */, 40, 50, 0, 2, 0);
    //AddNonControl(miNetRoomName);

    miNetRoomStartButton = new MI_Button(&rm->spr_selectfield, 300, 310, "(waiting)", 331, 1);
    miNetRoomStartButton->SetCode(MENU_CODE_TO_NET_ROOM_START_IN_PROGRESS);

    miNetRoomMessages = new MI_ChatMessageBox(20, 350, 640 - 2 * 26, 1);
    miNetRoomMapPreview = new MI_MapPreview(&rm->spr_selectfield, 264, 2, 400, 120);

    miNetRoomMessageField = new MI_TextField(&rm->menu_plain_field, 26, 432, "Say", 464 - 36, 60);
    miNetRoomMessageField->SetData(netplay.mychatmessage, NET_MAX_CHAT_MSG_LENGTH);

    miNetRoomSendButton = new MI_Button(&rm->spr_selectfield, 464, 432, "Send", 80, 1);
    miNetRoomSendButton->SetCode(MENU_CODE_NET_CHAT_SEND);

    miNetRoomBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Leave", 80, 1);
    miNetRoomBackButton->SetCode(MENU_CODE_TO_NET_LOBBY_MENU);

    for (short p = 0; p < 4; p++) {
        miNetRoomPlayerName[p] = new MI_Text("" /* Px name here */, 60, 80 + p * 60, 0, 2, 0);
        AddNonControl(miNetRoomPlayerName[p]);

        miSkinSelector[p] = new MI_NetRoomTeamSelect(16, 72 + p * 60, p, [](){
            netplay.client.sendSkinChange();
        });
        AddControl(miSkinSelector[p], miNetRoomMessageField, miNetRoomStartButton, NULL, NULL);
    }

    AddNonControl(miNetRoomMessages);
    AddNonControl(miNetRoomMapPreview);

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
    miNetRoomHeaderText->SetText(netplay.currentRoom.name.c_str());

    miNetRoomStartButton->SetNeighbor(MENU_ITEM_NEIGHBOR_UP, miSkinSelector[netplay.remotePlayerNumber]);
    miNetRoomMessageField->SetNeighbor(MENU_ITEM_NEIGHBOR_DOWN, miSkinSelector[netplay.remotePlayerNumber]);

    for (short p = 0; p < 4; p++) {
        miNetRoomPlayerName[p]->SetText(netplay.currentRoom.playerNames[p].c_str());

        // Reload player skins
        std::ostringstream path;
        path << GetHomeDirectory() << "net_skin" << p << ".bmp";
        if (p == netplay.remotePlayerNumber) {
            printf("  player %d -> local\n", p);
            rm->LoadMenuSkin(p, game_values.skinids[0], p, false);
        }
        else if (File_Exists(path.str())) {
            printf("  player %d -> %s\n", p, path.str().c_str());
            if (!rm->LoadMenuSkin(p, path.str(), p, false))
                rm->LoadMenuSkin(p, game_values.skinids[p], p, false);
        }
        else {
            printf("  player %d -> default\n", p);
            rm->LoadMenuSkin(p, 0, p, false);
        }
    }

    if (netplay.theHostIsMe && netplay.currentRoom.playerCount() > 1)
        miNetRoomStartButton->SetName("Start");
    else
        miNetRoomStartButton->SetName("(waiting)");

    SetPreviewMapPath(netplay.mapfilepath);
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
}

void UI_NetRoomMenu::SetPreviewMapPath(std::string& path)
{
    miNetRoomMapPreview->LoadMap(path.c_str());
}
