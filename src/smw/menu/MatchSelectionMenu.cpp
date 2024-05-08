#include "MatchSelectionMenu.h"

#include "FileList.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "path.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"
#include "ui/MI_WorldPreviewDisplay.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern TourList* tourlist;
extern WorldList* worldlist;

UI_MatchSelectionMenu::UI_MatchSelectionMenu()
    : UI_Menu()
{
    miMatchSelectionStartButton = new MI_Button(&rm->spr_selectfield, 270, 420, "Start", 100);
    miMatchSelectionStartButton->SetCode(MENU_CODE_MATCH_SELECTION_START);

    miMatchSelectionField = new MI_SelectField<MatchType>(&rm->spr_selectfield, 130, 340, "Match", 380, 100);
    miMatchSelectionField->add("Single Game", MatchType::SingleGame);
    miMatchSelectionField->add("Tournament", MatchType::Tournament);
    miMatchSelectionField->add("Tour", MatchType::Tour);
    miMatchSelectionField->add("World", MatchType::World);
    miMatchSelectionField->add("Minigame", MatchType::MiniGame);
    miMatchSelectionField->setOutputPtr(&game_values.matchtype);
    miMatchSelectionField->setCurrentValue(game_values.matchtype);
    miMatchSelectionField->setItemChangedCode(MENU_CODE_MATCH_SELECTION_MATCH_CHANGED);

    miTournamentField = new MI_SelectField<short>(&rm->spr_selectfield, 130, 380, "Wins", 380, 100);
    miTournamentField->add("2", 2);
    miTournamentField->add("3", 3);
    miTournamentField->add("4", 4);
    miTournamentField->add("5", 5);
    miTournamentField->add("6", 6);
    miTournamentField->add("7", 7);
    miTournamentField->add("8", 8);
    miTournamentField->add("9", 9);
    miTournamentField->add("10", 10);
    miTournamentField->setOutputPtr(&game_values.tournamentgames);
    miTournamentField->setCurrentValue(game_values.tournamentgames);
    miTournamentField->Show(false);

    miTourField = new MI_SelectField<short>(&rm->spr_selectfield, 130, 380, "Tour", 380, 100);
    for (size_t iTour = 0; iTour < tourlist->count(); iTour++) {
        std::string szTemp = GetNameFromFileName(tourlist->at(iTour), true);
        // strcat(szTemp, " Tour");
        miTourField->add(std::move(szTemp), iTour);
    }
    miTourField->setOutputPtr(&game_values.tourindex);
    miTourField->setCurrentValue(game_values.tourindex);
    miTourField->Show(false);

    miWorldField = new MI_SelectField<short>(&rm->spr_selectfield, 130, 380, "World", 380, 100);
    for (size_t iWorld = 0; iWorld < worldlist->count(); iWorld++) {
        std::string szTemp = GetNameFromFileName(worldlist->at(iWorld), true);
        miWorldField->add(std::move(szTemp), iWorld);
    }
    miWorldField->setOutputPtr(&game_values.worldindex);
    miWorldField->setCurrentValue(game_values.worldindex);
    miWorldField->setItemChangedCode(MENU_CODE_WORLD_MAP_CHANGED);
    miWorldField->Show(false);

    miMinigameField = new MI_SelectField<Minigame>(&rm->spr_selectfield, 130, 380, "Game", 380, 100);
    miMinigameField->add("Pipe Coin Game", Minigame::PipeCoin);
    miMinigameField->add("Hammer Boss Game", Minigame::HammerBoss);
    miMinigameField->add("Bomb Boss Game", Minigame::BombBoss);
    miMinigameField->add("Fire Boss Game", Minigame::FireBoss);
    miMinigameField->add("Boxes Game", Minigame::Boxes);
    miMinigameField->setOutputPtr(&game_values.selectedminigame);
    miMinigameField->setCurrentValue(game_values.selectedminigame);
    miMinigameField->Show(false);

    miMatchSelectionMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miMatchSelectionMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miMatchSelectionMenuHeaderText = new MI_HeaderText("Match Type Menu", 320, 5);

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

    setInitialFocus(miMatchSelectionStartButton);
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);
};

void UI_MatchSelectionMenu::SelectionChanged()
{
    miTournamentField->Show(game_values.matchtype == MatchType::Tournament);
    miTourField->Show(game_values.matchtype == MatchType::Tour);
    miWorldField->Show(game_values.matchtype == MatchType::World);
    miMinigameField->Show(game_values.matchtype == MatchType::MiniGame);

    // miMatchSelectionDisplayImage->Show(game_values.matchtype != MatchType::World);
    miWorldPreviewDisplay->Show(game_values.matchtype == MatchType::World);

    if (game_values.matchtype == MatchType::World)
        miMatchSelectionDisplayImage->SetImage(320, 0, 320, 240);
    else
        miMatchSelectionDisplayImage->SetImage(0, 240 * static_cast<int>(game_values.matchtype), 320, 240);
}

void UI_MatchSelectionMenu::WorldMapChanged()
{
    miWorldPreviewDisplay->SetWorld();
}

void UI_MatchSelectionMenu::ActivateMinigameField()
{
    miMatchSelectionField->hideItem(MatchType::MiniGame, false);
    miMatchSelectionField->setCurrentValue(MatchType::MiniGame);

    miTournamentField->Show(false);
    miTourField->Show(false);
    miWorldField->Show(false);
    miMinigameField->Show(true);

    miMatchSelectionDisplayImage->Show(true);
    miWorldPreviewDisplay->Show(false);
    miMatchSelectionDisplayImage->SetImage(0, 240 * static_cast<int>(game_values.matchtype), 320, 240);
}

Minigame UI_MatchSelectionMenu::GetMinigame() const
{
    return miMinigameField->currentValue();
}

MatchType UI_MatchSelectionMenu::GetSelectedMatchType() const
{
    return miMatchSelectionField->currentValue();
}
