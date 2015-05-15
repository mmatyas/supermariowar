#include "PlayerControlsMenu.h"

#include "ResourceManager.h"

extern CResourceManager* rm;

UI_PlayerControlsMenu::UI_PlayerControlsMenu() : UI_Menu()
{
    miInputContainer = new MI_InputControlContainer(&rm->menu_plain_field, 94, 10, 0);
    miInputContainer->SetAutoModify(true);

    AddControl(miInputContainer, NULL, NULL, NULL, NULL);
    SetHeadControl(miInputContainer);
    SetCancelCode(MENU_CODE_BACK_TO_CONTROLS_MENU);
};

UI_PlayerControlsMenu::~UI_PlayerControlsMenu() {
}

void UI_PlayerControlsMenu::SetPlayer(short playerID)
{
	miInputContainer->SetPlayer(playerID);
}
