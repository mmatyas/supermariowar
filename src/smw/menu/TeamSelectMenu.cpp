#include "TeamSelectMenu.h"

#include "ResourceManager.h"
#include "ui/MI_Image.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;

UI_TeamSelectMenu::UI_TeamSelectMenu() : UI_Menu()
{
    miTeamSelect = new MI_TeamSelect(&rm->spr_player_select_background, 112, 96);
    miTeamSelect->SetAutoModify(true);

    miTeamSelectLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTeamSelectRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTeamSelectHeaderText = new MI_HeaderText("Team and Character Selection", 320, 5);

    AddControl(miTeamSelect, NULL, NULL, NULL, NULL);

    AddNonControl(miTeamSelectLeftHeaderBar);
    AddNonControl(miTeamSelectRightHeaderBar);
    AddNonControl(miTeamSelectHeaderText);

    SetHeadControl(miTeamSelect);
    SetCancelCode(MENU_CODE_BACK_TO_MATCH_SELECTION_MENU);
}

UI_TeamSelectMenu::~UI_TeamSelectMenu() {
}

void UI_TeamSelectMenu::ResetTeamSelect()
{
    miTeamSelect->Reset();
}

short UI_TeamSelectMenu::GetTeamCount()
{
    return miTeamSelect->OrganizeTeams();
}
