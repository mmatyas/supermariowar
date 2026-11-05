#include "PlayerControlsSelectMenu.h"

#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_Text.h"

#include <string>

extern CResourceManager* rm;

UI_PlayerControlsSelectMenu::UI_PlayerControlsSelectMenu()
    : UI_Menu()
{
    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        const std::string name = "Player " + std::to_string(iPlayer + 1);
        miPlayerControlsButtons[iPlayer] = new MI_Button(&rm->spr_selectfield, 120, 80 + iPlayer * 40, name, 400, TextAlign::CENTER);
        miPlayerControlsButtons[iPlayer]->SetCode(static_cast<MenuCodeEnum>(MENU_CODE_TO_PLAYER_1_CONTROLS + iPlayer));
    }

    miPlayerControlsBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miPlayerControlsBackButton->SetCode(MENU_CODE_TO_MAIN_MENU);

    miPlayerControlsLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miPlayerControlsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miPlayerControlsMenuHeaderText = new MI_HeaderText("Player Controls Menu", 320, 5);

    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        MI_Button* up = iPlayer > 0 ? miPlayerControlsButtons[iPlayer - 1] : miPlayerControlsBackButton;
        MI_Button* down = iPlayer < MAX_PLAYERS - 1 ? miPlayerControlsButtons[iPlayer + 1] : miPlayerControlsBackButton;
        AddControl(miPlayerControlsButtons[iPlayer], up, down, NULL, miPlayerControlsBackButton);
    }
    AddControl(miPlayerControlsBackButton, miPlayerControlsButtons.back(), miPlayerControlsButtons.front(), miPlayerControlsButtons.front(), NULL);

    AddNonControl(miPlayerControlsLeftHeaderBar);
    AddNonControl(miPlayerControlsMenuRightHeaderBar);
    AddNonControl(miPlayerControlsMenuHeaderText);

    setInitialFocus(miPlayerControlsButtons.front());
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);
}
