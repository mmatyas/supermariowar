#include "TournamentScoreboardMenu.h"

#include "ResourceManager.h"

extern CResourceManager* rm;

UI_TournamentScoreboardMenu::UI_TournamentScoreboardMenu() : UI_Menu()
{
    miTournamentScoreboard = new MI_TournamentScoreboard(&rm->spr_tournament_background, 70, 98);

    miTournamentScoreboardNextButton = new MI_Button(&rm->spr_selectfield, 220, 416, "Next", 200, 1);
    miTournamentScoreboardNextButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU);

    miTournamentScoreboardImage = new MI_Image(&rm->spr_scoreboard, 124, 12, 0, 0, 386, 59, 1, 1, 0);

    AddNonControl(miTournamentScoreboard);
    AddNonControl(miTournamentScoreboardImage);
    AddControl(miTournamentScoreboardNextButton, NULL, NULL, NULL, NULL);
    SetHeadControl(miTournamentScoreboardNextButton);
    SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU);
};

UI_TournamentScoreboardMenu::~UI_TournamentScoreboardMenu() {
}
