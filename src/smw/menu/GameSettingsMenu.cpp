#include "GameSettingsMenu.h"

#include "FileList.h"
#include "GameMode.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "path.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_ImageSelectField.h"
#include "ui/MI_MapField.h"
#include "ui/MI_MapFilterScroll.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

extern CGameMode* gamemodes[GAMEMODE_LAST];
extern short currentgamemode;

extern CGameValues game_values;
extern CResourceManager* rm;
extern FiltersList* filterslist;

namespace {
const char* g_szAutoFilterNames[NUM_AUTO_FILTERS] {
    "Death Tiles",
    "Warps",
    "Ice",
    "Item Boxes",
    "Breakable Blocks",
    "Throwable Blocks",
    "On/Off Blocks",
    "Platforms",
    "Hazards",
    "Item Destroyable Blocks",
    "Hidden Blocks",
    "Map Items",
};
const short g_iAutoFilterIcons[NUM_AUTO_FILTERS] {37, 29, 33, 1, 0, 6, 40, 73, 19, 87, 17, 118};
} // namespace


UI_GameSettingsMenu::UI_GameSettingsMenu()
    : UI_Menu()
{
    miSettingsStartButton = new MI_Button(&rm->spr_selectfield, 70, 45, "Start", 500);
    miSettingsStartButton->SetCode(MENU_CODE_START_GAME);

    miModeField = new MI_ImageSelectField(&rm->spr_selectfield, &rm->menu_mode_small, 70, 85, "Mode", 500, 120, 16, 16);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
        miModeField->add(gamemodes[iGameMode]->GetModeName(), iGameMode);
    }
    miModeField->setOutputPtr(&currentgamemode);
    miModeField->setCurrentValue(0);
    miModeField->setItemChangedCode(MENU_CODE_MODE_CHANGED);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
        miGoalField[iGameMode] = new MI_SelectField<short>(&rm->spr_selectfield, 70, 125, gamemodes[iGameMode]->GetGoalName().c_str(), 352, 120);
        // miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
        miGoalField[iGameMode]->setVisible(iGameMode == 0);

        for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS; iGameModeOption++) {
            SModeOption* option = &gamemodes[iGameMode]->GetOptions()[iGameModeOption];
            miGoalField[iGameMode]->add(option->szName, option->iValue);
        }

        miGoalField[iGameMode]->setOutputPtr(&gamemodes[iGameMode]->goal);
        miGoalField[iGameMode]->setCurrentValue(gamemodes[iGameMode]->goal);
    }

    miModeSettingsButton = new MI_Button(&rm->spr_selectfield, 430, 125, "Settings", 140);
    miModeSettingsButton->SetCode(MENU_CODE_TO_MODE_SETTINGS_MENU);

    miMapField = new MI_MapField(&rm->spr_selectfield, 70, 165, "Map", 500, 120, true);

    miMapFiltersButton = new MI_Button(&rm->spr_selectfield, 430, 205, "Filters", 140);
    miMapFiltersButton->SetCode(MENU_CODE_TO_MAP_FILTERS);

    miMapFiltersOnImage = new MI_Image(&rm->menu_map_filter, 530, 213, 0, 48, 16, 16, 1, 1, 0);
    miMapFiltersOnImage->setVisible(false);

    miMapThumbnailsButton = new MI_Button(&rm->spr_selectfield, 430, 245, "Thumbs", 140);
    miMapThumbnailsButton->SetCode(MENU_CODE_TO_MAP_BROWSER_THUMBNAILS);

    miMapFilterScroll = new MI_MapFilterScroll(&rm->menu_plain_field, 120, 72, 400, 9);
    miMapFilterScroll->SetAutoModify(true);
    miMapFilterScroll->setVisible(false);

    // Add auto map filters
    for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++) {
        miMapFilterScroll->Add(g_szAutoFilterNames[iFilter], g_iAutoFilterIcons[iFilter]);
    }

    // Add user defined filters
    for (size_t iFilter = 0; iFilter < filterslist->count(); iFilter++) {
        std::string szTemp = GetNameFromFileName(filterslist->at(iFilter), true);
        miMapFilterScroll->Add(std::move(szTemp), game_values.piFilterIcons[NUM_AUTO_FILTERS + iFilter]);
    }


    miGameSettingsLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miGameSettingsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miGameSettingsMenuHeaderText = new MI_HeaderText("Single Game Menu", 320, 5);


    // Exit tournament dialog box
    miGameSettingsExitDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miGameSettingsExitDialogExitText = new MI_HeaderText("Exit", 320, 195);
    miGameSettingsExitDialogTournamentText = new MI_HeaderText("Tournament", 320, 220);
    miGameSettingsExitDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, TextAlign::CENTER);
    miGameSettingsExitDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, TextAlign::CENTER);

    miGameSettingsExitDialogYesButton->SetCode(MENU_CODE_EXIT_TOURNAMENT_YES);
    miGameSettingsExitDialogNoButton->SetCode(MENU_CODE_EXIT_TOURNAMENT_NO);

    miGameSettingsExitDialogImage->setVisible(false);
    miGameSettingsExitDialogTournamentText->setVisible(false);
    miGameSettingsExitDialogExitText->setVisible(false);
    miGameSettingsExitDialogYesButton->setVisible(false);
    miGameSettingsExitDialogNoButton->setVisible(false);

    AddControl(miSettingsStartButton, miMapThumbnailsButton, miModeField, NULL, NULL);
    AddControl(miModeField, miSettingsStartButton, miGoalField[0], NULL, NULL);

    AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);

    for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
        AddControl(miGoalField[iGoalField], miGoalField[iGoalField - 1], miGoalField[iGoalField + 1], miGoalField[iGoalField - 1], miModeSettingsButton);

    AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miMapField, miGoalField[GAMEMODE_LAST - 2], miModeSettingsButton);

    AddControl(miModeSettingsButton, miModeField, miMapField, miGoalField[GAMEMODE_LAST - 1], NULL);
    AddControl(miMapField, miGoalField[GAMEMODE_LAST - 1], miMapFiltersButton, NULL, NULL);
    AddControl(miMapFiltersButton, miMapField, miMapThumbnailsButton, NULL, NULL);
    AddControl(miMapThumbnailsButton, miMapFiltersButton, miSettingsStartButton, NULL, NULL);

    AddControl(miMapFilterScroll, NULL, NULL, NULL, NULL);

    AddNonControl(miGameSettingsLeftHeaderBar);
    AddNonControl(miGameSettingsMenuRightHeaderBar);
    AddNonControl(miGameSettingsMenuHeaderText);

    AddNonControl(miGameSettingsExitDialogImage);
    AddNonControl(miGameSettingsExitDialogExitText);
    AddNonControl(miGameSettingsExitDialogTournamentText);

    AddNonControl(miMapFiltersOnImage);

    AddControl(miGameSettingsExitDialogYesButton, NULL, NULL, NULL, miGameSettingsExitDialogNoButton);
    AddControl(miGameSettingsExitDialogNoButton, NULL, NULL, miGameSettingsExitDialogYesButton, NULL);

    setInitialFocus(miSettingsStartButton);

    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);
}

void UI_GameSettingsMenu::RefreshGameModeButtons()
{
    // Unhide/hide the settings button
    miModeSettingsButton->setVisible(miModeField->currentValue() != game_mode_owned);

    // Show the approprate goal field
    for (short iMode = 0; iMode < GAMEMODE_LAST; iMode++) {
        miGoalField[iMode]->setVisible(miModeField->currentValue() == iMode);
    }
}

void UI_GameSettingsMenu::OpenMapFilters()
{
    miMapFilterScroll->setVisible(true);
    RememberCurrent();

    setInitialFocus(miMapFilterScroll);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_GameSettingsMenu::CloseMapFilters()
{
    miMapFilterScroll->setVisible(false);

    setInitialFocus(miSettingsStartButton);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();

    miMapFiltersOnImage->setVisible(game_values.fFiltersOn);
}

void UI_GameSettingsMenu::OpenExitDialog()
{
    miGameSettingsExitDialogImage->setVisible(true);
    miGameSettingsExitDialogTournamentText->setVisible(true);
    miGameSettingsExitDialogExitText->setVisible(true);
    miGameSettingsExitDialogYesButton->setVisible(true);
    miGameSettingsExitDialogNoButton->setVisible(true);

    RememberCurrent();

    setInitialFocus(miGameSettingsExitDialogNoButton);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_GameSettingsMenu::CloseExitDialog()
{
    miGameSettingsExitDialogImage->setVisible(false);
    miGameSettingsExitDialogTournamentText->setVisible(false);
    miGameSettingsExitDialogExitText->setVisible(false);
    miGameSettingsExitDialogYesButton->setVisible(false);
    miGameSettingsExitDialogNoButton->setVisible(false);

    setInitialFocus(miSettingsStartButton);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();
}

void UI_GameSettingsMenu::SetHeaderText(const char* string)
{
    miGameSettingsMenuHeaderText->SetText(string);
}

void UI_GameSettingsMenu::GameModeChanged(short gmID)
{
    miModeField->setCurrentValue(gmID);

    RefreshGameModeButtons();
}

void UI_GameSettingsMenu::HideGMSettingsBtn()
{
    miModeSettingsButton->setVisible(false);
}

short UI_GameSettingsMenu::GetCurrentGameModeID()
{
    return miModeField->currentValue();
}

const char* UI_GameSettingsMenu::GetCurrentMapName()
{
    return miMapField->GetMapName();
}

bool UI_GameSettingsMenu::IsOnStartBtn()
{
    return initialFocus() == miSettingsStartButton;
}
