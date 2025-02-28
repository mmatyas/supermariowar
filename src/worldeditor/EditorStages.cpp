#include "EditorStages.h"

#include "GameValues.h"
#include "Helpers.h"
#include "MapList.h"
#include "ResourceManager.h"
#include "world.h"
#include "WorldTourStop.h"
#include "menu/ModeOptionsMenu.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_ImageSelectField.h"
#include "ui/MI_MapField.h"
#include "ui/MI_Text.h"
#include "ui/MI_TextField.h"

extern CGameValues game_values;
extern MapList *maplist;
extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern void SetDisplayMessage(short iTime,
    const char* szTitle,
    const char* szLine1,
    const char* szLine2,
    const char* szLine3);


namespace {
// Sets up default mode options
struct StageModeOption {
    char szName[64];
    short iValue;
};

struct StageMode {
    char szName[128];
    char szGoal[64];

    short iDefaultGoal;

    StageModeOption options[GAMEMODE_NUM_OPTIONS - 1];
};


constexpr int g_iNumGameModeSettings[GAMEMODE_LAST] = { 2, 2, 3, 4, 3, 10, 9, 6, 2, 1, 3, 5, 3, 3, 0, 22, 6, 4, 3, 4, 4, 3 };
StageMode stagemodes[GAMEMODE_LAST];
short iLastStageType = 0;


void SetStageMode(short iIndex, const char* szModeName, const char* szGoalName, short iIncrement, short iDefault)
{
    if (iIndex < 0 || iIndex >= GAMEMODE_LAST)
        return;

    StageMode* sm = &stagemodes[iIndex];

    strncpy(sm->szName, szModeName, 128);
    sm->szName[127] = 0;

    strncpy(sm->szGoal, szGoalName, 64);
    sm->szGoal[63] = 0;

    sm->iDefaultGoal = iDefault;

    for (short iModeOption = 0; iModeOption < GAMEMODE_NUM_OPTIONS - 1; iModeOption++) {
        sm->options[iModeOption].iValue = (iModeOption + 1) * iIncrement;
        sprintf(sm->options[iModeOption].szName, "%d", sm->options[iModeOption].iValue);
    }
}


void SetBonusString(char* szString, short iPlace, short iItem, short iStageType)
{
    char cType = 'p';

    if (iItem >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {
        cType = 's';
        iItem -= NUM_POWERUPS + NUM_WORLD_POWERUPS;
    } else if (iItem >= NUM_POWERUPS) {
        cType = 'w';
        iItem -= NUM_POWERUPS;
    }

    if (iStageType == 0) {
        sprintf(szString, "%d%c%d", iPlace, cType, iItem);
    } else {
        sprintf(szString, "%c%d", cType, iItem);
    }
}

void AdjustBonuses(TourStop* ts)
{
    // No need to do anything if we were a stage and we're still a stage
    //(or a house and still a house)
    if (ts->iStageType == iLastStageType)
        return;

    if (ts->iStageType == 0) {
        // Remove any score bonuses
        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            if (ts->wsbBonuses[iBonus].iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {
                ts->iNumBonuses--;
                for (short iRemoveBonus = iBonus; iRemoveBonus < ts->iNumBonuses; iRemoveBonus++) {
                    ts->wsbBonuses[iRemoveBonus].iBonus = ts->wsbBonuses[iRemoveBonus + 1].iBonus;
                    ts->wsbBonuses[iRemoveBonus].iWinnerPlace = ts->wsbBonuses[iRemoveBonus + 1].iWinnerPlace;
                    strcpy(ts->wsbBonuses[iRemoveBonus].szBonusString, ts->wsbBonuses[iRemoveBonus + 1].szBonusString);
                }
            }
        }

               // Add places to bonuses
        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            if (ts->wsbBonuses[iBonus].iWinnerPlace < 0 || ts->wsbBonuses[iBonus].iWinnerPlace > 3)
                ts->wsbBonuses[iBonus].iWinnerPlace = 0;

            SetBonusString(ts->wsbBonuses[iBonus].szBonusString, ts->wsbBonuses[iBonus].iWinnerPlace + 1, ts->wsbBonuses[iBonus].iBonus, ts->iStageType);
        }
    } else if (ts->iStageType == 1) {
        // Cap the number of
        if (ts->iNumBonuses > MAX_BONUS_CHESTS)
            ts->iNumBonuses = MAX_BONUS_CHESTS;

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            SetBonusString(ts->wsbBonuses[iBonus].szBonusString, 0, ts->wsbBonuses[iBonus].iBonus, ts->iStageType);
        }
    }

    iLastStageType = ts->iStageType;
}

void SaveStage(short iEditStage)
{
    // Set the number of game mode settings to the maximum so we write them all out
    game_values.tourstops[iEditStage]->fUseSettings = true;
    game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[game_values.tourstops[iEditStage]->iMode];

           // Copy the working values back into the structure that will be saved
    memcpy(&game_values.tourstops[iEditStage]->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

    if (game_values.tourstops[iEditStage]->iMode >= 25 && game_values.tourstops[iEditStage]->iMode <= 27)
        game_values.tourstops[iEditStage]->iMode += 975;
}
}  // namespace


EditorStages::EditorStages()
    : EditorBase()
{
    SetStageMode(0, "Classic", "Lives", 5, 10);
    SetStageMode(1, "Frag", "Kills", 5, 20);
    SetStageMode(2, "Time Limit", "Time", 30, 60);
    SetStageMode(3, "Jail", "Kills", 5, 20);
    SetStageMode(4, "Coin Collection", "Coins", 5, 20);
    SetStageMode(5, "Stomp", "Kills", 10, 10);
    SetStageMode(6, "Yoshi's Eggs", "Eggs", 5, 20);
    SetStageMode(7, "Capture The Flag", "Points", 5, 20);
    SetStageMode(8, "Chicken", "Points", 50, 200);
    SetStageMode(9, "Tag", "Points", 50, 200);
    SetStageMode(10, "Star", "Lives", 1, 5);
    SetStageMode(11, "Domination", "Points", 50, 200);
    SetStageMode(12, "King of the Hill", "Points", 50, 200);
    SetStageMode(13, "Race", "Laps", 2, 10);
    SetStageMode(14, "Owned", "Points", 50, 200);
    SetStageMode(15, "Frenzy", "Kills", 5, 20);
    SetStageMode(16, "Survival", "Lives", 5, 10);
    SetStageMode(17, "Greed", "Coins", 10, 40);
    SetStageMode(18, "Health", "Lives", 1, 5);
    SetStageMode(19, "Card Collection", "Points", 10, 30);
    SetStageMode(20, "Phanto Chase", "Points", 50, 200);
    SetStageMode(21, "Shy Guy Tag", "Points", 50, 200);

    short iColCount = 0;
    short iRowCount = 0;
    for (short iItem = 0; iItem < NUM_WORLD_ITEMS; iItem++) {
        rItemDst[iItem].x = 16 + iColCount * 48;
        rItemDst[iItem].y = 16 + iRowCount * 48;
        rItemDst[iItem].w = 32;
        rItemDst[iItem].h = 32;

        if (++iColCount > 12) {
            iColCount = 0;
            iRowCount++;
        }
    }

    for (short iStageBonus = 0; iStageBonus < 10; iStageBonus++) {
        rStageBonusDst[iStageBonus].x = 35 + iStageBonus * 58;
        rStageBonusDst[iStageBonus].y = 360;
        rStageBonusDst[iStageBonus].w = 32;
        rStageBonusDst[iStageBonus].h = 32;
    }

    short iStartItemX = (640 - (MAX_BONUS_CHESTS * 58 - 10)) >> 1;
    for (short iHouseBonus = 0; iHouseBonus < MAX_BONUS_CHESTS; iHouseBonus++) {
        rHouseBonusDst[iHouseBonus].x = iStartItemX + iHouseBonus * 58;
        rHouseBonusDst[iHouseBonus].y = 360;
        rHouseBonusDst[iHouseBonus].w = 32;
        rHouseBonusDst[iHouseBonus].h = 32;
    }
}


void EditorStages::onEnter(const WorldMap& world)
{
    EditorBase::onEnter(world);
    mCurrentMenu = &mStageSettingsMenu;
    mCurrentMenu->ResetMenu();

    iStageDisplay = -1;
    iEditStage = -1;
}


void EditorStages::NewStage(WorldMap& world, short* iEditStage)
{
    TourStop* ts = new TourStop();

    ts->iStageType = 0;

    ts->szBonusText[0][0] = 0;
    ts->szBonusText[1][0] = 0;
    ts->szBonusText[2][0] = 0;
    ts->szBonusText[3][0] = 0;
    ts->szBonusText[4][0] = 0;

    ts->pszMapFile = maplist->currentShortmapname();
    ts->iMode = 0;

    ts->fUseSettings = true;
    ts->iNumUsedSettings = g_iNumGameModeSettings[0];

    ts->iGoal = 10;
    ts->iPoints = 1;

    ts->iBonusType = 0;
    ts->iNumBonuses = 0;

    sprintf(ts->szName, "Tour Stop %d", game_values.tourstops.size() + 1);

    ts->fEndStage = false;

           // Copy in default values first
    memcpy(&ts->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

    game_values.tourstops.push_back(ts);
    world.setStageCount(world.stageCount() + 1);

    *iEditStage = game_values.tourstops.size() - 1;
    EditStage(*iEditStage);
}


void EditorStages::EditStage(short iEditStage)
{
    memcpy(&game_values.gamemodemenusettings, &game_values.tourstops[iEditStage]->gmsSettings, sizeof(GameModeSettings));

           // Set fields to write data to the selected stage
    TourStop* ts = game_values.tourstops[iEditStage];

    miModeField->setOutputPtr(&ts->iMode);
    miNameField->SetData(ts->szName, 128);

    miPointsField->setOutputPtr(&ts->iPoints);
    miFinalStageField->setOutputPtr(&ts->fEndStage);

    if (ts->pszMapFile.empty())
        ts->pszMapFile = maplist->currentShortmapname();

    miMapField->SetMap(ts->pszMapFile.c_str(), false);

    miBonusType->setOutputPtr(&ts->iBonusType);
    miBonusTextField[0]->SetData(ts->szBonusText[0], 128);
    miBonusTextField[1]->SetData(ts->szBonusText[1], 128);
    miBonusTextField[2]->SetData(ts->szBonusText[2], 128);
    miBonusTextField[3]->SetData(ts->szBonusText[3], 128);
    miBonusTextField[4]->SetData(ts->szBonusText[4], 128);

    ts->iBonusTextLines = 5;

    short iMode = game_values.tourstops[iEditStage]->iMode;
    short iStageType = game_values.tourstops[iEditStage]->iStageType;

    if (iMode >= 25 && iMode <= 27)
        iMode += 975;

           // Show fields applicable for this mode
    miPointsField->setVisible(iStageType == 0);
    miFinalStageField->setVisible(iStageType == 0);
    miMapField->setVisible(iStageType == 0);

    miBonusType->setVisible(iStageType == 1);
    miBonusTextField[0]->setVisible(iStageType == 1);
    miBonusTextField[1]->setVisible(iStageType == 1);
    miBonusTextField[2]->setVisible(iStageType == 1);
    miBonusTextField[3]->setVisible(iStageType == 1);
    miBonusTextField[4]->setVisible(iStageType == 1);

    miSpecialGoalField[0]->setVisible(iMode == 1000);
    miSpecialGoalField[1]->setVisible(iMode == 1001);
    miSpecialGoalField[2]->setVisible(iMode == 1002);

    miBonusItemsButton->SetPosition(430, iStageType == 0 ? 220 : 340);

    if (iStageType == 0 && iMode >= 0 && iMode < GAMEMODE_LAST) {
        miModeField->setCurrentValue(iMode);

        miModeSettingsButton->setVisible(iMode != game_mode_owned);

        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
            miGoalField[iGameMode]->setVisible(iMode == iGameMode);
        }

        miGoalField[iMode]->setOutputPtr(&game_values.tourstops[iEditStage]->iGoal);
        miGoalField[iMode]->setCurrentValue(game_values.tourstops[iEditStage]->iGoal);
        miPointsField->setCurrentValue(game_values.tourstops[iEditStage]->iPoints);
        miFinalStageField->setCurrentValue(game_values.tourstops[iEditStage]->fEndStage ? 1 : 0);

        game_values.tourstops[iEditStage]->fUseSettings = true;
        game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[iMode];
    } else {
        // Show the settings button for boss mode
        miModeSettingsButton->setVisible(iMode == 1001);

        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
            miGoalField[iGameMode]->setVisible(false);
        }

        if (iStageType == 1) {  // Bonus House
            miModeField->setCurrentValue(24);
        } else if (iMode >= 1000 && iMode <= 1002) {  // Pipe, Boss and Boxes Game
            miModeField->setCurrentValue(iMode - 975);
            miSpecialGoalField[iMode - 1000]->setOutputPtr(&game_values.tourstops[iEditStage]->iGoal);
            miSpecialGoalField[iMode - 1000]->setCurrentValue(game_values.tourstops[iEditStage]->iGoal);
            miPointsField->setCurrentValue(game_values.tourstops[iEditStage]->iPoints);
            miFinalStageField->setCurrentValue(game_values.tourstops[iEditStage]->fEndStage ? 1 : 0);

                   // Have to set this back again since the setCurrentValue() above will set it to 25
            game_values.tourstops[iEditStage]->iMode = iMode;
        }
    }
}


void EditorStages::EnableStageMenu(bool fEnable)
{
    miNameField->Disable(!fEnable);
    miModeField->Disable(!fEnable);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
        miGoalField[iGameMode]->Disable(!fEnable);

    miSpecialGoalField[0]->Disable(!fEnable);
    miSpecialGoalField[1]->Disable(!fEnable);
    miSpecialGoalField[2]->Disable(!fEnable);

    miModeSettingsButton->Disable(!fEnable);

    miPointsField->Disable(!fEnable);
    miFinalStageField->Disable(!fEnable);
    miMapField->Disable(!fEnable);
    miBonusItemsButton->Disable(!fEnable);
    miBonusType->Disable(!fEnable);
    miBonusTextField[0]->Disable(!fEnable);
    miBonusTextField[1]->Disable(!fEnable);
    miBonusTextField[2]->Disable(!fEnable);
    miBonusTextField[3]->Disable(!fEnable);
    miBonusTextField[4]->Disable(!fEnable);
    miDeleteStageButton->Disable(!fEnable);
}


void EditorStages::TestAndSetBonusItem(TourStop& ts, short iPlace)
{
    short iMaxBonusesAllowed = 10;

    if (ts.iStageType == 1)
        iMaxBonusesAllowed = MAX_BONUS_CHESTS;

    if (ts.iNumBonuses < iMaxBonusesAllowed) {
        short iNumSelectableItems = NUM_WORLD_ITEMS;
        if (ts.iStageType == 0)
            iNumSelectableItems = NUM_POWERUPS + NUM_WORLD_POWERUPS;

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        for (short iItem = 0; iItem < iNumSelectableItems; iItem++) {
            if (mouseX >= rItemDst[iItem].x && mouseX < rItemDst[iItem].w + rItemDst[iItem].x && mouseY >= rItemDst[iItem].y && mouseY < rItemDst[iItem].h + rItemDst[iItem].y) {
                // If this is a normal stage, then alert the player that they need to select the place for this item
                if (ts.iStageType == 0 && iPlace == 0) {
                    SetDisplayMessage(120, "Use Number Keys", "Hover over item", "use keys 1 to 4", "to select bonus");
                    return;
                }

                       // Set the bonus item for the place selected
                ts.wsbBonuses[ts.iNumBonuses].iBonus = iItem;
                ts.wsbBonuses[ts.iNumBonuses].iWinnerPlace = iPlace - 1;

                SetBonusString(ts.wsbBonuses[ts.iNumBonuses].szBonusString, iPlace, iItem, ts.iStageType);

                ts.iNumBonuses++;

                break;
            }
        }
    }
}


void EditorStages::setupMenu(CResourceManager& rm)
{
    // Setup The Mode Menu
    mCurrentMenu = &mStageSettingsMenu;

    // Name
    miNameField = new MI_TextField(&rm.spr_selectfield, 70, 20, "Name", 500, 120);
    miNameField->SetDisallowedChars(",");

    miModeField = new MI_ImageSelectField(&rm.spr_selectfield, &rm.menu_mode_small, 70, 60, "Mode", 500, 120, 16, 16);
    // miModeField->SetData(game_values.tourstops[0]->iMode, NULL, NULL);
    // miModeField->SetKey(0);
    miModeField->setItemChangedCode(MENU_CODE_MODE_CHANGED);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
        miModeField->add(stagemodes[iGameMode].szName, iGameMode);

        miGoalField[iGameMode] = new MI_SelectField<short>(&rm.spr_selectfield, 70, 100, stagemodes[iGameMode].szGoal, 352, 120);
        miGoalField[iGameMode]->setVisible(iGameMode == 0);

        for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS - 1; iGameModeOption++) {
            StageModeOption* option = &stagemodes[iGameMode].options[iGameModeOption];
            miGoalField[iGameMode]->add(option->szName, option->iValue);
        }

        // miGoalField[iGameMode]->SetData(&gamemodes[iGameMode]->goal, NULL, NULL);
        // miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
    }

    miModeField->add("Bonus House", 24);
    miModeField->add("Pipe Minigame", 25);
    miModeField->add("Boss Minigame", 26);
    miModeField->add("Boxes Minigame", 27);

    // Create goal field for pipe game
    miSpecialGoalField[0] = new MI_SelectField<short>(&rm.spr_selectfield, 70, 100, "Points", 352, 120);
    miSpecialGoalField[0]->setVisible(false);

    for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS - 1; iGameModeOption++) {
        short iValue = 10 + iGameModeOption * 10;
        char szName[16];
        sprintf(szName, "%d", iValue);
        miSpecialGoalField[0]->add(szName, iValue);
    }

    // Create goal field for boss game
    miSpecialGoalField[1] = new MI_SelectField<short>(&rm.spr_selectfield, 70, 100, "Lives", 352, 120);
    miSpecialGoalField[1]->setVisible(false);

    for (short iGameLives = 1; iGameLives <= 30; iGameLives++) {
        char szName[16];
        sprintf(szName, "%d", iGameLives);
        miSpecialGoalField[1]->add(szName, iGameLives);
    }

    // Create goal field for boxes game
    miSpecialGoalField[2] = new MI_SelectField<short>(&rm.spr_selectfield, 70, 100, "Lives", 352, 120);
    miSpecialGoalField[2]->setVisible(false);

    for (short iGameLives = 1; iGameLives <= 30; iGameLives++) {
        char szName[16];
        sprintf(szName, "%d", iGameLives);
        miSpecialGoalField[2]->add(szName, iGameLives);
    }

    // Mode Settings Button
    miModeSettingsButton = new MI_Button(&rm.spr_selectfield, 430, 100, "Settings", 140);
    miModeSettingsButton->SetCode(MENU_CODE_TO_MODE_SETTINGS_MENU);

    // Points Field
    miPointsField = new MI_SelectField<short>(&rm.spr_selectfield, 70, 140, "Points", 245, 120);
    for (short iPoints = 0; iPoints <= 20; iPoints++) {
        char szPoints[8];
        sprintf(szPoints, "%d", iPoints);
        miPointsField->add(szPoints, iPoints);
    }

    // Final Stage Field
    miFinalStageField = new MI_SelectField<bool>(&rm.spr_selectfield, 325, 140, "End Stage", 245, 120);
    miFinalStageField->add("No", false);
    miFinalStageField->add("Yes", true);
    miFinalStageField->setAutoAdvance(true);

    // Map Select Field
    miMapField = new MI_MapField(&rm.spr_selectfield, 70, 180, "Map", 500, 120, true);

    // Bonus Item Picker Menu Button
    miBonusItemsButton = new MI_Button(&rm.spr_selectfield, 430, 220, "Bonuses", 140);
    miBonusItemsButton->SetCode(MENU_CODE_TO_BONUS_PICKER_MENU);

    // Bonus Type
    miBonusType = new MI_SelectField<short>(&rm.spr_selectfield, 70, 100, "Type", 500, 120);
    miBonusType->add("Fixed", false);
    miBonusType->add("Random", true);
    miBonusType->setAutoAdvance(true);

    // Bonus House Text * 5
    miBonusTextField[0] = new MI_TextField(&rm.spr_selectfield, 70, 140, "Text", 500, 120);
    miBonusTextField[1] = new MI_TextField(&rm.spr_selectfield, 70, 180, "Text", 500, 120);
    miBonusTextField[2] = new MI_TextField(&rm.spr_selectfield, 70, 220, "Text", 500, 120);
    miBonusTextField[3] = new MI_TextField(&rm.spr_selectfield, 70, 260, "Text", 500, 120);
    miBonusTextField[4] = new MI_TextField(&rm.spr_selectfield, 70, 300, "Text", 500, 120);

    miBonusTextField[0]->SetDisallowedChars(",|");
    miBonusTextField[1]->SetDisallowedChars(",|");
    miBonusTextField[2]->SetDisallowedChars(",|");
    miBonusTextField[3]->SetDisallowedChars(",|");
    miBonusTextField[4]->SetDisallowedChars(",|");

    // Delete Stage Button
    miDeleteStageButton = new MI_Button(&rm.spr_selectfield, 430, 440, "Delete", 140);
    miDeleteStageButton->SetCode(MENU_CODE_DELETE_STAGE_BUTTON);

    // Are You Sure Dialog for Delete Stage
    miDeleteStageDialogImage = new MI_Image(&rm.spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miDeleteStageDialogAreYouText = new MI_HeaderText("Are You", 320, 195);
    miDeleteStageDialogSureText = new MI_HeaderText("Sure?", 320, 220);
    miDeleteStageDialogYesButton = new MI_Button(&rm.spr_selectfield, 235, 250, "Yes", 80, TextAlign::CENTER);
    miDeleteStageDialogNoButton = new MI_Button(&rm.spr_selectfield, 325, 250, "No", 80, TextAlign::CENTER);

    miDeleteStageDialogYesButton->SetCode(MENU_CODE_DELETE_STAGE_YES);
    miDeleteStageDialogNoButton->SetCode(MENU_CODE_DELETE_STAGE_NO);

    miDeleteStageDialogImage->setVisible(false);
    miDeleteStageDialogAreYouText->setVisible(false);
    miDeleteStageDialogSureText->setVisible(false);
    miDeleteStageDialogYesButton->setVisible(false);
    miDeleteStageDialogNoButton->setVisible(false);

    // Add Name Field
    mStageSettingsMenu.AddControl(miNameField, miDeleteStageButton, miModeField, NULL, NULL);

    // Add Mode Field
    mStageSettingsMenu.AddControl(miModeField, miNameField, miGoalField[0], NULL, NULL);

    // Add Mode Goal Fields
    mStageSettingsMenu.AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);

    for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
        mStageSettingsMenu.AddControl(miGoalField[iGoalField], miGoalField[iGoalField - 1], miGoalField[iGoalField + 1], miGoalField[iGoalField - 1], miModeSettingsButton);

    mStageSettingsMenu.AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miSpecialGoalField[0], miGoalField[GAMEMODE_LAST - 2], miModeSettingsButton);

    mStageSettingsMenu.AddControl(miSpecialGoalField[0], miGoalField[GAMEMODE_LAST - 1], miSpecialGoalField[1], miGoalField[GAMEMODE_LAST - 1], miSpecialGoalField[1]);
    mStageSettingsMenu.AddControl(miSpecialGoalField[1], miSpecialGoalField[0], miSpecialGoalField[2], miSpecialGoalField[0], miSpecialGoalField[2]);
    mStageSettingsMenu.AddControl(miSpecialGoalField[2], miSpecialGoalField[1], miPointsField, miSpecialGoalField[1], miModeSettingsButton);

    // Add Mode Settings Button
    mStageSettingsMenu.AddControl(miModeSettingsButton, miModeField, miFinalStageField, miSpecialGoalField[2], NULL);

    // Add Points Field
    mStageSettingsMenu.AddControl(miPointsField, miSpecialGoalField[2], miMapField, NULL, miFinalStageField);

    // Add Final Stage Field
    mStageSettingsMenu.AddControl(miFinalStageField, miSpecialGoalField[2], miMapField, miPointsField, NULL);

    // Add Map Field
    mStageSettingsMenu.AddControl(miMapField, miFinalStageField, miBonusType, NULL, miBonusItemsButton);

    // Add Bonus House Fields
    mStageSettingsMenu.AddControl(miBonusType, miMapField, miBonusTextField[0], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[0], miBonusType, miBonusTextField[1], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[1], miBonusTextField[0], miBonusTextField[2], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[2], miBonusTextField[1], miBonusTextField[3], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[3], miBonusTextField[2], miBonusTextField[4], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[4], miBonusTextField[3], miBonusItemsButton, NULL, NULL);

    // Add Bonus Button
    mStageSettingsMenu.AddControl(miBonusItemsButton, miBonusTextField[4], miDeleteStageButton, miMapField, NULL);

    // Add Delete Stage Button
    mStageSettingsMenu.AddControl(miDeleteStageButton, miBonusItemsButton, miNameField, miMapField, NULL);

    // Add Are You Sure Dialog
    mStageSettingsMenu.AddNonControl(miDeleteStageDialogImage);
    mStageSettingsMenu.AddNonControl(miDeleteStageDialogAreYouText);
    mStageSettingsMenu.AddNonControl(miDeleteStageDialogSureText);

    mStageSettingsMenu.AddControl(miDeleteStageDialogYesButton, NULL, NULL, NULL, miDeleteStageDialogNoButton);
    mStageSettingsMenu.AddControl(miDeleteStageDialogNoButton, NULL, NULL, miDeleteStageDialogYesButton, NULL);

    mStageSettingsMenu.setInitialFocus(miNameField);
    mStageSettingsMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);

    mBonusItemPicker.SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    mModeOptionsMenu = new UI_ModeOptionsMenu();
}


void EditorStages::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    if (event.state != SDL_PRESSED)
        return;

    // Do not allow saving world by pressing 's' key
    // World data structures are not in the correct state to be saved
    // until exiting the stage editor menu in (MENU_CODE_EXIT_APPLICATION == code) below

    if (iEditStage == -1 && event.keysym.sym == SDLK_n) {
        NewStage(world, &iEditStage);
    } else if ((event.keysym.sym == SDLK_ESCAPE || event.keysym.sym == SDLK_e) && iEditStage == -1) {
        if (world.stageCount() > 0 && (m_selectedTileId < 6 || m_selectedTileId >= world.stageCount() + 6)) {
            m_selectedTileId = 6;
        }

        newlyEntered = false;
        return;
    } else if (mCurrentMenu == &mBonusItemPicker && event.keysym.sym >= SDLK_1 && event.keysym.sym <= SDLK_4) {
        TourStop* ts = game_values.tourstops[iEditStage];
        if (ts->iStageType == 0) {
            short iPlace = event.keysym.sym - SDLK_1 + 1;
            TestAndSetBonusItem(*ts, iPlace);
        }
    } else if ((event.keysym.sym == SDLK_PAGEUP && iEditStage > 0) || (event.keysym.sym == SDLK_PAGEDOWN && iEditStage < world.stageCount() - 1)) {
        if (iEditStage != -1 && mCurrentMenu == &mStageSettingsMenu) {
            SaveStage(iEditStage);

            if (event.keysym.sym == SDLK_PAGEUP)
                iEditStage--;
            else if (event.keysym.sym == SDLK_PAGEDOWN)
                iEditStage++;

            EditStage(iEditStage);
            mCurrentMenu->ResetMenu();

            code = MENU_CODE_MODE_CHANGED;

            mModeOptionsMenu->Refresh();
        }
    }
}


void EditorStages::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.state != SDL_PRESSED)
        return;

    short iTileX = event.x / TILESIZE;
    short iTileY = event.y / TILESIZE;
    short iButtonX = event.x;
    short iButtonY = event.y;

    if (event.button == SDL_BUTTON_LEFT) {
        // Stages
        if (iEditStage == -1) {
            if (iTileX >= 0 && iTileX < world.stageCount() - (iTileY * 20) && iTileY >= 0 && iTileY <= (world.stageCount() - 1) / 20) {
                m_selectedTileId = iTileX + (iTileY * 20) + 6;
                newlyEntered = false;
                return;
            }
            // New stage button
            else if (iButtonX >= 256 && iButtonX < 384 && iButtonY >= 420 && iButtonY < 452) {
                NewStage(world, &iEditStage);
            }
        } else if (mCurrentMenu == &mBonusItemPicker) {
            TourStop* ts = game_values.tourstops[iEditStage];

                   // See if we clicked an item and add it if we did
            TestAndSetBonusItem(*ts, 0);

                   // See if we clicked an already added item and remove it
            for (short iRemoveItem = 0; iRemoveItem < ts->iNumBonuses; iRemoveItem++) {
                SDL_Rect* rects = rStageBonusDst;

                if (ts->iStageType == 1)
                    rects = rHouseBonusDst;

                if (iButtonX >= rects[iRemoveItem].x && iButtonX < rects[iRemoveItem].w + rects[iRemoveItem].x && iButtonY >= rects[iRemoveItem].y && iButtonY < rects[iRemoveItem].h + rects[iRemoveItem].y) {
                    for (short iAdjust = iRemoveItem; iAdjust < ts->iNumBonuses - 1; iAdjust++) {
                        ts->wsbBonuses[iAdjust].iBonus = ts->wsbBonuses[iAdjust + 1].iBonus;
                        ts->wsbBonuses[iAdjust].iWinnerPlace = ts->wsbBonuses[iAdjust + 1].iWinnerPlace;
                        strcpy(ts->wsbBonuses[iAdjust].szBonusString, ts->wsbBonuses[iAdjust + 1].szBonusString);
                    }

                    ts->iNumBonuses--;

                    break;
                }
            }
        } else {
            code = mCurrentMenu->MouseClick(iButtonX, iButtonY);
        }
    } else if (event.button == SDL_BUTTON_RIGHT) {
        if (iEditStage == -1) {
            if (iTileX >= 0 && iTileX < world.stageCount() - (iTileY * 20) && iTileY >= 0 && iTileY <= (world.stageCount() - 1) / 20) {
                iEditStage = iTileX + (iTileY * 20);
                EditStage(iEditStage);

                code = MENU_CODE_MODE_CHANGED;

                mModeOptionsMenu->Refresh();
            }
        }
    }
}


void EditorStages::onSetupMouseMotion(const SDL_MouseMotionEvent& event, WorldMap& world)
{
    iStageDisplay = -1;

    if (iEditStage == -1) {
        // if (event.button.x >= 0 && event.button.y >= 0)
        {
            short iMouseX = event.x / TILESIZE;
            short iMouseY = event.y / TILESIZE;

            if (iMouseX >= 0 && iMouseX < world.stageCount() - (iMouseY * 20) && iMouseY >= 0 && iMouseY <= (world.stageCount() - 1) / 20) {
                iStageDisplay = iMouseX + (iMouseY * 20);
            }
        }
    }
}


void EditorStages::updateMenu(WorldMap& world)
{
    if (iEditStage < 0)
        return;

    if (MENU_CODE_NONE == code) {
        code = mCurrentMenu->SendInput(&game_values.playerInput);
    }

    if (MENU_CODE_EXIT_APPLICATION == code) {
        // Save the current stage
        SaveStage(iEditStage);

               // We are no longer working on a specific stage
        iEditStage = -1;
    } else if (MENU_CODE_MODE_CHANGED == code) {
        short iMode = miModeField->currentValue();

        miPointsField->setVisible(iMode != 24);
        miFinalStageField->setVisible(iMode != 24);

        miMapField->setVisible(iMode != 24);

        miBonusType->setVisible(iMode == 24);
        miBonusTextField[0]->setVisible(iMode == 24);
        miBonusTextField[1]->setVisible(iMode == 24);
        miBonusTextField[2]->setVisible(iMode == 24);
        miBonusTextField[3]->setVisible(iMode == 24);
        miBonusTextField[4]->setVisible(iMode == 24);

        miSpecialGoalField[0]->setVisible(iMode == 25);
        miSpecialGoalField[1]->setVisible(iMode == 26);
        miSpecialGoalField[2]->setVisible(iMode == 27);

        miBonusItemsButton->SetPosition(430, iMode != 24 ? 220 : 340);

        if (iMode >= 0 && iMode < GAMEMODE_LAST) {
            miModeSettingsButton->setVisible(iMode != game_mode_owned);

            for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
                miGoalField[iGameMode]->setVisible(iMode == iGameMode);
            }

            miGoalField[iMode]->setOutputPtr(&game_values.tourstops[iEditStage]->iGoal);
            miGoalField[iMode]->updateOutput();

            game_values.tourstops[iEditStage]->iStageType = 0;

            game_values.tourstops[iEditStage]->fUseSettings = true;
            game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[iMode];
        } else {
            // Show the settings button for boss mode
            miModeSettingsButton->setVisible(iMode == 26);

            for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
                miGoalField[iGameMode]->setVisible(false);

            if (iMode == 24) {
                game_values.tourstops[iEditStage]->iStageType = 1;
                game_values.tourstops[iEditStage]->iBonusTextLines = 5;
            } else if (iMode >= 25 && iMode <= 27) {
                game_values.tourstops[iEditStage]->iStageType = 0;
            }
        }

               // Removes bonuses if we went from a stage to a bonus house
               //(and there were more than the max bonuses for a house)
        AdjustBonuses(game_values.tourstops[iEditStage]);

    } else if (MENU_CODE_TO_MODE_SETTINGS_MENU == code) {
        bool fModeFound = false;
        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
            if (miGoalField[iGameMode]->IsVisible()) {
                mCurrentMenu = mModeOptionsMenu->GetOptionsMenu(iGameMode);
                mCurrentMenu->ResetMenu();
                fModeFound = true;
                break;
            }
        }

               // Look to see if this is the boss mode and go to boss settings
        if (!fModeFound) {
            if (miSpecialGoalField[1]->IsVisible()) {
                mCurrentMenu = mModeOptionsMenu->GetBossOptionsMenu();
                mCurrentMenu->ResetMenu();
            }
        }
    } else if (MENU_CODE_HEALTH_MODE_START_LIFE_CHANGED == code) {
        mModeOptionsMenu->HealthModeStartLifeChanged();
    } else if (MENU_CODE_HEALTH_MODE_MAX_LIFE_CHANGED == code) {
        mModeOptionsMenu->HealthModeMaxLifeChanged();
    } else if (MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS == code) {
        mCurrentMenu = &mStageSettingsMenu;
        mCurrentMenu->ResetMenu();
    } else if (MENU_CODE_MAP_CHANGED == code) {
        game_values.tourstops[iEditStage]->pszMapFile = maplist->currentShortmapname();
    } else if (MENU_CODE_TO_BONUS_PICKER_MENU == code) {
        mCurrentMenu = &mBonusItemPicker;
        mCurrentMenu->ResetMenu();
    } else if (MENU_CODE_DELETE_STAGE_BUTTON == code) {
        miDeleteStageDialogImage->setVisible(true);
        miDeleteStageDialogAreYouText->setVisible(true);
        miDeleteStageDialogSureText->setVisible(true);
        miDeleteStageDialogYesButton->setVisible(true);
        miDeleteStageDialogNoButton->setVisible(true);

        EnableStageMenu(false);

        mStageSettingsMenu.RememberCurrent();

        mStageSettingsMenu.setInitialFocus(miDeleteStageDialogNoButton);
        mStageSettingsMenu.SetCancelCode(MENU_CODE_DELETE_STAGE_NO);
        mStageSettingsMenu.ResetMenu();
    } else if (MENU_CODE_DELETE_STAGE_YES == code || MENU_CODE_DELETE_STAGE_NO == code) {
        miDeleteStageDialogImage->setVisible(false);
        miDeleteStageDialogAreYouText->setVisible(false);
        miDeleteStageDialogSureText->setVisible(false);
        miDeleteStageDialogYesButton->setVisible(false);
        miDeleteStageDialogNoButton->setVisible(false);

        mStageSettingsMenu.setInitialFocus(miNameField);
        mStageSettingsMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);
        mStageSettingsMenu.RestoreCurrent();

               // Yes was selected to delete this stage
        if (MENU_CODE_DELETE_STAGE_YES == code) {
            // Scan the grid of stages and remove any references to this stage
            // and decrement stage numbers greater than this stage
            for (short iRow = 0; iRow < world.h(); iRow++) {
                for (short iCol = 0; iCol < world.w(); iCol++) {
                    if (world.getTiles().at(iCol, iRow).iType == iEditStage + 6) {
                        world.getTiles().at(iCol, iRow).iType = 0;
                    } else if (world.getTiles().at(iCol, iRow).iType > iEditStage + 6) {
                        world.getTiles().at(iCol, iRow).iType--;
                    }
                }
            }

            // Scan vehicles and remove references to deleted stage
            const auto eraseFrom = std::remove_if(
                world.getVehicles().begin(),
                world.getVehicles().end(),
                [this](const WorldVehicle& vehicle){ return vehicle.iActionId == iEditStage; });
            world.getVehicles().erase(eraseFrom, world.getVehicles().end());
            for (WorldVehicle& vehicle : world.getVehicles()) {
                if (vehicle.iActionId > iEditStage) {
                    vehicle.iActionId--;
                }
            }

                   // Remove stage from tourstops vector
            std::vector<TourStop*>::iterator itr = game_values.tourstops.begin(), lim = game_values.tourstops.end();

            short iIndex = 0;
            while (itr != lim) {
                if (iIndex == iEditStage) {
                    delete (*itr);

                    game_values.tourstops.erase(itr);
                    world.setStageCount(world.stageCount() - 1);

                    break;
                }

                ++itr;
                ++iIndex;
            }

            iEditStage = -1;
            mCurrentMenu = &mStageSettingsMenu;
            mCurrentMenu->ResetMenu();
        }

        EnableStageMenu(true);
    }
}


void EditorStages::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    int color = SDL_MapRGB(blitdest->format, 0, 0, 255);

    if (iEditStage == -1) {
        for (short iStage = 0; iStage < world.stageCount(); iStage++) {
            short ix = (iStage % 20) << 5;
            short iy = ((iStage / 20) << 5);

            SDL_Rect r = { ix, iy, 32, 32 };
            SDL_FillRect(blitdest, &r, color);

            rm.spr_worldforegroundspecial[0].draw(ix, iy, (iStage % 10) << 5, (iStage / 10) << 5, 32, 32);
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (iStageDisplay >= 0) {
            DisplayStageDetails(iStageDisplay, mouseX, mouseY, rm);
        }

               // Display New button
        rm.spr_selectfield.draw(256, 420, 0, 0, 64, 32);
        rm.spr_selectfield.draw(320, 420, 448, 0, 64, 32);

        rm.menu_font_large.drawCentered(320, 425, "New Stage");

        rm.menu_font_small.draw(0, 480 - rm.menu_font_small.getHeight(), "[LMB] Select Stage, [RMB] Edit Stage, [n] New Stage");
    } else {
        mCurrentMenu->Update();
        mCurrentMenu->Draw();

        if (mCurrentMenu != &mBonusItemPicker) {
            short ix = 20;
            short iy = 20;

            SDL_Rect r = { ix, iy, 32, 32 };
            SDL_FillRect(blitdest, &r, color);

            rm.spr_worldforegroundspecial[0].draw(ix, iy, (iEditStage % 10) << 5, (iEditStage / 10) << 5, 32, 32);
        }
    }

    if (mCurrentMenu == &mBonusItemPicker) {
        TourStop* ts = game_values.tourstops[iEditStage];

               // Game powerups
        for (short iItem = 0; iItem < NUM_POWERUPS; iItem++) {
            rm.spr_storedpoweruplarge.draw(rItemDst[iItem].x, rItemDst[iItem].y, iItem << 5, 0, 32, 32);
        }

               // World Powerups
        for (short iWorldItem = 0; iWorldItem < NUM_WORLD_POWERUPS; iWorldItem++) {
            rm.spr_worlditems.draw(rItemDst[iWorldItem + NUM_POWERUPS].x, rItemDst[iWorldItem + NUM_POWERUPS].y, iWorldItem << 5, 0, 32, 32);
        }

               // Score Bonuses
        if (ts->iStageType == 1) {
            for (short iScoreBonus = 0; iScoreBonus < NUM_WORLD_SCORE_BONUSES; iScoreBonus++) {
                rm.spr_worlditems.draw(rItemDst[iScoreBonus + NUM_POWERUPS + NUM_WORLD_POWERUPS].x, rItemDst[iScoreBonus + NUM_POWERUPS + NUM_WORLD_POWERUPS].y, iScoreBonus < 10 ? iScoreBonus << 5 : (iScoreBonus - 10) << 5, iScoreBonus < 10 ? 32 : 64, 32, 32);
            }
        }

               // Draw background container
        rm.spr_worlditempopup.draw(0, 344, 0, 0, 320, 64);
        rm.spr_worlditempopup.draw(320, 344, 192, 0, 320, 64);

        SDL_Rect* rects = rStageBonusDst;

        if (ts->iStageType == 1)
            rects = rHouseBonusDst;

        for (short iPickedItem = 0; iPickedItem < ts->iNumBonuses; iPickedItem++) {
            short iBonus = ts->wsbBonuses[iPickedItem].iBonus;
            short iPlace = ts->wsbBonuses[iPickedItem].iWinnerPlace;

                   // Draw place behind bonus
            if (ts->iStageType == 0)
                rm.spr_worlditempopup.draw(rects[iPickedItem].x - 8, rects[iPickedItem].y - 8, iPlace * 48, 256, 48, 48);

            if (iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {
                short iBonusIndex = iBonus - NUM_POWERUPS - NUM_WORLD_POWERUPS;
                rm.spr_worlditems.draw(rects[iPickedItem].x, rects[iPickedItem].y, iBonusIndex < 10 ? iBonusIndex << 5 : (iBonusIndex - 10) << 5, iBonusIndex < 10 ? 32 : 64, 32, 32);
            } else if (iBonus >= NUM_POWERUPS) {
                rm.spr_worlditems.draw(rects[iPickedItem].x, rects[iPickedItem].y, (iBonus - NUM_POWERUPS) << 5, 0, 32, 32);
            } else {
                rm.spr_storedpoweruplarge.draw(rects[iPickedItem].x, rects[iPickedItem].y, iBonus << 5, 0, 32, 32);
            }
        }

        if (ts->iStageType == 0)
            rm.menu_font_small.draw(0, 480 - rm.menu_font_small.getHeight(), "[1-4] Select Items, [LMB] Remove Items");
        else
            rm.menu_font_small.draw(0, 480 - rm.menu_font_small.getHeight(), "[LMB] Select Items, [LMB] Remove Items");
    }
}


bool EditorStages::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    bool changed = false;

    if (button == SDL_BUTTON_LEFT) {
        // if the stage was placed on a start tile
        if (tile.iType == 1) {
            tile.iForegroundSprite = 0;
            changed = true;
        }

        tile.iType = m_selectedTileId;
    } else {
        tile.iType = 0;
    }

    return changed;
}

