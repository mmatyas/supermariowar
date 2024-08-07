#include "NetServersMenu.h"

#include "ResourceManager.h"
#include "net.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_Text.h"
#include "ui/MI_TextField.h"
#include "ui/NetworkListScroll.h"

extern CResourceManager* rm;


UI_NetServersMenu::UI_NetServersMenu()
    : UI_Menu()
{
    miNetServersBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miNetServersBackButton->SetCode(MENU_CODE_TO_MAIN_MENU);

    miNetServersLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miNetServersRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miNetServersHeaderText = new MI_HeaderText("Multiplayer Servers Menu", 320, 5);

    miNetServersScroll = new MI_NetworkListScroll(&rm->menu_plain_field, 90, 72, 640 - 2 * 90, 9, "Saved Servers", MENU_CODE_NET_SERVERLIST_EXIT, MENU_CODE_NET_SERVERLIST_EXIT);
    miNetServersScroll->SetAutoModify(true);
    miNetServersScroll->setVisible(false);
    miNetServersScroll->RemoteIndex(&netplay.selectedServerIndex);

    miNetServersNicknameField = new MI_TextField(&rm->menu_plain_field, 70, 120, "Your name", 640 - 2 * 70, 150);
    miNetServersNicknameField->SetData(netplay.myPlayerName, NET_MAX_PLAYER_NAME_LENGTH);

    for (unsigned iServer = 0; iServer < netplay.savedServers.size(); iServer++) {
        ServerAddress* host = &netplay.savedServers[iServer];
        miNetServersScroll->Add(host->hostname, "");
    }

    miNetServersSelectButton = new MI_Button(&rm->spr_selectfield, 70, 200, "Selected Server", 640 - 2 * 70);
    miNetServersSelectButton->SetCode(MENU_CODE_TO_NET_SERVERLIST);
    if (netplay.savedServers.size() > 0)
        miNetServersSelectedHostText = new MI_Text(netplay.savedServers[netplay.selectedServerIndex].hostname.c_str(), 640 - 90, 205, 0, true, TextAlign::RIGHT);
    else
        miNetServersSelectedHostText = new MI_Text("(none)", 640 - 90, 205, 0, true, TextAlign::RIGHT);

    miNetServersConnectButton = new MI_Button(&rm->spr_selectfield, 70, 240, "Connect", 640 - 2 * 70, TextAlign::CENTER);
    miNetServersConnectButton->SetCode(MENU_CODE_NET_CONNECT_IN_PROGRESS);

    miNetServersAddRemoveButton = new MI_Button(&rm->spr_selectfield, 70, 280, "Add / Remove Server", 640 - 2 * 70, TextAlign::CENTER);
    miNetServersAddRemoveButton->SetCode(MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU);

    miNetServersConnectingDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miNetServersConnectingDialogText = new MI_HeaderText("Connecting...", 640 / 2, 240 - 12);

    miNetServersConnectingDialogImage->setVisible(false);
    miNetServersConnectingDialogText->setVisible(false);
    miNetServersConnectingDialogText->Disable(true);

    // miNetServersConnectionDetector = new MI_NetworkStatusDetector(&rm->spr_selectfield, 320, 240 - 10, "Connecting...", 80, 1);
    /*miNetServersConnectionDetector = new MI_NetworkStatusDetector(0, 0);
    miNetServersConnectingDialogImage->Show(false);
    miNetServersConnectionDetector->Show(false);*/
    // miNetServersConnectionDetector->SetAutoModify(true);

    // miNetServersConnectionDetector->SetCode(MENU_CODE_NET_CONNECT_ABORT);
    /*miNetServersConnectionDetector->SuccessIfTrue(&netplay.connectSuccessful);
    miNetServersConnectionDetector->AbortCode(MENU_CODE_NET_CONNECT_ABORT);
    miNetServersConnectionDetector->SuccessCode(MENU_CODE_NET_CONNECT_SUCCESS);
    miNetServersConnectionDetector->WaitingCode(MENU_CODE_NET_CONNECT_IN_PROGRESS);*/

    AddControl(miNetServersNicknameField, miNetServersBackButton, miNetServersSelectButton, NULL, NULL);
    AddControl(miNetServersSelectButton, miNetServersNicknameField, miNetServersConnectButton, NULL, NULL);
    AddNonControl(miNetServersSelectedHostText);
    AddControl(miNetServersConnectButton, miNetServersSelectButton, miNetServersAddRemoveButton, NULL, NULL);
    AddControl(miNetServersAddRemoveButton, miNetServersConnectButton, miNetServersBackButton, NULL, NULL);
    AddControl(miNetServersBackButton, miNetServersAddRemoveButton, miNetServersNicknameField, NULL, NULL);

    AddNonControl(miNetServersLeftHeaderBar);
    AddNonControl(miNetServersRightHeaderBar);
    AddNonControl(miNetServersHeaderText);

    AddNonControl(miNetServersConnectingDialogImage);
    AddNonControl(miNetServersConnectingDialogText);

    AddControl(miNetServersScroll, NULL, NULL, NULL, NULL);
    // AddNonControl(miNetServersConnectingDialogImage);
    // AddControl(miNetServersConnectionDetector, NULL, NULL, NULL, NULL);

    setInitialFocus(miNetServersSelectButton);
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);
};

void UI_NetServersMenu::Refresh()
{
    if (netplay.savedServers.size() > 0) {
        const char* nethostname = netplay.savedServers[netplay.selectedServerIndex].hostname.c_str();
        miNetServersSelectedHostText->SetText(nethostname);
    }
}

void UI_NetServersMenu::RefreshScroll()
{
    miNetServersScroll->Clear();
    for (unsigned iServer = 0; iServer < netplay.savedServers.size(); iServer++) {
        ServerAddress* host = &netplay.savedServers[iServer];
        miNetServersScroll->Add(host->hostname, "");
    }

    if (netplay.savedServers.size() > 0)
        miNetServersSelectedHostText->SetText(netplay.savedServers[netplay.selectedServerIndex].hostname.c_str());
    else
        miNetServersSelectedHostText->SetText("(none)");
}

void UI_NetServersMenu::ConnectInProgress()
{
    if (!netplay.savedServers.size())
        return;

    netplay.client.sendConnectRequestToSelectedServer();
    netplay.operationInProgress = true;

    miNetServersConnectingDialogImage->setVisible(true);
    miNetServersConnectingDialogText->setVisible(true);
    RememberCurrent();

    setInitialFocus(miNetServersConnectingDialogText);
    SetCancelCode(MENU_CODE_NET_CONNECT_ABORT);
    ResetMenu();
}

void UI_NetServersMenu::OpenServerList()
{
    netplay.connectSuccessful = false;
    miNetServersScroll->setVisible(true);
    RememberCurrent();

    setInitialFocus(miNetServersScroll);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_NetServersMenu::Restore()
{
    miNetServersScroll->setVisible(false);
    miNetServersConnectingDialogImage->setVisible(false);
    miNetServersConnectingDialogText->setVisible(false);

    setInitialFocus(miNetServersSelectButton);
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);
    RestoreCurrent();
}
