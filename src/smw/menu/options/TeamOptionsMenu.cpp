#include "TeamOptionsMenu.h"

#include "uicontrol.h"
#include "ui/MI_SelectField.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_TeamOptionsMenu::UI_TeamOptionsMenu() : UI_Menu()
{
	miTeamKillsField = new MI_SelectField(&rm->spr_selectfield, 70, 180, "Player Collision", 500, 220);
    miTeamKillsField->Add("Off", 0, "", false, false);
    miTeamKillsField->Add("Assist", 1, "", false, false);
    miTeamKillsField->Add("On", 2, "", false, false);
    miTeamKillsField->SetData(&game_values.teamcollision, NULL, NULL);
    miTeamKillsField->SetKey(game_values.teamcollision);

    miTeamColorsField = new MI_SelectField(&rm->spr_selectfield, 70, 220, "Colors", 500, 220);
    miTeamColorsField->Add("Individual", 0, "", false, false);
    miTeamColorsField->Add("Team", 1, "", true, false);
    miTeamColorsField->SetData(NULL, NULL, &game_values.teamcolors);
    miTeamColorsField->SetKey(game_values.teamcolors ? 1 : 0);
    miTeamColorsField->SetAutoAdvance(true);

    miTournamentControlField = new MI_SelectField(&rm->spr_selectfield, 70, 260, "Tournament Control", 500, 220);
    miTournamentControlField->Add("All", 0, "", false, false);
    miTournamentControlField->Add("Game Winner", 1, "", false, false);
    miTournamentControlField->Add("Game Loser", 2, "", false, false);
    miTournamentControlField->Add("Leading Teams", 3, "", false, false);
    miTournamentControlField->Add("Trailing Teams", 4, "", false, false);
    miTournamentControlField->Add("Random", 5, "", false, false);
    miTournamentControlField->Add("Random Loser", 6, "", false, false);
    miTournamentControlField->Add("Round Robin", 7, "", false, false);
    miTournamentControlField->SetData(&game_values.tournamentcontrolstyle, NULL, NULL);
    miTournamentControlField->SetKey(game_values.tournamentcontrolstyle);

    miTeamOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miTeamOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miTeamOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTeamOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTeamOptionsMenuHeaderText = new MI_Text("Team Options Menu", 320, 5, 0, 2, 1);

    AddControl(miTeamKillsField, miTeamOptionsMenuBackButton, miTeamColorsField, NULL, miTeamOptionsMenuBackButton);
    AddControl(miTeamColorsField, miTeamKillsField, miTournamentControlField, NULL, miTeamOptionsMenuBackButton);
    AddControl(miTournamentControlField, miTeamColorsField, miTeamOptionsMenuBackButton, NULL, miTeamOptionsMenuBackButton);
    AddControl(miTeamOptionsMenuBackButton, miTournamentControlField, miTeamKillsField, miTournamentControlField, NULL);

    AddNonControl(miTeamOptionsMenuLeftHeaderBar);
    AddNonControl(miTeamOptionsMenuRightHeaderBar);
    AddNonControl(miTeamOptionsMenuHeaderText);

    SetHeadControl(miTeamKillsField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};

UI_TeamOptionsMenu::~UI_TeamOptionsMenu() {
}
