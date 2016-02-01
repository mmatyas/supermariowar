#include "NetNewRoomMenu.h"

#include "uicontrol.h"
#include "ui/MI_SelectField.h"

#include "net.h"
#include "ResourceManager.h"

#include <cstring>

extern CResourceManager* rm;

UI_NetNewRoomMenu::UI_NetNewRoomMenu() : UI_Menu()
{
    miNetNewRoomNameField = new MI_TextField(&rm->menu_plain_field, 70, 160, "Room name", 640 - 2 * 70, 230);
    miNetNewRoomNameField->SetData(netplay.newroom_name, NET_MAX_ROOM_NAME_LENGTH);

    miNetNewRoomPasswordField = new MI_TextField(&rm->menu_plain_field, 70, 200, "Password (optional)", 640 - 2 * 70, 230);
    miNetNewRoomPasswordField->SetData(netplay.newroom_password, NET_MAX_ROOM_PASSWORD_LENGTH);

    miNetNewRoomCreateButton = new MI_Button(&rm->spr_selectfield, 70, 240, "Create!", 640 - 2 * 70, 1);
    miNetNewRoomCreateButton->SetCode(MENU_CODE_TO_NET_NEW_ROOM_CREATE_IN_PROGRESS);

    miNetNewRoomBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miNetNewRoomBackButton->SetCode(MENU_CODE_TO_NET_NEW_ROOM_LEVEL_SELECT_MENU);

    AddControl(miNetNewRoomNameField, miNetNewRoomBackButton, miNetNewRoomPasswordField, NULL, NULL);
    AddControl(miNetNewRoomPasswordField, miNetNewRoomNameField, miNetNewRoomCreateButton, NULL, NULL);
    AddControl(miNetNewRoomCreateButton, miNetNewRoomPasswordField, miNetNewRoomBackButton, NULL, NULL);
    AddControl(miNetNewRoomBackButton, miNetNewRoomCreateButton, miNetNewRoomNameField, NULL, NULL);

    miNetNewRoomCreatingDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miNetNewRoomCreatingDialogText = new MI_Text("Creating...", 320, 240 - 12, 0, 2, 1);

    miNetNewRoomCreatingDialogImage->Show(false);
    miNetNewRoomCreatingDialogText->Show(false);

    AddNonControl(miNetNewRoomCreatingDialogImage);
    AddNonControl(miNetNewRoomCreatingDialogText);

    miNetNewRoomLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miNetNewRoomRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miNetNewRoomHeaderText = new MI_Text("New Room Settings Menu", 320, 5, 0, 2, 1);

    AddNonControl(miNetNewRoomLeftHeaderBar);
    AddNonControl(miNetNewRoomRightHeaderBar);
    AddNonControl(miNetNewRoomHeaderText);

    SetHeadControl(miNetNewRoomNameField);
    SetCancelCode(MENU_CODE_TO_NET_NEW_ROOM_LEVEL_SELECT_MENU);
};

UI_NetNewRoomMenu::~UI_NetNewRoomMenu() {
}

void UI_NetNewRoomMenu::CreateInProgress()
{
    if (strlen(netplay.newroom_name) > 2) {
        netplay.client.sendCreateRoomMessage();
        netplay.operationInProgress = true;

        miNetNewRoomCreatingDialogImage ->Show(true);
        miNetNewRoomCreatingDialogText->Show(true);

        RememberCurrent();
        SetHeadControl(miNetNewRoomCreatingDialogText);
        SetCancelCode(MENU_CODE_TO_NET_NEW_ROOM_CREATE_ABORT);
        ResetMenu();
    }
    else
        printf("[net] Room name is too short!\n"); // TODO: dialog box
}

void UI_NetNewRoomMenu::AbortCreate()
{
    netplay.operationInProgress = false;
    Restore();
}

void UI_NetNewRoomMenu::Restore()
{
    miNetNewRoomCreatingDialogImage->Show(false);
    miNetNewRoomCreatingDialogText->Show(false);
    SetHeadControl(miNetNewRoomNameField);
    SetCancelCode(MENU_CODE_TO_NET_NEW_ROOM_LEVEL_SELECT_MENU);
}
