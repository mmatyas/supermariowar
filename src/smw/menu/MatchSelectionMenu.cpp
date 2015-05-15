#include "MatchSelectionMenu.h"

#include "FileList.h"
#include "GameValues.h"
#include "path.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern TourList *tourlist;
extern WorldList *worldlist;

UI_MatchSelectionMenu::UI_MatchSelectionMenu() : UI_Menu()
{
	miMatchSelectionStartButton = new MI_Button(&rm->spr_selectfield, 270, 420, "Start", 100, 0);
    miMatchSelectionStartButton->SetCode(MENU_CODE_MATCH_SELECTION_START);

    miMatchSelectionField = new MI_SelectField(&rm->spr_selectfield, 130, 340, "Match", 380, 100);
    miMatchSelectionField->Add("Single Game", MATCH_TYPE_SINGLE_GAME, "", false, false);
    miMatchSelectionField->Add("Tournament", MATCH_TYPE_TOURNAMENT, "", false, false);
    miMatchSelectionField->Add("Tour", MATCH_TYPE_TOUR, "", false, false);
    miMatchSelectionField->Add("World", MATCH_TYPE_WORLD, "", false, false);
    miMatchSelectionField->Add("Minigame", MATCH_TYPE_MINIGAME, "", false, !game_values.minigameunlocked);
    miMatchSelectionField->SetData(&game_values.matchtype, NULL, NULL);
    miMatchSelectionField->SetKey(game_values.matchtype);
    miMatchSelectionField->SetItemChangedCode(MENU_CODE_MATCH_SELECTION_MATCH_CHANGED);

    miTournamentField = new MI_SelectField(&rm->spr_selectfield, 130, 380, "Wins", 380, 100);
    miTournamentField->Add("2", 2, "", false, false);
    miTournamentField->Add("3", 3, "", false, false);
    miTournamentField->Add("4", 4, "", false, false);
    miTournamentField->Add("5", 5, "", false, false);
    miTournamentField->Add("6", 6, "", false, false);
    miTournamentField->Add("7", 7, "", false, false);
    miTournamentField->Add("8", 8, "", false, false);
    miTournamentField->Add("9", 9, "", false, false);
    miTournamentField->Add("10", 10, "", false, false);
    miTournamentField->SetData(&game_values.tournamentgames, NULL, NULL);
    miTournamentField->SetKey(game_values.tournamentgames);
    miTournamentField->Show(false);

    char szTemp[256];
    miTourField = new MI_SelectField(&rm->spr_selectfield, 130, 380, "Tour", 380, 100);
    for (short iTour = 0; iTour < tourlist->GetCount(); iTour++) {
        GetNameFromFileName(szTemp, tourlist->GetIndex(iTour), true);
        //strcat(szTemp, " Tour");
        miTourField->Add(szTemp, iTour, "", true, false);
    }
    miTourField->SetData(&game_values.tourindex, NULL, NULL);
    miTourField->SetKey(game_values.tourindex);
    miTourField->Show(false);

    miWorldField = new MI_SelectField(&rm->spr_selectfield, 130, 380, "World", 380, 100);
    for (short iWorld = 0; iWorld < worldlist->GetCount(); iWorld++) {
        GetNameFromFileName(szTemp, worldlist->GetIndex(iWorld), true);
        miWorldField->Add(szTemp, iWorld, "", true, false);
    }
    miWorldField->SetData(&game_values.worldindex, NULL, NULL);
    miWorldField->SetKey(game_values.worldindex);
    miWorldField->SetItemChangedCode(MENU_CODE_WORLD_MAP_CHANGED);
    miWorldField->Show(false);

    miMinigameField = new MI_SelectField(&rm->spr_selectfield, 130, 380, "Game", 380, 100);
    miMinigameField->Add("Pipe Coin Game", 0, "", false, false);
    miMinigameField->Add("Hammer Boss Game", 1, "", false, false);
    miMinigameField->Add("Bomb Boss Game", 2, "", false, false);
    miMinigameField->Add("Fire Boss Game", 3, "", false, false);
    miMinigameField->Add("Boxes Game", 4, "", false, false);
    miMinigameField->SetData(&game_values.selectedminigame, NULL, NULL);
    miMinigameField->SetKey(game_values.selectedminigame);
    miMinigameField->Show(false);

    miMatchSelectionMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miMatchSelectionMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miMatchSelectionMenuHeaderText = new MI_Text("Match Type Menu", 320, 5, 0, 2, 1);

    miMatchSelectionDisplayImage = new MI_Image(&rm->menu_match_select, 160, 80, 0, 0, 320, 240, 1, 1, 0);
    miWorldPreviewDisplay = new MI_WorldPreviewDisplay(160, 80, 20, 15);
    miWorldPreviewDisplay->Show(false);

    AddNonControl(miMatchSelectionMenuLeftHeaderBar);
    AddNonControl(miMatchSelectionMenuRightHeaderBar);
    AddNonControl(miMatchSelectionMenuHeaderText);

    AddNonControl(miWorldPreviewDisplay);
    AddNonControl(miMatchSelectionDisplayImage);

    AddControl(miMatchSelectionField, miMatchSelectionStartButton, miTournamentField, NULL, NULL);
    AddControl(miTournamentField, miMatchSelectionField, miTourField, NULL, NULL);
    AddControl(miTourField, miTournamentField, miWorldField, NULL, NULL);
    AddControl(miWorldField, miTourField, miMinigameField, NULL, NULL);
    AddControl(miMinigameField, miWorldField, miMatchSelectionStartButton, NULL, NULL);
    AddControl(miMatchSelectionStartButton, miMinigameField, miMatchSelectionField, NULL, NULL);

    SetHeadControl(miMatchSelectionStartButton);
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);
};

UI_MatchSelectionMenu::~UI_MatchSelectionMenu() {
}

void UI_MatchSelectionMenu::SelectionChanged()
{
    miTournamentField->Show(game_values.matchtype == MATCH_TYPE_TOURNAMENT);
    miTourField->Show(game_values.matchtype == MATCH_TYPE_TOUR);
    miWorldField->Show(game_values.matchtype == MATCH_TYPE_WORLD);
    miMinigameField->Show(game_values.matchtype == MATCH_TYPE_MINIGAME);

    //miMatchSelectionDisplayImage->Show(game_values.matchtype != MATCH_TYPE_WORLD);
    miWorldPreviewDisplay->Show(game_values.matchtype == MATCH_TYPE_WORLD);

    if (game_values.matchtype == MATCH_TYPE_WORLD)
        miMatchSelectionDisplayImage->SetImage(320, 0, 320, 240);
    else
        miMatchSelectionDisplayImage->SetImage(0, 240 * game_values.matchtype, 320, 240);
}

void UI_MatchSelectionMenu::WorldMapChanged()
{
    miWorldPreviewDisplay->SetWorld();
}

void UI_MatchSelectionMenu::ActivateMinigameField()
{
    miMatchSelectionField->HideItem(MATCH_TYPE_MINIGAME, false);
    miMatchSelectionField->SetKey(MATCH_TYPE_MINIGAME);

    miTournamentField->Show(false);
    miTourField->Show(false);
    miWorldField->Show(false);
    miMinigameField->Show(true);

    miMatchSelectionDisplayImage->Show(true);
    miWorldPreviewDisplay->Show(false);
    miMatchSelectionDisplayImage->SetImage(0, 240 * game_values.matchtype, 320, 240);
}

short UI_MatchSelectionMenu::GetMinigameID()
{
     return miMinigameField->GetShortValue();
}

short UI_MatchSelectionMenu::GetSelectedMatchType()
{
    return miMatchSelectionField->GetShortValue();
}
