#include "PlayerControlsMenu.h"

#include "ResourceManager.h"
#include "ui/MI_InputControlContainer.h"

extern CResourceManager* rm;


UI_PlayerControlsMenu::UI_PlayerControlsMenu()
    : UI_Menu()
{
    miInputContainer = new MI_InputControlContainer(&rm->menu_plain_field, 94, 10, 0);
    miInputContainer->SetAutoModify(true);

    AddControl(miInputContainer, NULL, NULL, NULL, NULL);
    setInitialFocus(miInputContainer);
    SetCancelCode(MENU_CODE_BACK_TO_CONTROLS_MENU);
};

void UI_PlayerControlsMenu::SetPlayer(short playerID)
{
    miInputContainer->SetPlayer(playerID);
}
