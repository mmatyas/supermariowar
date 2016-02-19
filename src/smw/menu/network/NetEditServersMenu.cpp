#include "NetEditServersMenu.h"

#include "net.h"
#include "ResourceManager.h"

#include <cassert>
#include <cstring>

extern CResourceManager* rm;

UI_NetEditServersMenu::UI_NetEditServersMenu() : UI_Menu()
{
    currentState = DEFAULT;
    std::fill(dialogTextData, dialogTextData + 128, 0);

    miBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miBackButton->SetCode(MENU_CODE_TO_NET_SERVERS_MENU);

    miAddButton = new MI_Button(&rm->spr_selectfield, 40, 40, "Add", 200, 1);
    miAddButton->SetCode(MENU_CODE_NET_ADDREMOVE_SERVER_ON_ADD_BTN);

    miEditButton = new MI_Button(&rm->spr_selectfield, 40, 80, "Edit", 200, 1);
    miEditButton->SetCode(MENU_CODE_NET_ADDREMOVE_SERVER_ON_EDIT_BTN);

    miRemoveButton = new MI_Button(&rm->spr_selectfield, 40, 120, "Remove", 200, 1);
    miRemoveButton->SetCode(MENU_CODE_NET_ADDREMOVE_SERVER_ON_DELETE_BTN);

    miInstructionsText1 = new MI_Text("", 40, 195, 0, 2, 0);
    miInstructionsText2 = new MI_Text("", 40, 215, 0, 2, 0);

    miLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miHeaderText = new MI_Text("Add/Remove Servers Menu", 320, 5, 0, 2, 1);

    miServerScroll = new MI_StringScroll(&rm->menu_plain_field, 260, 32, 350, 9);
    miServerScroll->SetAutoModify(true);
    miServerScroll->SetAcceptCode(MENU_CODE_NET_ADDREMOVE_SERVER_ON_SELECT);
    miServerScroll->SetCancelCode(MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU);
    miServerScroll->Deactivate();

    miDialogTitle = new MI_Text("Enter a new server address below:", 40, 370, 0, 2, 0);
    miDialogTextField = new MI_TextField(&rm->menu_plain_field, 40, 395, "URL or IP", 640 - 2 * 40, 150);
    miDialogTextField->SetData(dialogTextData, 127);
    miDialogOK = new MI_Button(&rm->spr_selectfield, 40, 432, "OK", 100, 1);
    miDialogOK->SetCode(MENU_CODE_NET_ADDREMOVE_SERVER_ON_DIALOG_OK_BTN);
    miDialogCancel = new MI_Button(&rm->spr_selectfield, 150, 432, "Cancel", 100, 1);
    miDialogCancel->SetCode(MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU);

    miDialogTitle->Show(false);
    miDialogTextField->Show(false);
    miDialogOK->Show(false);
    miDialogCancel->Show(false);

    AddControl(miAddButton, miBackButton, miEditButton, NULL, NULL);
    AddControl(miEditButton, miAddButton, miRemoveButton, NULL, NULL);
    AddControl(miRemoveButton, miEditButton, miBackButton, NULL, NULL);
    AddControl(miBackButton, miRemoveButton, miAddButton, NULL, NULL);

    AddNonControl(miLeftHeaderBar);
    AddNonControl(miRightHeaderBar);
    AddNonControl(miHeaderText);
    AddNonControl(miInstructionsText1);
    AddNonControl(miInstructionsText2);

    AddControl(miServerScroll, NULL, NULL, NULL, NULL);

    AddNonControl(miDialogTitle);
    AddControl(miDialogTextField, miDialogOK, miDialogOK, NULL, NULL);
    AddControl(miDialogOK, miDialogTextField, miDialogTextField, miDialogCancel, miDialogCancel);
    AddControl(miDialogCancel, miDialogTextField, miDialogTextField, miDialogOK, miDialogOK);

    SetHeadControl(miAddButton);
    SetCancelCode(MENU_CODE_TO_NET_SERVERS_MENU);
};

UI_NetEditServersMenu::~UI_NetEditServersMenu() {
}

void UI_NetEditServersMenu::ReloadScroll() {
    miServerScroll->ClearItems();
    for (unsigned iServer = 0; iServer < netplay.savedServers.size(); iServer++) {
        ServerAddress* host = &netplay.savedServers[iServer];
        miServerScroll->Add(host->hostname);
    }
}

void UI_NetEditServersMenu::ShowDialog() {
    miDialogTitle->Show(true);
    miDialogTextField->Show(true);
    miDialogOK->Show(true);
    miDialogCancel->Show(true);
}

void UI_NetEditServersMenu::HideDialog() {
    miDialogTitle->Show(false);
    miDialogTextField->Show(false);
    miDialogOK->Show(false);
    miDialogCancel->Show(false);
}

void UI_NetEditServersMenu::Restore() {
    if (savedCurrent)
        RestoreCurrent();

    SetCancelCode(MENU_CODE_TO_NET_SERVERS_MENU);
    currentState = DEFAULT;

    miInstructionsText1->SetText("");
    miInstructionsText2->SetText("");

    HideDialog();
    ReloadScroll();
}

void UI_NetEditServersMenu::onPressAdd() {
    RememberCurrent();
    currentState = ADD;

    ShowDialog();
    SetHeadControl(miDialogTextField);
    SetCancelCode(MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU);
}

void UI_NetEditServersMenu::onPressEdit() {
    if (netplay.savedServers.size() == 0)
        return;

    RememberCurrent();
    currentState = EDIT;

    SetHeadControl(miServerScroll);
    SetCancelCode(MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU);
    miServerScroll->Activate();

    miInstructionsText1->SetText("Select an entry");
    miInstructionsText2->SetText("to edit");
}

void UI_NetEditServersMenu::onPressDelete() {
    if (netplay.savedServers.size() == 0)
        return;

    RememberCurrent();
    currentState = DELETE;

    SetHeadControl(miServerScroll);
    SetCancelCode(MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU);
    miServerScroll->Activate();

    miInstructionsText1->SetText("Select an entry");
    miInstructionsText2->SetText("to delete");
}

void UI_NetEditServersMenu::onEntrySelect() {
    assert(netplay.savedServers.size() > miServerScroll->CurrentIndex());

    switch (currentState) {
    case EDIT:
        strncpy(dialogTextData, netplay.savedServers[miServerScroll->CurrentIndex()].hostname.c_str(), 127);
        miDialogTextField->Refresh();
        ShowDialog();
        SetHeadControl(miDialogTextField);
        SetCancelCode(MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU);
        break;

    case DELETE:
        netplay.savedServers.erase(netplay.savedServers.begin() + miServerScroll->CurrentIndex());
        ReloadScroll();
        miServerScroll->Activate();

        if (netplay.savedServers.size() == 0)
            Restore();

        break;

    default:
        // You should not reach this branch
        assert(false);
    }
}

void UI_NetEditServersMenu::onDialogOk() {
    ServerAddress new_address;
    new_address.hostname = dialogTextData;

    switch (currentState) {
    case ADD:
        netplay.savedServers.push_back(new_address);
        break;

    case EDIT:
        assert(netplay.savedServers.size() > miServerScroll->CurrentIndex());
        netplay.savedServers[miServerScroll->CurrentIndex()] = new_address;
        break;

    default:
        // You should not reach this branch
        assert(false);
    }

    std::fill(dialogTextData, dialogTextData + 128, 0);
    miDialogTextField->Refresh();
    Restore();
}
