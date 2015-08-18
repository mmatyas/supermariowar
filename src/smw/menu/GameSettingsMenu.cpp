#include "GameSettingsMenu.h"

#include "FileList.h"
#include "GameMode.h"
#include "GameValues.h"
#include "path.h"
#include "ResourceManager.h"

extern CGameMode * gamemodes[GAMEMODE_LAST];
extern short currentgamemode;

extern CGameValues game_values;
extern CResourceManager* rm;
extern FiltersList *filterslist;

extern const char * g_szAutoFilterNames[NUM_AUTO_FILTERS];
extern short g_iAutoFilterIcons[NUM_AUTO_FILTERS];

UI_GameSettingsMenu::UI_GameSettingsMenu() : UI_Menu()
{
    miSettingsStartButton = new MI_Button(&rm->spr_selectfield, 70, 45, "Start", 500, 0);
    miSettingsStartButton->SetCode(MENU_CODE_START_GAME);

    miModeField = new MI_ImageSelectField(&rm->spr_selectfield, &rm->menu_mode_small, 70, 85, "Mode", 500, 120, 16, 16);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
        miModeField->Add(gamemodes[iGameMode]->GetModeName(), iGameMode, "", false, false);
    }
    miModeField->SetData(&currentgamemode, NULL, NULL);
    miModeField->SetKey(0);
    miModeField->SetItemChangedCode(MENU_CODE_MODE_CHANGED);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
        miGoalField[iGameMode] = new MI_SelectField(&rm->spr_selectfield, 70, 125, gamemodes[iGameMode]->GetGoalName(), 352, 120);
        //miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
        miGoalField[iGameMode]->Show(iGameMode == 0);

        for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS; iGameModeOption++) {
            SModeOption * option = &gamemodes[iGameMode]->GetOptions()[iGameModeOption];
            miGoalField[iGameMode]->Add(option->szName, option->iValue, "", false, false);
        }

        miGoalField[iGameMode]->SetData(&gamemodes[iGameMode]->goal, NULL, NULL);
        miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
    }

    miModeSettingsButton = new MI_Button(&rm->spr_selectfield, 430, 125, "Settings", 140, 0);
    miModeSettingsButton->SetCode(MENU_CODE_TO_MODE_SETTINGS_MENU);

    miMapField = new MI_MapField(&rm->spr_selectfield, 70, 165, "Map", 400, 120, true);

    miMapFiltersButton = new MI_Button(&rm->spr_selectfield, 430, 205, "Filters", 140, 0);
    miMapFiltersButton->SetCode(MENU_CODE_TO_MAP_FILTERS);

    miMapFiltersOnImage = new MI_Image(&rm->menu_map_filter, 530, 213, 0, 48, 16, 16, 1, 1, 0);
    miMapFiltersOnImage->Show(false);

    miMapThumbnailsButton = new MI_Button(&rm->spr_selectfield, 430, 245, "Thumbs", 140, 0);
    miMapThumbnailsButton->SetCode(MENU_CODE_TO_MAP_BROWSER_THUMBNAILS);

    miMapFilterScroll = new MI_MapFilterScroll(&rm->menu_plain_field, 120, 72, 400, 9);
    miMapFilterScroll->SetAutoModify(true);
    miMapFilterScroll->Show(false);

    //Add auto map filters
    for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++) {
        miMapFilterScroll->Add(g_szAutoFilterNames[iFilter], g_iAutoFilterIcons[iFilter]);
    }

    //Add user defined filters
    char szTemp[256];
    for (short iFilter = 0; iFilter < filterslist->GetCount(); iFilter++) {
        GetNameFromFileName(szTemp, filterslist->GetIndex(iFilter), true);
        miMapFilterScroll->Add(szTemp, game_values.piFilterIcons[NUM_AUTO_FILTERS + iFilter]);
    }


    miGameSettingsLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miGameSettingsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miGameSettingsMenuHeaderText = new MI_Text("Single Game Menu", 320, 5, 0, 2, 1);


    //Exit tournament dialog box
    miGameSettingsExitDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miGameSettingsExitDialogExitText = new MI_Text("Exit", 320, 195, 0, 2, 1);
    miGameSettingsExitDialogTournamentText = new MI_Text("Tournament", 320, 220, 0, 2, 1);
    miGameSettingsExitDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, 1);
    miGameSettingsExitDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, 1);

    miGameSettingsExitDialogYesButton->SetCode(MENU_CODE_EXIT_TOURNAMENT_YES);
    miGameSettingsExitDialogNoButton->SetCode(MENU_CODE_EXIT_TOURNAMENT_NO);

    miGameSettingsExitDialogImage->Show(false);
    miGameSettingsExitDialogTournamentText->Show(false);
    miGameSettingsExitDialogExitText->Show(false);
    miGameSettingsExitDialogYesButton->Show(false);
    miGameSettingsExitDialogNoButton->Show(false);

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

    SetHeadControl(miSettingsStartButton);

    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);
};

UI_GameSettingsMenu::~UI_GameSettingsMenu() {
}

void UI_GameSettingsMenu::RefreshGameModeButtons()
{
    // Unhide/hide the settings button
    miModeSettingsButton->Show(miModeField->GetShortValue() != game_mode_owned);

    //Show the approprate goal field
    for (short iMode = 0; iMode < GAMEMODE_LAST; iMode++) {
        miGoalField[iMode]->Show(miModeField->GetShortValue() == iMode);
    }
}

void UI_GameSettingsMenu::OpenMapFilters()
{
    miMapFilterScroll->Show(true);
    RememberCurrent();

    SetHeadControl(miMapFilterScroll);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_GameSettingsMenu::CloseMapFilters()
{
    miMapFilterScroll->Show(false);

    SetHeadControl(miSettingsStartButton);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();

    miMapFiltersOnImage->Show(game_values.fFiltersOn);
}

void UI_GameSettingsMenu::OpenExitDialog()
{
    miGameSettingsExitDialogImage->Show(true);
    miGameSettingsExitDialogTournamentText->Show(true);
    miGameSettingsExitDialogExitText->Show(true);
    miGameSettingsExitDialogYesButton->Show(true);
    miGameSettingsExitDialogNoButton->Show(true);

    RememberCurrent();

    SetHeadControl(miGameSettingsExitDialogNoButton);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_GameSettingsMenu::CloseExitDialog()
{
    miGameSettingsExitDialogImage->Show(false);
    miGameSettingsExitDialogTournamentText->Show(false);
    miGameSettingsExitDialogExitText->Show(false);
    miGameSettingsExitDialogYesButton->Show(false);
    miGameSettingsExitDialogNoButton->Show(false);

    SetHeadControl(miSettingsStartButton);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();
}

void UI_GameSettingsMenu::SetHeaderText(const char* string)
{
    miGameSettingsMenuHeaderText->SetText(string);
}

void UI_GameSettingsMenu::GameModeChanged(short gmID)
{
    miModeField->SetKey(gmID);

    RefreshGameModeButtons();
}

void UI_GameSettingsMenu::HideGMSettingsBtn()
{
    miModeSettingsButton->Show(false);
}

short UI_GameSettingsMenu::GetCurrentGameModeID()
{
    return miModeField->GetShortValue();
}

const char* UI_GameSettingsMenu::GetCurrentMapName()
{
    return miMapField->GetMapName();
}

bool UI_GameSettingsMenu::IsOnStartBtn()
{
    return (GetHeadControl() == miSettingsStartButton);
}
