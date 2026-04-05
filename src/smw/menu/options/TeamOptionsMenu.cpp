#include "TeamOptionsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;
extern CGameValues game_values;


UI_TeamOptionsMenu::UI_TeamOptionsMenu()
    : UI_Menu()
{
    miTeamKillsField = new MI_SelectField<TeamCollisionStyle>(&rm->spr_selectfield, 70, 180, "Player Collision", 500, 220);
    miTeamKillsField->add("Off", TeamCollisionStyle::Off);
    miTeamKillsField->add("Assist", TeamCollisionStyle::Assist);
    miTeamKillsField->add("On", TeamCollisionStyle::On);
    miTeamKillsField->setOutputPtr(&game_values.teamcollision);
    miTeamKillsField->setCurrentValue(game_values.teamcollision);

    miTeamColorsField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 220, "Colors", 500, 220);
    miTeamColorsField->add("Individual", false);
    miTeamColorsField->add("Team", true);
    miTeamColorsField->setOutputPtr(&game_values.teamcolors);
    miTeamColorsField->setCurrentValue(game_values.teamcolors ? 1 : 0);
    miTeamColorsField->setAutoAdvance(true);

    miTournamentControlField = new MI_SelectField<TournamentControlStyle>(&rm->spr_selectfield, 70, 260, "Tournament Control", 500, 220);
    miTournamentControlField->add("All", TournamentControlStyle::All);
    miTournamentControlField->add("Game Winner", TournamentControlStyle::GameWinner);
    miTournamentControlField->add("Game Loser", TournamentControlStyle::GameLoser);
    miTournamentControlField->add("Leading Teams", TournamentControlStyle::LeadingTeams);
    miTournamentControlField->add("Trailing Teams", TournamentControlStyle::TrailingTeams);
    miTournamentControlField->add("Random", TournamentControlStyle::Random);
    miTournamentControlField->add("Random Loser", TournamentControlStyle::RandomLoser);
    miTournamentControlField->add("Round Robin", TournamentControlStyle::RoundRobin);
    miTournamentControlField->setOutputPtr(&game_values.tournamentcontrolstyle);
    miTournamentControlField->setCurrentValue(game_values.tournamentcontrolstyle);

    miTeamOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miTeamOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miTeamOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTeamOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTeamOptionsMenuHeaderText = new MI_HeaderText("Team Options Menu", 320, 5);

    AddControl(miTeamKillsField, miTeamOptionsMenuBackButton, miTeamColorsField, NULL, miTeamOptionsMenuBackButton);
    AddControl(miTeamColorsField, miTeamKillsField, miTournamentControlField, NULL, miTeamOptionsMenuBackButton);
    AddControl(miTournamentControlField, miTeamColorsField, miTeamOptionsMenuBackButton, NULL, miTeamOptionsMenuBackButton);
    AddControl(miTeamOptionsMenuBackButton, miTournamentControlField, miTeamKillsField, miTournamentControlField, NULL);

    AddNonControl(miTeamOptionsMenuLeftHeaderBar);
    AddNonControl(miTeamOptionsMenuRightHeaderBar);
    AddNonControl(miTeamOptionsMenuHeaderText);

    setInitialFocus(miTeamKillsField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};
