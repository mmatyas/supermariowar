#include "ModeOptionsMenu.h"

#include "GameValues.h"
#include "GameMode.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;


UI_ModeOptionsMenu::UI_ModeOptionsMenu() : UI_Menu()
{

    //***********************
    // Classic Mode Settings
    //***********************

    miClassicModeStyleField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "On Kill", 400, 180);
    miClassicModeStyleField->Add("Respawn", 0, "", false, false);
    miClassicModeStyleField->Add("Shield", 1, "", false, false);
    miClassicModeStyleField->SetData(&game_values.gamemodemenusettings.classic.style, NULL, NULL);
    miClassicModeStyleField->SetKey(game_values.gamemodemenusettings.classic.style);

    miClassicModeScoringField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Scoring", 400, 180);
    miClassicModeScoringField->Add("All Kills", 0, "", false, false);
    miClassicModeScoringField->Add("Push Kills Only", 1, "", false, false, false);
    miClassicModeScoringField->SetData(&game_values.gamemodemenusettings.classic.scoring, NULL, NULL);
    miClassicModeScoringField->SetKey(game_values.gamemodemenusettings.classic.scoring);

    miClassicModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miClassicModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miClassicModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miClassicModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miClassicModeHeaderText = new MI_Text("Classic Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[0].AddControl(miClassicModeStyleField, miClassicModeBackButton, miClassicModeScoringField, NULL, miClassicModeBackButton);
    mModeSettingsMenu[0].AddControl(miClassicModeScoringField, miClassicModeStyleField, miClassicModeBackButton, NULL, miClassicModeBackButton);
    mModeSettingsMenu[0].AddControl(miClassicModeBackButton, miClassicModeScoringField, miClassicModeStyleField, miClassicModeScoringField, NULL);

    mModeSettingsMenu[0].AddNonControl(miClassicModeLeftHeaderBar);
    mModeSettingsMenu[0].AddNonControl(miClassicModeRightHeaderBar);
    mModeSettingsMenu[0].AddNonControl(miClassicModeHeaderText);

    mModeSettingsMenu[0].SetHeadControl(miClassicModeStyleField);
    mModeSettingsMenu[0].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Frag Mode Settings
    //***********************

    miFragModeStyleField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "On Kill", 400, 180);
    miFragModeStyleField->Add("Respawn", 0, "", false, false);
    miFragModeStyleField->Add("Shield", 1, "", false, false);
    miFragModeStyleField->SetData(&game_values.gamemodemenusettings.frag.style, NULL, NULL);
    miFragModeStyleField->SetKey(game_values.gamemodemenusettings.frag.style);

    miFragModeScoringField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Scoring", 400, 180);
    miFragModeScoringField->Add("All Kills", 0, "", false, false);
    miFragModeScoringField->Add("Push Kills Only", 1, "", false, false, false);
    miFragModeScoringField->SetData(&game_values.gamemodemenusettings.frag.scoring, NULL, NULL);
    miFragModeScoringField->SetKey(game_values.gamemodemenusettings.frag.scoring);

    miFragModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miFragModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miFragModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miFragModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miFragModeHeaderText = new MI_Text("Frag Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[1].AddControl(miFragModeStyleField, miFragModeBackButton, miFragModeScoringField, NULL, miFragModeBackButton);
    mModeSettingsMenu[1].AddControl(miFragModeScoringField, miFragModeStyleField, miFragModeBackButton, NULL, miFragModeBackButton);
    mModeSettingsMenu[1].AddControl(miFragModeBackButton, miFragModeScoringField, miFragModeStyleField, miFragModeScoringField, NULL);

    mModeSettingsMenu[1].AddNonControl(miFragModeLeftHeaderBar);
    mModeSettingsMenu[1].AddNonControl(miFragModeRightHeaderBar);
    mModeSettingsMenu[1].AddNonControl(miFragModeHeaderText);

    mModeSettingsMenu[1].SetHeadControl(miFragModeStyleField);
    mModeSettingsMenu[1].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Time Limit Mode Settings
    //***********************

    miTimeLimitModeStyleField = new MI_SelectField(&rm->spr_selectfield, 120, 180, "On Kill", 400, 150);
    miTimeLimitModeStyleField->Add("Respawn", 0, "", false, false);
    miTimeLimitModeStyleField->Add("Shield", 1, "", false, false);
    miTimeLimitModeStyleField->SetData(&game_values.gamemodemenusettings.time.style, NULL, NULL);
    miTimeLimitModeStyleField->SetKey(game_values.gamemodemenusettings.time.style);

    miTimeLimitModeScoringField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Scoring", 400, 150);
    miTimeLimitModeScoringField->Add("All Kills", 0, "", false, false);
    miTimeLimitModeScoringField->Add("Push Kills Only", 1, "", false, false, false);
    miTimeLimitModeScoringField->SetData(&game_values.gamemodemenusettings.time.scoring, NULL, NULL);
    miTimeLimitModeScoringField->SetKey(game_values.gamemodemenusettings.time.scoring);

    miTimeLimitModePercentExtraTime = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Time", 400, 150, 384);
    miTimeLimitModePercentExtraTime->Add("0", 0, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("5", 5, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("10", 10, "", false, false);
    miTimeLimitModePercentExtraTime->Add("15", 15, "", false, false);
    miTimeLimitModePercentExtraTime->Add("20", 20, "", false, false);
    miTimeLimitModePercentExtraTime->Add("25", 25, "", false, false);
    miTimeLimitModePercentExtraTime->Add("30", 30, "", false, false);
    miTimeLimitModePercentExtraTime->Add("35", 35, "", false, false);
    miTimeLimitModePercentExtraTime->Add("40", 40, "", false, false);
    miTimeLimitModePercentExtraTime->Add("45", 45, "", false, false);
    miTimeLimitModePercentExtraTime->Add("50", 50, "", false, false);
    miTimeLimitModePercentExtraTime->Add("55", 55, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("60", 60, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("65", 65, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("70", 70, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("75", 75, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("80", 80, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("85", 85, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("90", 90, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("95", 95, "", false, false, false);
    miTimeLimitModePercentExtraTime->Add("100", 100, "", false, false, false);
    miTimeLimitModePercentExtraTime->SetData(&game_values.gamemodemenusettings.time.percentextratime, NULL, NULL);
    miTimeLimitModePercentExtraTime->SetKey(game_values.gamemodemenusettings.time.percentextratime);
    miTimeLimitModePercentExtraTime->SetNoWrap(true);

    miTimeLimitModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miTimeLimitModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miTimeLimitModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTimeLimitModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTimeLimitModeHeaderText = new MI_Text("Time Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[2].AddControl(miTimeLimitModeStyleField, miTimeLimitModeBackButton, miTimeLimitModeScoringField, NULL, miTimeLimitModeBackButton);
    mModeSettingsMenu[2].AddControl(miTimeLimitModeScoringField, miTimeLimitModeStyleField, miTimeLimitModePercentExtraTime, NULL, miTimeLimitModeBackButton);
    mModeSettingsMenu[2].AddControl(miTimeLimitModePercentExtraTime, miTimeLimitModeScoringField, miTimeLimitModeBackButton, NULL, miTimeLimitModeBackButton);
    mModeSettingsMenu[2].AddControl(miTimeLimitModeBackButton, miTimeLimitModePercentExtraTime, miTimeLimitModeStyleField, miTimeLimitModePercentExtraTime, NULL);

    mModeSettingsMenu[2].AddNonControl(miTimeLimitModeLeftHeaderBar);
    mModeSettingsMenu[2].AddNonControl(miTimeLimitModeRightHeaderBar);
    mModeSettingsMenu[2].AddNonControl(miTimeLimitModeHeaderText);

    mModeSettingsMenu[2].SetHeadControl(miTimeLimitModeStyleField);
    mModeSettingsMenu[2].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Jail Mode Settings
    //***********************

    miJailModeStyleField = new MI_SelectField(&rm->spr_selectfield, 120, 160, "Style", 400, 150);
    miJailModeStyleField->Add("Classic", 0, "", false, false);
    miJailModeStyleField->Add("Owned", 1, "", true, false);
    miJailModeStyleField->Add("Free For All", 2, "", true, false);
    miJailModeStyleField->SetData(&game_values.gamemodemenusettings.jail.style, NULL, NULL);
    miJailModeStyleField->SetKey(game_values.gamemodemenusettings.jail.style);

    miJailModeTimeFreeField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "Free Timer", 400, 150);
    miJailModeTimeFreeField->Add("None", 1, "", false, false);
    miJailModeTimeFreeField->Add("5 Seconds", 310, "", false, false, false);
    miJailModeTimeFreeField->Add("10 Seconds", 620, "", false, false);
    miJailModeTimeFreeField->Add("15 Seconds", 930, "", false, false);
    miJailModeTimeFreeField->Add("20 Seconds", 1240, "", false, false);
    miJailModeTimeFreeField->Add("25 Seconds", 1550, "", false, false);
    miJailModeTimeFreeField->Add("30 Seconds", 1860, "", false, false);
    miJailModeTimeFreeField->Add("35 Seconds", 2170, "", false, false);
    miJailModeTimeFreeField->Add("40 Seconds", 2480, "", false, false);
    miJailModeTimeFreeField->Add("45 Seconds", 2790, "", false, false, false);
    miJailModeTimeFreeField->Add("50 Seconds", 3100, "", false, false, false);
    miJailModeTimeFreeField->Add("55 Seconds", 3410, "", false, false, false);
    miJailModeTimeFreeField->Add("60 Seconds", 3720, "", false, false, false);
    miJailModeTimeFreeField->SetData(&game_values.gamemodemenusettings.jail.timetofree, NULL, NULL);
    miJailModeTimeFreeField->SetKey(game_values.gamemodemenusettings.jail.timetofree);

    miJailModeTagFreeField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Tag Free", 400, 150);
    miJailModeTagFreeField->Add("Off", 0, "", false, false);
    miJailModeTagFreeField->Add("On", 1, "", true, false);
    miJailModeTagFreeField->SetData(NULL, NULL, &game_values.gamemodemenusettings.jail.tagfree);
    miJailModeTagFreeField->SetKey(game_values.gamemodemenusettings.jail.tagfree ? 1 : 0);
    miJailModeTagFreeField->SetAutoAdvance(true);

    miJailModeJailKeyField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 280, "Jail Key", 400, 150, 384);
    miJailModeJailKeyField->Add("0", 0, "", false, false, false);
    miJailModeJailKeyField->Add("5", 5, "", false, false, false);
    miJailModeJailKeyField->Add("10", 10, "", false, false);
    miJailModeJailKeyField->Add("15", 15, "", false, false);
    miJailModeJailKeyField->Add("20", 20, "", false, false);
    miJailModeJailKeyField->Add("25", 25, "", false, false);
    miJailModeJailKeyField->Add("30", 30, "", false, false);
    miJailModeJailKeyField->Add("35", 35, "", false, false);
    miJailModeJailKeyField->Add("40", 40, "", false, false);
    miJailModeJailKeyField->Add("45", 45, "", false, false);
    miJailModeJailKeyField->Add("50", 50, "", false, false);
    miJailModeJailKeyField->Add("55", 55, "", false, false, false);
    miJailModeJailKeyField->Add("60", 60, "", false, false, false);
    miJailModeJailKeyField->Add("65", 65, "", false, false, false);
    miJailModeJailKeyField->Add("70", 70, "", false, false, false);
    miJailModeJailKeyField->Add("75", 75, "", false, false, false);
    miJailModeJailKeyField->Add("80", 80, "", false, false, false);
    miJailModeJailKeyField->Add("85", 85, "", false, false, false);
    miJailModeJailKeyField->Add("90", 90, "", false, false, false);
    miJailModeJailKeyField->Add("95", 95, "", false, false, false);
    miJailModeJailKeyField->Add("100", 100, "", false, false, false);
    miJailModeJailKeyField->SetData(&game_values.gamemodemenusettings.jail.percentkey, NULL, NULL);
    miJailModeJailKeyField->SetKey(game_values.gamemodemenusettings.jail.percentkey);
    miJailModeJailKeyField->SetNoWrap(true);

    miJailModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miJailModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miJailModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miJailModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miJailModeHeaderText = new MI_Text("Jail Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[3].AddControl(miJailModeStyleField, miJailModeBackButton, miJailModeTimeFreeField, NULL, miJailModeBackButton);
    mModeSettingsMenu[3].AddControl(miJailModeTimeFreeField, miJailModeStyleField, miJailModeTagFreeField, NULL, miJailModeBackButton);
    mModeSettingsMenu[3].AddControl(miJailModeTagFreeField, miJailModeTimeFreeField, miJailModeJailKeyField, NULL, miJailModeBackButton);
    mModeSettingsMenu[3].AddControl(miJailModeJailKeyField, miJailModeTagFreeField, miJailModeBackButton, NULL, miJailModeBackButton);
    mModeSettingsMenu[3].AddControl(miJailModeBackButton, miJailModeJailKeyField, miJailModeStyleField, miJailModeJailKeyField, NULL);

    mModeSettingsMenu[3].AddNonControl(miJailModeLeftHeaderBar);
    mModeSettingsMenu[3].AddNonControl(miJailModeRightHeaderBar);
    mModeSettingsMenu[3].AddNonControl(miJailModeHeaderText);

    mModeSettingsMenu[3].SetHeadControl(miJailModeStyleField);
    mModeSettingsMenu[3].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Coins Mode Settings
    //***********************

    miCoinModePenaltyField = new MI_SelectField(&rm->spr_selectfield, 120, 180, "Penalty", 400, 150);
    miCoinModePenaltyField->Add("Off", 0, "", false, false);
    miCoinModePenaltyField->Add("On", 1, "", true, false);
    miCoinModePenaltyField->SetData(NULL, NULL, &game_values.gamemodemenusettings.coins.penalty);
    miCoinModePenaltyField->SetKey(game_values.gamemodemenusettings.coins.penalty ? 1 : 0);
    miCoinModePenaltyField->SetAutoAdvance(true);

    miCoinModeQuantityField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Quantity", 400, 150);
    miCoinModeQuantityField->Add("1", 1, "", false, false);
    miCoinModeQuantityField->Add("2", 2, "", false, false);
    miCoinModeQuantityField->Add("3", 3, "", false, false);
    miCoinModeQuantityField->Add("4", 4, "", false, false);
    miCoinModeQuantityField->Add("5", 5, "", false, false);
    miCoinModeQuantityField->SetData(&game_values.gamemodemenusettings.coins.quantity, NULL, NULL);
    miCoinModeQuantityField->SetKey(game_values.gamemodemenusettings.coins.quantity);

    miCoinModePercentExtraCoin = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Coins", 400, 150, 384);
    miCoinModePercentExtraCoin->Add("0", 0, "", false, false, false);
    miCoinModePercentExtraCoin->Add("5", 5, "", false, false, false);
    miCoinModePercentExtraCoin->Add("10", 10, "", false, false);
    miCoinModePercentExtraCoin->Add("15", 15, "", false, false);
    miCoinModePercentExtraCoin->Add("20", 20, "", false, false);
    miCoinModePercentExtraCoin->Add("25", 25, "", false, false);
    miCoinModePercentExtraCoin->Add("30", 30, "", false, false);
    miCoinModePercentExtraCoin->Add("35", 35, "", false, false);
    miCoinModePercentExtraCoin->Add("40", 40, "", false, false);
    miCoinModePercentExtraCoin->Add("45", 45, "", false, false);
    miCoinModePercentExtraCoin->Add("50", 50, "", false, false);
    miCoinModePercentExtraCoin->Add("55", 55, "", false, false, false);
    miCoinModePercentExtraCoin->Add("60", 60, "", false, false, false);
    miCoinModePercentExtraCoin->Add("65", 65, "", false, false, false);
    miCoinModePercentExtraCoin->Add("70", 70, "", false, false, false);
    miCoinModePercentExtraCoin->Add("75", 75, "", false, false, false);
    miCoinModePercentExtraCoin->Add("80", 80, "", false, false, false);
    miCoinModePercentExtraCoin->Add("85", 85, "", false, false, false);
    miCoinModePercentExtraCoin->Add("90", 90, "", false, false, false);
    miCoinModePercentExtraCoin->Add("95", 95, "", false, false, false);
    miCoinModePercentExtraCoin->Add("100", 100, "", false, false, false);
    miCoinModePercentExtraCoin->SetData(&game_values.gamemodemenusettings.coins.percentextracoin, NULL, NULL);
    miCoinModePercentExtraCoin->SetKey(game_values.gamemodemenusettings.coins.percentextracoin);
    miCoinModePercentExtraCoin->SetNoWrap(true);

    miCoinModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miCoinModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miCoinModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miCoinModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miCoinModeHeaderText = new MI_Text("Coin Collection Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[4].AddControl(miCoinModePenaltyField, miCoinModeBackButton, miCoinModeQuantityField, NULL, miCoinModeBackButton);
    mModeSettingsMenu[4].AddControl(miCoinModeQuantityField, miCoinModePenaltyField, miCoinModePercentExtraCoin, NULL, miCoinModeBackButton);
    mModeSettingsMenu[4].AddControl(miCoinModePercentExtraCoin, miCoinModeQuantityField, miCoinModeBackButton, NULL, miCoinModeBackButton);
    mModeSettingsMenu[4].AddControl(miCoinModeBackButton, miCoinModePercentExtraCoin, miCoinModePenaltyField, miCoinModePercentExtraCoin, NULL);

    mModeSettingsMenu[4].AddNonControl(miCoinModeLeftHeaderBar);
    mModeSettingsMenu[4].AddNonControl(miCoinModeRightHeaderBar);
    mModeSettingsMenu[4].AddNonControl(miCoinModeHeaderText);

    mModeSettingsMenu[4].SetHeadControl(miCoinModePenaltyField);
    mModeSettingsMenu[4].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Stomp Mode Settings
    //***********************

    miStompModeRateField = new MI_SelectField(&rm->spr_selectfield, 120, 40, "Rate", 400, 180);
    miStompModeRateField->Add("Very Slow", 150, "", false, false, false);
    miStompModeRateField->Add("Slow", 120, "", false, false);
    miStompModeRateField->Add("Moderate", 90, "", false, false);
    miStompModeRateField->Add("Fast", 60, "", false, false);
    miStompModeRateField->Add("Very Fast", 30, "", false, false, false);
    miStompModeRateField->SetData(&game_values.gamemodemenusettings.stomp.rate, NULL, NULL);
    miStompModeRateField->SetKey(game_values.gamemodemenusettings.stomp.rate);

    for (short iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++) {
        miStompModeEnemySlider[iEnemy] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->menu_stomp, 120, 80 + 40 * iEnemy, 400, iEnemy);
        miStompModeEnemySlider[iEnemy]->Add("", 0, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 1, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 2, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 3, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 4, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 5, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 6, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 7, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 8, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 9, "", false, false);
        miStompModeEnemySlider[iEnemy]->Add("", 10, "", false, false);
        miStompModeEnemySlider[iEnemy]->SetNoWrap(true);
        miStompModeEnemySlider[iEnemy]->SetData(&game_values.gamemodemenusettings.stomp.enemyweight[iEnemy], NULL, NULL);
        miStompModeEnemySlider[iEnemy]->SetKey(game_values.gamemodemenusettings.stomp.enemyweight[iEnemy]);
    }

    miStompModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miStompModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miStompModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miStompModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miStompModeHeaderText = new MI_Text("Stomp Mode Menu", 320, 5, 0, 2, 1);


    mModeSettingsMenu[5].AddControl(miStompModeRateField, miStompModeBackButton, miStompModeEnemySlider[0], NULL, miStompModeBackButton);

    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[0], miStompModeRateField,      miStompModeEnemySlider[1], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[1], miStompModeEnemySlider[0], miStompModeEnemySlider[2], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[2], miStompModeEnemySlider[1], miStompModeEnemySlider[3], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[3], miStompModeEnemySlider[2], miStompModeEnemySlider[4], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[4], miStompModeEnemySlider[3], miStompModeEnemySlider[5], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[5], miStompModeEnemySlider[4], miStompModeEnemySlider[6], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[6], miStompModeEnemySlider[5], miStompModeEnemySlider[7], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[7], miStompModeEnemySlider[6], miStompModeEnemySlider[8], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[8], miStompModeEnemySlider[7], miStompModeBackButton,     NULL, miStompModeBackButton);

    mModeSettingsMenu[5].AddControl(miStompModeBackButton, miStompModeEnemySlider[8], miStompModeRateField, miStompModeEnemySlider[8], NULL);

    mModeSettingsMenu[5].AddNonControl(miStompModeLeftHeaderBar);
    mModeSettingsMenu[5].AddNonControl(miStompModeRightHeaderBar);
    mModeSettingsMenu[5].AddNonControl(miStompModeHeaderText);

    mModeSettingsMenu[5].SetHeadControl(miStompModeRateField);
    mModeSettingsMenu[5].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Yoshi's Eggs Mode Settings
    //***********************

    for (short iEggField = 0; iEggField < 4; iEggField++) {
        miEggModeEggQuantityField[iEggField] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->menu_egg, 170, 60 + 40 * iEggField, 300, iEggField);
        miEggModeEggQuantityField[iEggField]->Add("0", 0, "", false, false, iEggField == 0 ? false : true);
        miEggModeEggQuantityField[iEggField]->Add("1", 1, "", false, false, iEggField >= 2 ? false : true);
        miEggModeEggQuantityField[iEggField]->Add("2", 2, "", false, false, iEggField >= 2 ? false : true);
        miEggModeEggQuantityField[iEggField]->Add("3", 3, "", false, false, iEggField >= 1 ? false : true);
        miEggModeEggQuantityField[iEggField]->Add("4", 4, "", false, false, false);
        miEggModeEggQuantityField[iEggField]->SetData(&game_values.gamemodemenusettings.egg.eggs[iEggField], NULL, NULL);
        miEggModeEggQuantityField[iEggField]->SetKey(game_values.gamemodemenusettings.egg.eggs[iEggField]);
        miEggModeEggQuantityField[iEggField]->SetNoWrap(true);
    }

    for (short iYoshiField = 0; iYoshiField < 4; iYoshiField++) {
        miEggModeYoshiQuantityField[iYoshiField] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->menu_egg, 170, 220 + 40 * iYoshiField, 300, iYoshiField + 4);
        miEggModeYoshiQuantityField[iYoshiField]->Add("0", 0, "", false, false, iYoshiField == 0 ? false : true);
        miEggModeYoshiQuantityField[iYoshiField]->Add("1", 1, "", false, false, iYoshiField >= 2 ? false : true);
        miEggModeYoshiQuantityField[iYoshiField]->Add("2", 2, "", false, false, iYoshiField >= 1 ? false : true);
        miEggModeYoshiQuantityField[iYoshiField]->Add("3", 3, "", false, false, false);
        miEggModeYoshiQuantityField[iYoshiField]->Add("4", 4, "", false, false, false);
        miEggModeYoshiQuantityField[iYoshiField]->SetData(&game_values.gamemodemenusettings.egg.yoshis[iYoshiField], NULL, NULL);
        miEggModeYoshiQuantityField[iYoshiField]->SetKey(game_values.gamemodemenusettings.egg.yoshis[iYoshiField]);
        miEggModeYoshiQuantityField[iYoshiField]->SetNoWrap(true);
    }

    miEggModeExplosionTimeField = new MI_SelectField(&rm->spr_selectfield, 120, 380, "Explosion Timer", 400, 180);
    miEggModeExplosionTimeField->Add("Off", 0, "", false, false);
    miEggModeExplosionTimeField->Add("3 Seconds", 3, "", false, false, false);
    miEggModeExplosionTimeField->Add("5 Seconds", 5, "", false, false);
    miEggModeExplosionTimeField->Add("8 Seconds", 8, "", false, false);
    miEggModeExplosionTimeField->Add("10 Seconds", 10, "", false, false);
    miEggModeExplosionTimeField->Add("15 Seconds", 15, "", false, false);
    miEggModeExplosionTimeField->Add("20 Seconds", 20, "", false, false);
    miEggModeExplosionTimeField->SetData(&game_values.gamemodemenusettings.egg.explode, NULL, NULL);
    miEggModeExplosionTimeField->SetKey(game_values.gamemodemenusettings.egg.explode);

    miEggModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miEggModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miEggModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miEggModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miEggModeHeaderText = new MI_Text("Yoshi's Eggs Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[6].AddControl(miEggModeEggQuantityField[0], miEggModeBackButton, miEggModeEggQuantityField[1], NULL, miEggModeBackButton);
    mModeSettingsMenu[6].AddControl(miEggModeEggQuantityField[1], miEggModeEggQuantityField[0], miEggModeEggQuantityField[2], NULL, miEggModeBackButton);
    mModeSettingsMenu[6].AddControl(miEggModeEggQuantityField[2], miEggModeEggQuantityField[1], miEggModeEggQuantityField[3], NULL, miEggModeBackButton);
    mModeSettingsMenu[6].AddControl(miEggModeEggQuantityField[3], miEggModeEggQuantityField[2], miEggModeYoshiQuantityField[0], NULL, miEggModeBackButton);

    mModeSettingsMenu[6].AddControl(miEggModeYoshiQuantityField[0], miEggModeEggQuantityField[3], miEggModeYoshiQuantityField[1], NULL, miEggModeBackButton);
    mModeSettingsMenu[6].AddControl(miEggModeYoshiQuantityField[1], miEggModeYoshiQuantityField[0], miEggModeYoshiQuantityField[2], NULL, miEggModeBackButton);
    mModeSettingsMenu[6].AddControl(miEggModeYoshiQuantityField[2], miEggModeYoshiQuantityField[1], miEggModeYoshiQuantityField[3], NULL, miEggModeBackButton);
    mModeSettingsMenu[6].AddControl(miEggModeYoshiQuantityField[3], miEggModeYoshiQuantityField[2], miEggModeExplosionTimeField, NULL, miEggModeBackButton);

    mModeSettingsMenu[6].AddControl(miEggModeExplosionTimeField, miEggModeYoshiQuantityField[3], miEggModeBackButton, NULL, miEggModeBackButton);

    mModeSettingsMenu[6].AddControl(miEggModeBackButton, miEggModeExplosionTimeField, miEggModeEggQuantityField[0], miEggModeExplosionTimeField, NULL);

    mModeSettingsMenu[6].AddNonControl(miEggModeLeftHeaderBar);
    mModeSettingsMenu[6].AddNonControl(miEggModeRightHeaderBar);
    mModeSettingsMenu[6].AddNonControl(miEggModeHeaderText);

    mModeSettingsMenu[6].SetHeadControl(miEggModeEggQuantityField[0]);
    mModeSettingsMenu[6].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    //***********************
    // Flag Mode Settings
    //***********************

    miFlagModeSpeedField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 120, "Speed", 400, 180, 380);
    miFlagModeSpeedField->Add("0", 0, "", false, false);
    miFlagModeSpeedField->Add("1", 1, "", false, false);
    miFlagModeSpeedField->Add("2", 2, "", false, false);
    miFlagModeSpeedField->Add("3", 3, "", false, false);
    miFlagModeSpeedField->Add("4", 4, "", false, false);
    miFlagModeSpeedField->Add("5", 5, "", false, false);
    miFlagModeSpeedField->Add("6", 6, "", false, false, false);
    miFlagModeSpeedField->Add("7", 7, "", false, false, false);
    miFlagModeSpeedField->Add("8", 8, "", false, false, false);
    miFlagModeSpeedField->SetData(&game_values.gamemodemenusettings.flag.speed, NULL, NULL);
    miFlagModeSpeedField->SetKey(game_values.gamemodemenusettings.flag.speed);
    miFlagModeSpeedField->SetNoWrap(true);

    miFlagModeTouchReturnField = new MI_SelectField(&rm->spr_selectfield, 120, 160, "Touch Return", 400, 180);
    miFlagModeTouchReturnField->Add("Off", 0, "", false, false);
    miFlagModeTouchReturnField->Add("On", 1, "", true, false);
    miFlagModeTouchReturnField->SetData(NULL, NULL, &game_values.gamemodemenusettings.flag.touchreturn);
    miFlagModeTouchReturnField->SetKey(game_values.gamemodemenusettings.flag.touchreturn ? 1 : 0);
    miFlagModeTouchReturnField->SetAutoAdvance(true);

    miFlagModePointMoveField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "Point Move", 400, 180);
    miFlagModePointMoveField->Add("Off", 0, "", false, false);
    miFlagModePointMoveField->Add("On", 1, "", true, false);
    miFlagModePointMoveField->SetData(NULL, NULL, &game_values.gamemodemenusettings.flag.pointmove);
    miFlagModePointMoveField->SetKey(game_values.gamemodemenusettings.flag.pointmove ? 1 : 0);
    miFlagModePointMoveField->SetAutoAdvance(true);

    miFlagModeAutoReturnField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Auto Return", 400, 180);
    miFlagModeAutoReturnField->Add("None", 0, "", false, false);
    miFlagModeAutoReturnField->Add("5 Seconds", 310, "", false, false, false);
    miFlagModeAutoReturnField->Add("10 Seconds", 620, "", false, false);
    miFlagModeAutoReturnField->Add("15 Seconds", 930, "", false, false, false);
    miFlagModeAutoReturnField->Add("20 Seconds", 1240, "", false, false);
    miFlagModeAutoReturnField->Add("25 Seconds", 1550, "", false, false, false);
    miFlagModeAutoReturnField->Add("30 Seconds", 1860, "", false, false, false);
    miFlagModeAutoReturnField->Add("35 Seconds", 2170, "", false, false, false);
    miFlagModeAutoReturnField->Add("40 Seconds", 2480, "", false, false, false);
    miFlagModeAutoReturnField->Add("45 Seconds", 2790, "", false, false, false);
    miFlagModeAutoReturnField->Add("50 Seconds", 3100, "", false, false, false);
    miFlagModeAutoReturnField->Add("55 Seconds", 3410, "", false, false, false);
    miFlagModeAutoReturnField->Add("60 Seconds", 3720, "", false, false, false);
    miFlagModeAutoReturnField->SetData(&game_values.gamemodemenusettings.flag.autoreturn, NULL, NULL);
    miFlagModeAutoReturnField->SetKey(game_values.gamemodemenusettings.flag.autoreturn);

    miFlagModeHomeScoreField = new MI_SelectField(&rm->spr_selectfield, 120, 280, "Need Home", 400, 180);
    miFlagModeHomeScoreField->Add("Off", 0, "", false, false);
    miFlagModeHomeScoreField->Add("On", 1, "", true, false);
    miFlagModeHomeScoreField->SetData(NULL, NULL, &game_values.gamemodemenusettings.flag.homescore);
    miFlagModeHomeScoreField->SetKey(game_values.gamemodemenusettings.flag.homescore ? 1 : 0);
    miFlagModeHomeScoreField->SetAutoAdvance(true);

    miFlagModeCenterFlagField = new MI_SelectField(&rm->spr_selectfield, 120, 320, "Center Flag", 400, 180);
    miFlagModeCenterFlagField->Add("Off", 0, "", false, false);
    miFlagModeCenterFlagField->Add("On", 1, "", true, false);
    miFlagModeCenterFlagField->SetData(NULL, NULL, &game_values.gamemodemenusettings.flag.centerflag);
    miFlagModeCenterFlagField->SetKey(game_values.gamemodemenusettings.flag.centerflag ? 1 : 0);
    miFlagModeCenterFlagField->SetAutoAdvance(true);

    miFlagModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miFlagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miFlagModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miFlagModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miFlagModeHeaderText = new MI_Text("Capture The Flag Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[7].AddControl(miFlagModeSpeedField, miFlagModeBackButton, miFlagModeTouchReturnField, NULL, miFlagModeBackButton);
    mModeSettingsMenu[7].AddControl(miFlagModeTouchReturnField, miFlagModeSpeedField, miFlagModePointMoveField, NULL, miFlagModeBackButton);
    mModeSettingsMenu[7].AddControl(miFlagModePointMoveField, miFlagModeTouchReturnField, miFlagModeAutoReturnField, NULL, miFlagModeBackButton);
    mModeSettingsMenu[7].AddControl(miFlagModeAutoReturnField, miFlagModePointMoveField, miFlagModeHomeScoreField, NULL, miFlagModeBackButton);
    mModeSettingsMenu[7].AddControl(miFlagModeHomeScoreField, miFlagModeAutoReturnField, miFlagModeCenterFlagField, NULL, miFlagModeBackButton);
    mModeSettingsMenu[7].AddControl(miFlagModeCenterFlagField, miFlagModeHomeScoreField, miFlagModeBackButton, NULL, miFlagModeBackButton);

    mModeSettingsMenu[7].AddControl(miFlagModeBackButton, miFlagModeCenterFlagField, miFlagModeSpeedField, miFlagModeCenterFlagField, NULL);

    mModeSettingsMenu[7].AddNonControl(miFlagModeLeftHeaderBar);
    mModeSettingsMenu[7].AddNonControl(miFlagModeRightHeaderBar);
    mModeSettingsMenu[7].AddNonControl(miFlagModeHeaderText);

    mModeSettingsMenu[7].SetHeadControl(miFlagModeSpeedField);
    mModeSettingsMenu[7].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Chicken Mode Settings
    //***********************

    miChickenModeShowTargetField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "Show Target", 400, 180);
    miChickenModeShowTargetField->Add("Off", 0, "", false, false);
    miChickenModeShowTargetField->Add("On", 1, "", true, false);
    miChickenModeShowTargetField->SetData(NULL, NULL, &game_values.gamemodemenusettings.chicken.usetarget);
    miChickenModeShowTargetField->SetKey(game_values.gamemodemenusettings.chicken.usetarget ? 1 : 0);
    miChickenModeShowTargetField->SetAutoAdvance(true);

    miChickenModeGlideField= new MI_SelectField(&rm->spr_selectfield, 120, 240, "Chicken Glide", 400, 180);
    miChickenModeGlideField->Add("Off", 0, "", false, false);
    miChickenModeGlideField->Add("On", 1, "", true, false);
    miChickenModeGlideField->SetData(NULL, NULL, &game_values.gamemodemenusettings.chicken.glide);
    miChickenModeGlideField->SetKey(game_values.gamemodemenusettings.chicken.glide ? 1 : 0);
    miChickenModeGlideField->SetAutoAdvance(true);

    miChickenModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miChickenModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miChickenModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miChickenModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miChickenModeHeaderText = new MI_Text("Chicken Mode Menu", 320, 5, 0, 2, 1);


    mModeSettingsMenu[8].AddControl(miChickenModeShowTargetField, miChickenModeBackButton, miChickenModeGlideField, NULL, miChickenModeBackButton);
    mModeSettingsMenu[8].AddControl(miChickenModeGlideField, miChickenModeShowTargetField, miChickenModeBackButton, NULL, miChickenModeBackButton);
    mModeSettingsMenu[8].AddControl(miChickenModeBackButton, miChickenModeGlideField, miChickenModeShowTargetField, miChickenModeGlideField, NULL);

    mModeSettingsMenu[8].AddNonControl(miChickenModeLeftHeaderBar);
    mModeSettingsMenu[8].AddNonControl(miChickenModeRightHeaderBar);
    mModeSettingsMenu[8].AddNonControl(miChickenModeHeaderText);

    mModeSettingsMenu[8].SetHeadControl(miChickenModeShowTargetField);
    mModeSettingsMenu[8].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Tag Mode Settings
    //***********************

    miTagModeTagOnTouchField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Touch Tag", 400, 180);
    miTagModeTagOnTouchField->Add("Off", 0, "", false, false);
    miTagModeTagOnTouchField->Add("On", 1, "", true, false);
    miTagModeTagOnTouchField->SetData(NULL, NULL, &game_values.gamemodemenusettings.tag.tagontouch);
    miTagModeTagOnTouchField->SetKey(game_values.gamemodemenusettings.tag.tagontouch ? 1 : 0);
    miTagModeTagOnTouchField->SetAutoAdvance(true);

    miTagModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miTagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miTagModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTagModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTagModeHeaderText = new MI_Text("Tag Mode Menu", 320, 5, 0, 2, 1);


    mModeSettingsMenu[9].AddControl(miTagModeTagOnTouchField, miTagModeBackButton, miTagModeBackButton, NULL, miTagModeBackButton);
    mModeSettingsMenu[9].AddControl(miTagModeBackButton, miTagModeTagOnTouchField, miTagModeTagOnTouchField, miTagModeTagOnTouchField, NULL);

    mModeSettingsMenu[9].AddNonControl(miTagModeLeftHeaderBar);
    mModeSettingsMenu[9].AddNonControl(miTagModeRightHeaderBar);
    mModeSettingsMenu[9].AddNonControl(miTagModeHeaderText);

    mModeSettingsMenu[9].SetHeadControl(miTagModeTagOnTouchField);
    mModeSettingsMenu[9].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Star Mode Settings
    //***********************

    miStarModeTimeField = new MI_SelectField(&rm->spr_selectfield, 120, 180, "Time", 400, 150);
    miStarModeTimeField->Add("5 Seconds", 5, "", false, false, false);
    miStarModeTimeField->Add("10 Seconds", 10, "", false, false, false);
    miStarModeTimeField->Add("15 Seconds", 15, "", false, false);
    miStarModeTimeField->Add("20 Seconds", 20, "", false, false);
    miStarModeTimeField->Add("25 Seconds", 25, "", false, false);
    miStarModeTimeField->Add("30 Seconds", 30, "", false, false);
    miStarModeTimeField->Add("35 Seconds", 35, "", false, false);
    miStarModeTimeField->Add("40 Seconds", 40, "", false, false);
    miStarModeTimeField->Add("45 Seconds", 45, "", false, false, false);
    miStarModeTimeField->Add("50 Seconds", 50, "", false, false, false);
    miStarModeTimeField->Add("55 Seconds", 55, "", false, false, false);
    miStarModeTimeField->Add("60 Seconds", 60, "", false, false, false);
    miStarModeTimeField->SetData(&game_values.gamemodemenusettings.star.time, NULL, NULL);
    miStarModeTimeField->SetKey(game_values.gamemodemenusettings.star.time);

    miStarModeShineField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Star Type", 400, 150);
    miStarModeShineField->Add("Ztar", 0, "", false, false);
    miStarModeShineField->Add("Shine", 1, "", false, false);
    miStarModeShineField->Add("Multi Star", 2, "", false, false);
    miStarModeShineField->Add("Random", 3, "", false, false);
    miStarModeShineField->SetData(&game_values.gamemodemenusettings.star.shine, NULL, NULL);
    miStarModeShineField->SetKey(game_values.gamemodemenusettings.star.shine);

    miStarModePercentExtraTime = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Time", 400, 150, 384);
    miStarModePercentExtraTime->Add("0", 0, "", false, false, false);
    miStarModePercentExtraTime->Add("5", 5, "", false, false, false);
    miStarModePercentExtraTime->Add("10", 10, "", false, false);
    miStarModePercentExtraTime->Add("15", 15, "", false, false);
    miStarModePercentExtraTime->Add("20", 20, "", false, false);
    miStarModePercentExtraTime->Add("25", 25, "", false, false);
    miStarModePercentExtraTime->Add("30", 30, "", false, false);
    miStarModePercentExtraTime->Add("35", 35, "", false, false);
    miStarModePercentExtraTime->Add("40", 40, "", false, false);
    miStarModePercentExtraTime->Add("45", 45, "", false, false);
    miStarModePercentExtraTime->Add("50", 50, "", false, false);
    miStarModePercentExtraTime->Add("55", 55, "", false, false, false);
    miStarModePercentExtraTime->Add("60", 60, "", false, false, false);
    miStarModePercentExtraTime->Add("65", 65, "", false, false, false);
    miStarModePercentExtraTime->Add("70", 70, "", false, false, false);
    miStarModePercentExtraTime->Add("75", 75, "", false, false, false);
    miStarModePercentExtraTime->Add("80", 80, "", false, false, false);
    miStarModePercentExtraTime->Add("85", 85, "", false, false, false);
    miStarModePercentExtraTime->Add("90", 90, "", false, false, false);
    miStarModePercentExtraTime->Add("95", 95, "", false, false, false);
    miStarModePercentExtraTime->Add("100", 100, "", false, false, false);
    miStarModePercentExtraTime->SetData(&game_values.gamemodemenusettings.star.percentextratime, NULL, NULL);
    miStarModePercentExtraTime->SetKey(game_values.gamemodemenusettings.star.percentextratime);
    miStarModePercentExtraTime->SetNoWrap(true);

    miStarModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miStarModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miStarModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miStarModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miStarModeHeaderText = new MI_Text("Star Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[10].AddControl(miStarModeTimeField, miStarModeBackButton, miStarModeShineField, NULL, miStarModeBackButton);
    mModeSettingsMenu[10].AddControl(miStarModeShineField, miStarModeTimeField, miStarModePercentExtraTime, NULL, miStarModeBackButton);
    mModeSettingsMenu[10].AddControl(miStarModePercentExtraTime, miStarModeShineField, miStarModeBackButton, NULL, miStarModeBackButton);
    mModeSettingsMenu[10].AddControl(miStarModeBackButton, miStarModePercentExtraTime, miStarModeTimeField, miStarModePercentExtraTime, NULL);

    mModeSettingsMenu[10].AddNonControl(miStarModeLeftHeaderBar);
    mModeSettingsMenu[10].AddNonControl(miStarModeRightHeaderBar);
    mModeSettingsMenu[10].AddNonControl(miStarModeHeaderText);

    mModeSettingsMenu[10].SetHeadControl(miStarModeTimeField);
    mModeSettingsMenu[10].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Domination Mode Settings
    //***********************

    miDominationModeQuantityField = new MI_SelectField(&rm->spr_selectfield, 120, 120, "Quantity", 400, 180);
    miDominationModeQuantityField->Add("1 Base", 1, "", false, false, false);
    miDominationModeQuantityField->Add("2 Bases", 2, "", false, false, false);
    miDominationModeQuantityField->Add("3 Bases", 3, "", false, false);
    miDominationModeQuantityField->Add("4 Bases", 4, "", false, false);
    miDominationModeQuantityField->Add("5 Bases", 5, "", false, false);
    miDominationModeQuantityField->Add("6 Bases", 6, "", false, false, false);
    miDominationModeQuantityField->Add("7 Bases", 7, "", false, false, false);
    miDominationModeQuantityField->Add("8 Bases", 8, "", false, false, false);
    miDominationModeQuantityField->Add("9 Bases", 9, "", false, false, false);
    miDominationModeQuantityField->Add("10 Bases", 10, "", false, false, false);
    miDominationModeQuantityField->Add("# Players - 1", 11, "", false, false, false);
    miDominationModeQuantityField->Add("# Players", 12, "", false, false, false);
    miDominationModeQuantityField->Add("# Players + 1", 13, "", false, false);
    miDominationModeQuantityField->Add("# Players + 2", 14, "", false, false);
    miDominationModeQuantityField->Add("# Players + 3", 15, "", false, false);
    miDominationModeQuantityField->Add("# Players + 4", 16, "", false, false);
    miDominationModeQuantityField->Add("# Players + 5", 17, "", false, false);
    miDominationModeQuantityField->Add("# Players + 6", 18, "", false, false, false);
    miDominationModeQuantityField->Add("2x Players - 3", 19, "", false, false, false);
    miDominationModeQuantityField->Add("2x Players - 2", 20, "", false, false, false);
    miDominationModeQuantityField->Add("2x Players - 1", 21, "", false, false);
    miDominationModeQuantityField->Add("2x Players", 22, "", false, false);
    miDominationModeQuantityField->Add("2x Players + 1", 23, "", false, false, false);
    miDominationModeQuantityField->Add("2x Players + 2", 24, "", false, false, false);
    miDominationModeQuantityField->SetData(&game_values.gamemodemenusettings.domination.quantity, NULL, NULL);
    miDominationModeQuantityField->SetKey(game_values.gamemodemenusettings.domination.quantity);

    miDominationModeRelocateFrequencyField = new MI_SelectField(&rm->spr_selectfield, 120, 160, "Relocate", 400, 180);
    miDominationModeRelocateFrequencyField->Add("Never", 0, "", false, false);
    miDominationModeRelocateFrequencyField->Add("5 Seconds", 310, "", false, false, false);
    miDominationModeRelocateFrequencyField->Add("10 Seconds", 620, "", false, false);
    miDominationModeRelocateFrequencyField->Add("15 Seconds", 930, "", false, false);
    miDominationModeRelocateFrequencyField->Add("20 Seconds", 1240, "", false, false);
    miDominationModeRelocateFrequencyField->Add("30 Seconds", 1860, "", false, false);
    miDominationModeRelocateFrequencyField->Add("45 Seconds", 2790, "", false, false);
    miDominationModeRelocateFrequencyField->Add("1 Minute", 3720, "", false, false);
    miDominationModeRelocateFrequencyField->Add("1.5 Minutes", 5580, "", false, false);
    miDominationModeRelocateFrequencyField->Add("2 Minutes", 7440, "", false, false);
    miDominationModeRelocateFrequencyField->Add("2.5 Minutes", 9300, "", false, false);
    miDominationModeRelocateFrequencyField->Add("3 Minutes", 11160, "", false, false);
    miDominationModeRelocateFrequencyField->SetData(&game_values.gamemodemenusettings.domination.relocationfrequency, NULL, NULL);
    miDominationModeRelocateFrequencyField->SetKey(game_values.gamemodemenusettings.domination.relocationfrequency);

    miDominationModeDeathText = new MI_Text("On Death", 120, 210, 0, 2, 0);

    miDominationModeLoseOnDeathField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Lose Bases", 400, 180);
    miDominationModeLoseOnDeathField->Add("Off", 0, "", false, false);
    miDominationModeLoseOnDeathField->Add("On", 1, "", true, false);
    miDominationModeLoseOnDeathField->SetData(NULL, NULL, &game_values.gamemodemenusettings.domination.loseondeath);
    miDominationModeLoseOnDeathField->SetKey(game_values.gamemodemenusettings.domination.loseondeath ? 1 : 0);
    miDominationModeLoseOnDeathField->SetAutoAdvance(true);

    miDominationModeRelocateOnDeathField = new MI_SelectField(&rm->spr_selectfield, 120, 280, "Move Bases", 400, 180);
    miDominationModeRelocateOnDeathField->Add("Off", 0, "", false, false);
    miDominationModeRelocateOnDeathField->Add("On", 1, "", true, false);
    miDominationModeRelocateOnDeathField->SetData(NULL, NULL, &game_values.gamemodemenusettings.domination.relocateondeath);
    miDominationModeRelocateOnDeathField->SetKey(game_values.gamemodemenusettings.domination.relocateondeath ? 1 : 0);
    miDominationModeRelocateOnDeathField->SetAutoAdvance(true);

    miDominationModeStealOnDeathField = new MI_SelectField(&rm->spr_selectfield, 120, 320, "Steal Bases", 400, 180);
    miDominationModeStealOnDeathField->Add("Off", 0, "", false, false);
    miDominationModeStealOnDeathField->Add("On", 1, "", true, false);
    miDominationModeStealOnDeathField->SetData(NULL, NULL, &game_values.gamemodemenusettings.domination.stealondeath);
    miDominationModeStealOnDeathField->SetKey(game_values.gamemodemenusettings.domination.stealondeath ? 1 : 0);
    miDominationModeStealOnDeathField->SetAutoAdvance(true);


    miDominationModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miDominationModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miDominationModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miDominationModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miDominationModeHeaderText = new MI_Text("Domination Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[11].AddControl(miDominationModeQuantityField, miDominationModeBackButton, miDominationModeRelocateFrequencyField, NULL, miDominationModeBackButton);
    mModeSettingsMenu[11].AddControl(miDominationModeRelocateFrequencyField, miDominationModeQuantityField, miDominationModeLoseOnDeathField, NULL, miDominationModeBackButton);
    mModeSettingsMenu[11].AddControl(miDominationModeLoseOnDeathField, miDominationModeRelocateFrequencyField, miDominationModeRelocateOnDeathField, NULL, miDominationModeBackButton);
    mModeSettingsMenu[11].AddControl(miDominationModeRelocateOnDeathField, miDominationModeLoseOnDeathField, miDominationModeStealOnDeathField, NULL, miDominationModeBackButton);
    mModeSettingsMenu[11].AddControl(miDominationModeStealOnDeathField, miDominationModeRelocateOnDeathField, miDominationModeBackButton, NULL, miDominationModeBackButton);

    mModeSettingsMenu[11].AddControl(miDominationModeBackButton, miDominationModeStealOnDeathField, miDominationModeQuantityField, miDominationModeStealOnDeathField, NULL);

    mModeSettingsMenu[11].AddNonControl(miDominationModeLeftHeaderBar);
    mModeSettingsMenu[11].AddNonControl(miDominationModeRightHeaderBar);
    mModeSettingsMenu[11].AddNonControl(miDominationModeHeaderText);
    mModeSettingsMenu[11].AddNonControl(miDominationModeDeathText);

    mModeSettingsMenu[11].SetHeadControl(miDominationModeQuantityField);
    mModeSettingsMenu[11].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // King of the Hill Mode Settings
    //***********************

    miKingOfTheHillModeSizeField = new MI_SelectField(&rm->spr_selectfield, 120, 180, "Size", 400, 180);
    miKingOfTheHillModeSizeField->Add("2 x 2", 2, "", false, false);
    miKingOfTheHillModeSizeField->Add("3 x 3", 3, "", false, false);
    miKingOfTheHillModeSizeField->Add("4 x 4", 4, "", false, false);
    miKingOfTheHillModeSizeField->Add("5 x 5", 5, "", false, false);
    miKingOfTheHillModeSizeField->SetData(&game_values.gamemodemenusettings.kingofthehill.areasize, NULL, NULL);
    miKingOfTheHillModeSizeField->SetKey(game_values.gamemodemenusettings.kingofthehill.areasize);

    miKingOfTheHillModeRelocateFrequencyField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Relocate", 400, 180);
    miKingOfTheHillModeRelocateFrequencyField->Add("Never", 0, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("5 Seconds", 310, "", false, false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("10 Seconds", 620, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("15 Seconds", 930, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("20 Seconds", 1240, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("30 Seconds", 1860, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("45 Seconds", 2790, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("1 Minute", 3720, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("1.5 Minutes", 5580, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("2 Minutes", 7440, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("2.5 Minutes", 9300, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("3 Minutes", 11160, "", false, false);
    miKingOfTheHillModeRelocateFrequencyField->SetData(&game_values.gamemodemenusettings.kingofthehill.relocationfrequency, NULL, NULL);
    miKingOfTheHillModeRelocateFrequencyField->SetKey(game_values.gamemodemenusettings.kingofthehill.relocationfrequency);

    miKingOfTheHillModeMultiplierField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Max Multiplier", 400, 180);
    miKingOfTheHillModeMultiplierField->Add("None", 1, "", false, false);
    miKingOfTheHillModeMultiplierField->Add("2", 2, "", false, false);
    miKingOfTheHillModeMultiplierField->Add("3", 3, "", false, false);
    miKingOfTheHillModeMultiplierField->Add("4", 4, "", false, false);
    miKingOfTheHillModeMultiplierField->Add("5", 5, "", false, false);
    miKingOfTheHillModeMultiplierField->SetData(&game_values.gamemodemenusettings.kingofthehill.maxmultiplier, NULL, NULL);
    miKingOfTheHillModeMultiplierField->SetKey(game_values.gamemodemenusettings.kingofthehill.maxmultiplier);

    miKingOfTheHillModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miKingOfTheHillModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miKingOfTheHillModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miKingOfTheHillModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miKingOfTheHillModeHeaderText = new MI_Text("King of the Hill Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[12].AddControl(miKingOfTheHillModeSizeField, miKingOfTheHillModeBackButton, miKingOfTheHillModeRelocateFrequencyField, NULL, miKingOfTheHillModeBackButton);
    mModeSettingsMenu[12].AddControl(miKingOfTheHillModeRelocateFrequencyField, miKingOfTheHillModeSizeField, miKingOfTheHillModeMultiplierField, NULL, miKingOfTheHillModeBackButton);
    mModeSettingsMenu[12].AddControl(miKingOfTheHillModeMultiplierField, miKingOfTheHillModeRelocateFrequencyField, miKingOfTheHillModeBackButton, NULL, miKingOfTheHillModeBackButton);

    mModeSettingsMenu[12].AddControl(miKingOfTheHillModeBackButton, miKingOfTheHillModeMultiplierField, miKingOfTheHillModeSizeField, miKingOfTheHillModeMultiplierField, NULL);

    mModeSettingsMenu[12].AddNonControl(miKingOfTheHillModeLeftHeaderBar);
    mModeSettingsMenu[12].AddNonControl(miKingOfTheHillModeRightHeaderBar);
    mModeSettingsMenu[12].AddNonControl(miKingOfTheHillModeHeaderText);

    mModeSettingsMenu[12].SetHeadControl(miKingOfTheHillModeSizeField);
    mModeSettingsMenu[12].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Race Mode Settings
    //***********************

    miRaceModeQuantityField = new MI_SelectField(&rm->spr_selectfield, 120, 180, "Quantity", 400, 180);
    miRaceModeQuantityField->Add("2", 2, "", false, false, false);
    miRaceModeQuantityField->Add("3", 3, "", false, false);
    miRaceModeQuantityField->Add("4", 4, "", false, false);
    miRaceModeQuantityField->Add("5", 5, "", false, false);
    miRaceModeQuantityField->Add("6", 6, "", false, false);
    miRaceModeQuantityField->Add("7", 7, "", false, false);
    miRaceModeQuantityField->Add("8", MAXRACEGOALS, "", false, false);
    miRaceModeQuantityField->SetData(&game_values.gamemodemenusettings.race.quantity, NULL, NULL);
    miRaceModeQuantityField->SetKey(game_values.gamemodemenusettings.race.quantity);

    miRaceModeSpeedField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Speed", 400, 180);
    miRaceModeSpeedField->Add("Stationary", 0, "", false, false);
    miRaceModeSpeedField->Add("Very Slow", 2, "", false, false);
    miRaceModeSpeedField->Add("Slow", 3, "", false, false);
    miRaceModeSpeedField->Add("Moderate", 4, "", false, false);
    miRaceModeSpeedField->Add("Fast", 6, "", false, false);
    miRaceModeSpeedField->Add("Very Fast", 8, "", false, false, false);
    miRaceModeSpeedField->SetData(&game_values.gamemodemenusettings.race.speed, NULL, NULL);
    miRaceModeSpeedField->SetKey(game_values.gamemodemenusettings.race.speed);

    miRaceModePenaltyField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Penalty", 400, 180);
    miRaceModePenaltyField->Add("None", 0, "", false, false);
    miRaceModePenaltyField->Add("One Goal", 1, "", false, false);
    miRaceModePenaltyField->Add("All Goals", 2, "", false, false);
    miRaceModePenaltyField->SetData(&game_values.gamemodemenusettings.race.penalty, NULL, NULL);
    miRaceModePenaltyField->SetKey(game_values.gamemodemenusettings.race.penalty);

    miRaceModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miRaceModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miRaceModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miRaceModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miRaceModeHeaderText = new MI_Text("Race Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[13].AddControl(miRaceModeQuantityField, miRaceModeBackButton, miRaceModeSpeedField, NULL, miRaceModeBackButton);
    mModeSettingsMenu[13].AddControl(miRaceModeSpeedField, miRaceModeQuantityField, miRaceModePenaltyField, NULL, miRaceModeBackButton);
    mModeSettingsMenu[13].AddControl(miRaceModePenaltyField, miRaceModeSpeedField, miRaceModeBackButton, NULL, miRaceModeBackButton);

    mModeSettingsMenu[13].AddControl(miRaceModeBackButton, miRaceModePenaltyField, miRaceModeQuantityField, miRaceModePenaltyField, NULL);

    mModeSettingsMenu[13].AddNonControl(miRaceModeLeftHeaderBar);
    mModeSettingsMenu[13].AddNonControl(miRaceModeRightHeaderBar);
    mModeSettingsMenu[13].AddNonControl(miRaceModeHeaderText);

    mModeSettingsMenu[13].SetHeadControl(miRaceModeQuantityField);
    mModeSettingsMenu[13].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Frenzy Mode Settings
    //***********************

    miFrenzyModeOptions = new MI_FrenzyModeOptions(50, 44, 640, 7);
    miFrenzyModeOptions->SetAutoModify(true);

    miFrenzyModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miFrenzyModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miFrenzyModeHeaderText = new MI_Text("Frenzy Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[15].AddControl(miFrenzyModeOptions, NULL, NULL, NULL, NULL);

    mModeSettingsMenu[15].AddNonControl(miFrenzyModeLeftHeaderBar);
    mModeSettingsMenu[15].AddNonControl(miFrenzyModeRightHeaderBar);
    mModeSettingsMenu[15].AddNonControl(miFrenzyModeHeaderText);

    mModeSettingsMenu[15].SetHeadControl(miFrenzyModeOptions);
    mModeSettingsMenu[15].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Survival Mode Settings
    //***********************

    for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++) {
        miSurvivalModeEnemySlider[iEnemy] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->menu_survival, 120, 120 + 40 * iEnemy, 400, iEnemy);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 0, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 1, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 2, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 3, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 4, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 5, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 6, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 7, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 8, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 9, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->Add("", 10, "", false, false);
        miSurvivalModeEnemySlider[iEnemy]->SetNoWrap(true);
        miSurvivalModeEnemySlider[iEnemy]->SetData(&game_values.gamemodemenusettings.survival.enemyweight[iEnemy], NULL, NULL);
        miSurvivalModeEnemySlider[iEnemy]->SetKey(game_values.gamemodemenusettings.survival.enemyweight[iEnemy]);
    }

    miSurvivalModeDensityField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Density", 400, 180);
    miSurvivalModeDensityField->Add("Very Low", 40, "", false, false, false);
    miSurvivalModeDensityField->Add("Low", 30, "", false, false);
    miSurvivalModeDensityField->Add("Medium", 20, "", false, false);
    miSurvivalModeDensityField->Add("High", 15, "", false, false);
    miSurvivalModeDensityField->Add("Very High", 10, "", false, false, false);
    miSurvivalModeDensityField->SetData(&game_values.gamemodemenusettings.survival.density, NULL, NULL);
    miSurvivalModeDensityField->SetKey(game_values.gamemodemenusettings.survival.density);

    miSurvivalModeSpeedField = new MI_SelectField(&rm->spr_selectfield, 120, 280, "Speed", 400, 180);
    miSurvivalModeSpeedField->Add("Very Slow", 2, "", false, false, false);
    miSurvivalModeSpeedField->Add("Slow", 3, "", false, false);
    miSurvivalModeSpeedField->Add("Moderate", 4, "", false, false);
    miSurvivalModeSpeedField->Add("Fast", 6, "", false, false);
    miSurvivalModeSpeedField->Add("Very Fast", 8, "", false, false, false);
    miSurvivalModeSpeedField->SetData(&game_values.gamemodemenusettings.survival.speed, NULL, NULL);
    miSurvivalModeSpeedField->SetKey(game_values.gamemodemenusettings.survival.speed);

    miSurvivalModeShieldField = new MI_SelectField(&rm->spr_selectfield, 120, 320, "Shield", 400, 180);
    miSurvivalModeShieldField->Add("Off", 0, "", false, false);
    miSurvivalModeShieldField->Add("On", 1, "", true, false);
    miSurvivalModeShieldField->SetData(NULL, NULL, &game_values.gamemodemenusettings.survival.shield);
    miSurvivalModeShieldField->SetKey(game_values.gamemodemenusettings.survival.shield ? 1 : 0);
    miSurvivalModeShieldField->SetAutoAdvance(true);

    miSurvivalModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miSurvivalModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miSurvivalModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miSurvivalModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miSurvivalModeHeaderText = new MI_Text("Survival Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[16].AddControl(miSurvivalModeEnemySlider[0], miSurvivalModeBackButton, miSurvivalModeEnemySlider[1], NULL, miSurvivalModeBackButton);
    mModeSettingsMenu[16].AddControl(miSurvivalModeEnemySlider[1], miSurvivalModeEnemySlider[0], miSurvivalModeEnemySlider[2], NULL, miSurvivalModeBackButton);
    mModeSettingsMenu[16].AddControl(miSurvivalModeEnemySlider[2], miSurvivalModeEnemySlider[1], miSurvivalModeDensityField, NULL, miSurvivalModeBackButton);
    mModeSettingsMenu[16].AddControl(miSurvivalModeDensityField, miSurvivalModeEnemySlider[2], miSurvivalModeSpeedField, NULL, miSurvivalModeBackButton);
    mModeSettingsMenu[16].AddControl(miSurvivalModeSpeedField, miSurvivalModeDensityField, miSurvivalModeShieldField, NULL, miSurvivalModeBackButton);
    mModeSettingsMenu[16].AddControl(miSurvivalModeShieldField, miSurvivalModeSpeedField, miSurvivalModeBackButton, NULL, miSurvivalModeBackButton);
    mModeSettingsMenu[16].AddControl(miSurvivalModeBackButton, miSurvivalModeShieldField, miSurvivalModeEnemySlider[0], miSurvivalModeShieldField, NULL);

    mModeSettingsMenu[16].AddNonControl(miSurvivalModeLeftHeaderBar);
    mModeSettingsMenu[16].AddNonControl(miSurvivalModeRightHeaderBar);
    mModeSettingsMenu[16].AddNonControl(miSurvivalModeHeaderText);

    mModeSettingsMenu[16].SetHeadControl(miSurvivalModeEnemySlider[0]);
    mModeSettingsMenu[16].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Greed Mode Settings
    //***********************
    miGreedModeCoinLife = new MI_SelectField(&rm->spr_selectfield, 120, 160, "Coin Life", 400, 150);
    miGreedModeCoinLife->Add("1 Second", 62, "", false, false, false);
    miGreedModeCoinLife->Add("2 Seconds", 124, "", false, false, false);
    miGreedModeCoinLife->Add("3 Seconds", 186, "", false, false);
    miGreedModeCoinLife->Add("4 Seconds", 248, "", false, false);
    miGreedModeCoinLife->Add("5 Seconds", 310, "", false, false);
    miGreedModeCoinLife->Add("6 Seconds", 372, "", false, false);
    miGreedModeCoinLife->Add("7 Seconds", 434, "", false, false);
    miGreedModeCoinLife->Add("8 Seconds", 496, "", false, false);
    miGreedModeCoinLife->Add("9 Seconds", 558, "", false, false);
    miGreedModeCoinLife->Add("10 Seconds", 620, "", false, false);
    miGreedModeCoinLife->Add("12 Seconds", 744, "", false, false);
    miGreedModeCoinLife->Add("15 Seconds", 930, "", false, false);
    miGreedModeCoinLife->Add("18 Seconds", 1116, "", false, false);
    miGreedModeCoinLife->Add("20 Seconds", 1240, "", false, false);
    miGreedModeCoinLife->Add("25 Seconds", 1550, "", false, false);
    miGreedModeCoinLife->Add("30 Seconds", 1860, "", false, false);
    miGreedModeCoinLife->SetData(&game_values.gamemodemenusettings.greed.coinlife, NULL, NULL);
    miGreedModeCoinLife->SetKey(game_values.gamemodemenusettings.greed.coinlife);

    miGreedModeOwnCoins = new MI_SelectField(&rm->spr_selectfield, 120, 200, "Own Coins", 400, 150);
    miGreedModeOwnCoins->Add("Yes", 1, "", true, false);
    miGreedModeOwnCoins->Add("No", 0, "", false, false);
    miGreedModeOwnCoins->SetData(NULL, NULL, &game_values.gamemodemenusettings.greed.owncoins);
    miGreedModeOwnCoins->SetKey(game_values.gamemodemenusettings.greed.owncoins);
    miGreedModeOwnCoins->SetAutoAdvance(true);

    miGreedModeMultiplier = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Multipler", 400, 150);
    miGreedModeMultiplier->Add("0.5", 1, "", false, false, false);
    miGreedModeMultiplier->Add("1", 2, "", false, false);
    miGreedModeMultiplier->Add("1.5", 3, "", false, false);
    miGreedModeMultiplier->Add("2", 4, "", false, false);
    miGreedModeMultiplier->Add("2.5", 5, "", false, false, false);
    miGreedModeMultiplier->Add("3", 6, "", false, false, false);
    miGreedModeMultiplier->SetData(&game_values.gamemodemenusettings.greed.multiplier, NULL, NULL);
    miGreedModeMultiplier->SetKey(game_values.gamemodemenusettings.greed.multiplier);

    miGreedModePercentExtraCoin = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 280, "Extra Coins", 400, 150, 384);
    miGreedModePercentExtraCoin->Add("0", 0, "", false, false, false);
    miGreedModePercentExtraCoin->Add("5", 5, "", false, false, false);
    miGreedModePercentExtraCoin->Add("10", 10, "", false, false);
    miGreedModePercentExtraCoin->Add("15", 15, "", false, false);
    miGreedModePercentExtraCoin->Add("20", 20, "", false, false);
    miGreedModePercentExtraCoin->Add("25", 25, "", false, false);
    miGreedModePercentExtraCoin->Add("30", 30, "", false, false);
    miGreedModePercentExtraCoin->Add("35", 35, "", false, false);
    miGreedModePercentExtraCoin->Add("40", 40, "", false, false);
    miGreedModePercentExtraCoin->Add("45", 45, "", false, false);
    miGreedModePercentExtraCoin->Add("50", 50, "", false, false);
    miGreedModePercentExtraCoin->Add("55", 55, "", false, false, false);
    miGreedModePercentExtraCoin->Add("60", 60, "", false, false, false);
    miGreedModePercentExtraCoin->Add("65", 65, "", false, false, false);
    miGreedModePercentExtraCoin->Add("70", 70, "", false, false, false);
    miGreedModePercentExtraCoin->Add("75", 75, "", false, false, false);
    miGreedModePercentExtraCoin->Add("80", 80, "", false, false, false);
    miGreedModePercentExtraCoin->Add("85", 85, "", false, false, false);
    miGreedModePercentExtraCoin->Add("90", 90, "", false, false, false);
    miGreedModePercentExtraCoin->Add("95", 95, "", false, false, false);
    miGreedModePercentExtraCoin->Add("100", 100, "", false, false, false);
    miGreedModePercentExtraCoin->SetData(&game_values.gamemodemenusettings.greed.percentextracoin, NULL, NULL);
    miGreedModePercentExtraCoin->SetKey(game_values.gamemodemenusettings.greed.percentextracoin);
    miGreedModePercentExtraCoin->SetNoWrap(true);

    miGreedModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miGreedModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miGreedModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miGreedModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miGreedModeHeaderText = new MI_Text("Greed Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[17].AddControl(miGreedModeCoinLife, miGreedModeBackButton, miGreedModeOwnCoins, NULL, miGreedModeBackButton);
    mModeSettingsMenu[17].AddControl(miGreedModeOwnCoins, miGreedModeCoinLife, miGreedModeMultiplier, NULL, miGreedModeBackButton);
    mModeSettingsMenu[17].AddControl(miGreedModeMultiplier, miGreedModeOwnCoins, miGreedModePercentExtraCoin, NULL, miGreedModeBackButton);
    mModeSettingsMenu[17].AddControl(miGreedModePercentExtraCoin, miGreedModeMultiplier, miGreedModeBackButton, NULL, miGreedModeBackButton);

    mModeSettingsMenu[17].AddControl(miGreedModeBackButton, miGreedModePercentExtraCoin, miGreedModeCoinLife, miGreedModePercentExtraCoin, NULL);

    mModeSettingsMenu[17].AddNonControl(miGreedModeLeftHeaderBar);
    mModeSettingsMenu[17].AddNonControl(miGreedModeRightHeaderBar);
    mModeSettingsMenu[17].AddNonControl(miGreedModeHeaderText);

    mModeSettingsMenu[17].SetHeadControl(miGreedModeCoinLife);
    mModeSettingsMenu[17].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Health Mode Settings
    //***********************
    miHealthModeStartLife = new MI_SelectField(&rm->spr_selectfield, 120, 180, "Start Life", 400, 150);
    miHealthModeStartLife->Add("2", 2, "", false, false, false);
    miHealthModeStartLife->Add("3", 3, "", false, false, false);
    miHealthModeStartLife->Add("4", 4, "", false, false);
    miHealthModeStartLife->Add("5", 5, "", false, false);
    miHealthModeStartLife->Add("6", 6, "", false, false);
    miHealthModeStartLife->Add("7", 7, "", false, false, false);
    miHealthModeStartLife->Add("8", 8, "", false, false, false);
    miHealthModeStartLife->Add("9", 9, "", false, false, false);
    miHealthModeStartLife->Add("10", 10, "", false, false, false);
    miHealthModeStartLife->SetData(&game_values.gamemodemenusettings.health.startlife, NULL, NULL);
    miHealthModeStartLife->SetKey(game_values.gamemodemenusettings.health.startlife);
    miHealthModeStartLife->SetNoWrap(true);
    miHealthModeStartLife->SetItemChangedCode(MENU_CODE_HEALTH_MODE_START_LIFE_CHANGED);

    miHealthModeMaxLife = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Max Life", 400, 150);
    miHealthModeMaxLife->Add("2", 2, "", false, false, false);
    miHealthModeMaxLife->Add("3", 3, "", false, false, false);
    miHealthModeMaxLife->Add("4", 4, "", false, false, false);
    miHealthModeMaxLife->Add("5", 5, "", false, false, false);
    miHealthModeMaxLife->Add("6", 6, "", false, false);
    miHealthModeMaxLife->Add("7", 7, "", false, false);
    miHealthModeMaxLife->Add("8", 8, "", false, false);
    miHealthModeMaxLife->Add("9", 9, "", false, false);
    miHealthModeMaxLife->Add("10", 10, "", false, false);
    miHealthModeMaxLife->SetData(&game_values.gamemodemenusettings.health.maxlife, NULL, NULL);
    miHealthModeMaxLife->SetKey(game_values.gamemodemenusettings.health.maxlife);
    miHealthModeMaxLife->SetNoWrap(true);
    miHealthModeMaxLife->SetItemChangedCode(MENU_CODE_HEALTH_MODE_MAX_LIFE_CHANGED);

    miHealthModePercentExtraLife = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Life", 400, 150, 384);
    miHealthModePercentExtraLife->Add("0", 0, "", false, false, false);
    miHealthModePercentExtraLife->Add("5", 5, "", false, false, false);
    miHealthModePercentExtraLife->Add("10", 10, "", false, false);
    miHealthModePercentExtraLife->Add("15", 15, "", false, false);
    miHealthModePercentExtraLife->Add("20", 20, "", false, false);
    miHealthModePercentExtraLife->Add("25", 25, "", false, false);
    miHealthModePercentExtraLife->Add("30", 30, "", false, false);
    miHealthModePercentExtraLife->Add("35", 35, "", false, false);
    miHealthModePercentExtraLife->Add("40", 40, "", false, false);
    miHealthModePercentExtraLife->Add("45", 45, "", false, false);
    miHealthModePercentExtraLife->Add("50", 50, "", false, false);
    miHealthModePercentExtraLife->Add("55", 55, "", false, false, false);
    miHealthModePercentExtraLife->Add("60", 60, "", false, false, false);
    miHealthModePercentExtraLife->Add("65", 65, "", false, false, false);
    miHealthModePercentExtraLife->Add("70", 70, "", false, false, false);
    miHealthModePercentExtraLife->Add("75", 75, "", false, false, false);
    miHealthModePercentExtraLife->Add("80", 80, "", false, false, false);
    miHealthModePercentExtraLife->Add("85", 85, "", false, false, false);
    miHealthModePercentExtraLife->Add("90", 90, "", false, false, false);
    miHealthModePercentExtraLife->Add("95", 95, "", false, false, false);
    miHealthModePercentExtraLife->Add("100", 100, "", false, false, false);
    miHealthModePercentExtraLife->SetData(&game_values.gamemodemenusettings.health.percentextralife, NULL, NULL);
    miHealthModePercentExtraLife->SetKey(game_values.gamemodemenusettings.health.percentextralife);
    miHealthModePercentExtraLife->SetNoWrap(true);

    miHealthModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miHealthModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miHealthModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miHealthModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miHealthModeHeaderText = new MI_Text("Health Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[18].AddControl(miHealthModeStartLife, miHealthModeBackButton, miHealthModeMaxLife, NULL, miHealthModeBackButton);
    mModeSettingsMenu[18].AddControl(miHealthModeMaxLife, miHealthModeStartLife, miHealthModePercentExtraLife, NULL, miHealthModeBackButton);
    mModeSettingsMenu[18].AddControl(miHealthModePercentExtraLife, miHealthModeMaxLife, miHealthModeBackButton, NULL, miHealthModeBackButton);
    mModeSettingsMenu[18].AddControl(miHealthModeBackButton, miHealthModePercentExtraLife, miHealthModeStartLife, miHealthModePercentExtraLife, NULL);

    mModeSettingsMenu[18].AddNonControl(miHealthModeLeftHeaderBar);
    mModeSettingsMenu[18].AddNonControl(miHealthModeRightHeaderBar);
    mModeSettingsMenu[18].AddNonControl(miHealthModeHeaderText);

    mModeSettingsMenu[18].SetHeadControl(miHealthModeStartLife);
    mModeSettingsMenu[18].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Card Collection Mode Settings
    //***********************

    miCollectionModeQuantityField = new MI_SelectField(&rm->spr_selectfield, 120, 160, "Limit", 400, 180);
    miCollectionModeQuantityField->Add("1 Card", 1, "", false, false);
    miCollectionModeQuantityField->Add("2 Cards", 2, "", false, false);
    miCollectionModeQuantityField->Add("3 Cards", 3, "", false, false);
    miCollectionModeQuantityField->Add("4 Cards", 4, "", false, false, false);
    miCollectionModeQuantityField->Add("5 Cards", 5, "", false, false, false);
    miCollectionModeQuantityField->Add("# Players - 1", 6, "", false, false);
    miCollectionModeQuantityField->Add("# Players", 7, "", false, false);
    miCollectionModeQuantityField->Add("# Players + 1", 8, "", false, false, false);
    miCollectionModeQuantityField->Add("# Players + 2", 9, "", false, false, false);
    miCollectionModeQuantityField->Add("# Players + 3", 10, "", false, false, false);
    miCollectionModeQuantityField->SetData(&game_values.gamemodemenusettings.collection.quantity, NULL, NULL);
    miCollectionModeQuantityField->SetKey(game_values.gamemodemenusettings.collection.quantity);

    miCollectionModeRateField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "Rate", 400, 180);
    miCollectionModeRateField->Add("Instant", 0, "", false, false);
    miCollectionModeRateField->Add("1 Second", 62, "", false, false);
    miCollectionModeRateField->Add("2 Seconds", 124, "", false, false);
    miCollectionModeRateField->Add("3 Seconds", 186, "", false, false);
    miCollectionModeRateField->Add("5 Seconds", 310, "", false, false);
    miCollectionModeRateField->Add("10 Seconds", 620, "", false, false, false);
    miCollectionModeRateField->Add("15 Seconds", 930, "", false, false, false);
    miCollectionModeRateField->Add("20 Seconds", 1240, "", false, false, false);
    miCollectionModeRateField->Add("25 Seconds", 1550, "", false, false, false);
    miCollectionModeRateField->Add("30 Seconds", 1860, "", false, false, false);
    miCollectionModeRateField->SetData(&game_values.gamemodemenusettings.collection.rate, NULL, NULL);
    miCollectionModeRateField->SetKey(game_values.gamemodemenusettings.collection.rate);

    miCollectionModeBankTimeField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Bank Time", 400, 180);
    miCollectionModeBankTimeField->Add("Instant", 0, "", false, false, false);
    miCollectionModeBankTimeField->Add("1 Second", 62, "", false, false, false);
    miCollectionModeBankTimeField->Add("2 Seconds", 124, "", false, false);
    miCollectionModeBankTimeField->Add("3 Seconds", 186, "", false, false);
    miCollectionModeBankTimeField->Add("4 Seconds", 248, "", false, false);
    miCollectionModeBankTimeField->Add("5 Seconds", 310, "", false, false);
    miCollectionModeBankTimeField->Add("6 Seconds", 372, "", false, false, false);
    miCollectionModeBankTimeField->Add("7 Seconds", 434, "", false, false, false);
    miCollectionModeBankTimeField->Add("8 Seconds", 496, "", false, false, false);
    miCollectionModeBankTimeField->Add("9 Seconds", 558, "", false, false, false);
    miCollectionModeBankTimeField->Add("10 Seconds", 620, "", false, false, false);
    miCollectionModeBankTimeField->SetData(&game_values.gamemodemenusettings.collection.banktime, NULL, NULL);
    miCollectionModeBankTimeField->SetKey(game_values.gamemodemenusettings.collection.banktime);

    miCollectionModeCardLifeField = new MI_SelectField(&rm->spr_selectfield, 120, 280, "Card Life", 400, 180);
    miCollectionModeCardLifeField->Add("1 Second", 62, "", false, false, false);
    miCollectionModeCardLifeField->Add("2 Seconds", 124, "", false, false, false);
    miCollectionModeCardLifeField->Add("3 Seconds", 186, "", false, false);
    miCollectionModeCardLifeField->Add("4 Seconds", 248, "", false, false);
    miCollectionModeCardLifeField->Add("5 Seconds", 310, "", false, false);
    miCollectionModeCardLifeField->Add("6 Seconds", 372, "", false, false);
    miCollectionModeCardLifeField->Add("7 Seconds", 434, "", false, false);
    miCollectionModeCardLifeField->Add("8 Seconds", 496, "", false, false);
    miCollectionModeCardLifeField->Add("9 Seconds", 558, "", false, false);
    miCollectionModeCardLifeField->Add("10 Seconds", 620, "", false, false);
    miCollectionModeCardLifeField->Add("12 Seconds", 744, "", false, false);
    miCollectionModeCardLifeField->Add("15 Seconds", 930, "", false, false);
    miCollectionModeCardLifeField->Add("18 Seconds", 1116, "", false, false);
    miCollectionModeCardLifeField->Add("20 Seconds", 1240, "", false, false);
    miCollectionModeCardLifeField->Add("25 Seconds", 1550, "", false, false);
    miCollectionModeCardLifeField->Add("30 Seconds", 1860, "", false, false);
    miCollectionModeCardLifeField->SetData(&game_values.gamemodemenusettings.collection.cardlife, NULL, NULL);
    miCollectionModeCardLifeField->SetKey(game_values.gamemodemenusettings.collection.cardlife);

    miCollectionModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miCollectionModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miCollectionModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miCollectionModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miCollectionModeHeaderText = new MI_Text("Card Collection Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[19].AddControl(miCollectionModeQuantityField, miCollectionModeBackButton, miCollectionModeRateField, NULL, miCollectionModeBackButton);
    mModeSettingsMenu[19].AddControl(miCollectionModeRateField, miCollectionModeQuantityField, miCollectionModeBankTimeField, NULL, miCollectionModeBackButton);
    mModeSettingsMenu[19].AddControl(miCollectionModeBankTimeField, miCollectionModeRateField, miCollectionModeCardLifeField, NULL, miCollectionModeBackButton);
    mModeSettingsMenu[19].AddControl(miCollectionModeCardLifeField, miCollectionModeBankTimeField, miCollectionModeBackButton, NULL, miCollectionModeBackButton);
    mModeSettingsMenu[19].AddControl(miCollectionModeBackButton, miCollectionModeCardLifeField, miCollectionModeQuantityField, miCollectionModeCardLifeField, NULL);

    mModeSettingsMenu[19].AddNonControl(miCollectionModeLeftHeaderBar);
    mModeSettingsMenu[19].AddNonControl(miCollectionModeRightHeaderBar);
    mModeSettingsMenu[19].AddNonControl(miCollectionModeHeaderText);

    mModeSettingsMenu[19].SetHeadControl(miCollectionModeQuantityField);
    mModeSettingsMenu[19].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    //***********************
    // Chase Mode Settings
    //***********************

    miChaseModeSpeedField = new MI_SelectField(&rm->spr_selectfield, 120, 160, "Speed", 400, 180);
    miChaseModeSpeedField->Add("Very Slow", 3, "", false, false, false);
    miChaseModeSpeedField->Add("Slow", 4, "", false, false);
    miChaseModeSpeedField->Add("Moderate", 5, "", false, false);
    miChaseModeSpeedField->Add("Fast", 6, "", false, false);
    miChaseModeSpeedField->Add("Very Fast", 7, "", false, false);
    miChaseModeSpeedField->Add("Extremely Fast", 8, "", false, false, false);
    miChaseModeSpeedField->Add("Insanely Fast", 10, "", false, false, false);
    miChaseModeSpeedField->SetData(&game_values.gamemodemenusettings.chase.phantospeed, NULL, NULL);
    miChaseModeSpeedField->SetKey(game_values.gamemodemenusettings.chase.phantospeed);

    for (short iPhanto = 0; iPhanto < 3; iPhanto++) {
        miChaseModeQuantitySlider[iPhanto] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->spr_phanto, 120, 200 + 40 * iPhanto, 400, iPhanto);
        miChaseModeQuantitySlider[iPhanto]->Add("", 0, "", false, false, iPhanto == 0 ? false : true);
        miChaseModeQuantitySlider[iPhanto]->Add("", 1, "", false, false);
        miChaseModeQuantitySlider[iPhanto]->Add("", 2, "", false, false);
        miChaseModeQuantitySlider[iPhanto]->Add("", 3, "", false, false, false);
        miChaseModeQuantitySlider[iPhanto]->Add("", 4, "", false, false, false);
        miChaseModeQuantitySlider[iPhanto]->Add("", 5, "", false, false, false);
        miChaseModeQuantitySlider[iPhanto]->SetNoWrap(true);
        miChaseModeQuantitySlider[iPhanto]->SetData(&game_values.gamemodemenusettings.chase.phantoquantity[iPhanto], NULL, NULL);
        miChaseModeQuantitySlider[iPhanto]->SetKey(game_values.gamemodemenusettings.chase.phantoquantity[iPhanto]);
    }

    miChaseModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miChaseModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miChaseModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miChaseModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miChaseModeHeaderText = new MI_Text("Phanto Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[20].AddControl(miChaseModeSpeedField, miChaseModeBackButton, miChaseModeQuantitySlider[0], NULL, miChaseModeBackButton);

    mModeSettingsMenu[20].AddControl(miChaseModeQuantitySlider[0], miChaseModeSpeedField, miChaseModeQuantitySlider[1], NULL, miChaseModeBackButton);
    mModeSettingsMenu[20].AddControl(miChaseModeQuantitySlider[1], miChaseModeQuantitySlider[0], miChaseModeQuantitySlider[2], NULL, miChaseModeBackButton);
    mModeSettingsMenu[20].AddControl(miChaseModeQuantitySlider[2], miChaseModeQuantitySlider[1], miChaseModeBackButton, NULL, miChaseModeBackButton);

    mModeSettingsMenu[20].AddControl(miChaseModeBackButton, miChaseModeQuantitySlider[2], miChaseModeSpeedField, miChaseModeQuantitySlider[2], NULL);

    mModeSettingsMenu[20].AddNonControl(miChaseModeLeftHeaderBar);
    mModeSettingsMenu[20].AddNonControl(miChaseModeRightHeaderBar);
    mModeSettingsMenu[20].AddNonControl(miChaseModeHeaderText);

    mModeSettingsMenu[20].SetHeadControl(miChaseModeSpeedField);
    mModeSettingsMenu[20].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Shyguy Tag Mode Settings
    //***********************

    miShyGuyTagModeTagOnSuicideField = new MI_SelectField(&rm->spr_selectfield, 120, 180, "Suicide Tag", 400, 180);
    miShyGuyTagModeTagOnSuicideField->Add("Off", 0, "", false, false);
    miShyGuyTagModeTagOnSuicideField->Add("On", 1, "", true, false);
    miShyGuyTagModeTagOnSuicideField->SetData(NULL, NULL, &game_values.gamemodemenusettings.shyguytag.tagonsuicide);
    miShyGuyTagModeTagOnSuicideField->SetKey(game_values.gamemodemenusettings.shyguytag.tagonsuicide ? 1 : 0);
    miShyGuyTagModeTagOnSuicideField->SetAutoAdvance(true);

    miShyGuyTagModeTagOnStompField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Tag Transfer", 400, 180);
    miShyGuyTagModeTagOnStompField->Add("Touch Only", 0, "", false, false);
    miShyGuyTagModeTagOnStompField->Add("Kills Only", 1, "", false, false);
    miShyGuyTagModeTagOnStompField->Add("Touch and Kills", 2, "", false, false);
    miShyGuyTagModeTagOnStompField->SetData(&game_values.gamemodemenusettings.shyguytag.tagtransfer, NULL, NULL);
    miShyGuyTagModeTagOnStompField->SetKey(game_values.gamemodemenusettings.shyguytag.tagtransfer);

    miShyGuyTagModeFreeTimeField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Free Time", 400, 180);
    miShyGuyTagModeFreeTimeField->Add("Instant", 0, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("1 Second", 1, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("2 Seconds", 2, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("3 Seconds", 3, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("4 Seconds", 4, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("5 Seconds", 5, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("6 Seconds", 6, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("7 Seconds", 7, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("8 Seconds", 8, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("9 Seconds", 9, "", false, false);
    miShyGuyTagModeFreeTimeField->Add("10 Seconds", 10, "", false, false);
    miShyGuyTagModeFreeTimeField->SetData(&game_values.gamemodemenusettings.shyguytag.freetime, NULL, NULL);
    miShyGuyTagModeFreeTimeField->SetKey(game_values.gamemodemenusettings.shyguytag.freetime);

    miShyGuyTagModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miShyGuyTagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miShyGuyTagModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miShyGuyTagModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miShyGuyTagModeHeaderText = new MI_Text("Shyguy Tag Mode Menu", 320, 5, 0, 2, 1);

    mModeSettingsMenu[21].AddControl(miShyGuyTagModeTagOnSuicideField, miShyGuyTagModeBackButton, miShyGuyTagModeTagOnStompField, NULL, miShyGuyTagModeBackButton);
    mModeSettingsMenu[21].AddControl(miShyGuyTagModeTagOnStompField, miShyGuyTagModeTagOnSuicideField, miShyGuyTagModeFreeTimeField, NULL, miShyGuyTagModeBackButton);
    mModeSettingsMenu[21].AddControl(miShyGuyTagModeFreeTimeField, miShyGuyTagModeTagOnStompField, miShyGuyTagModeBackButton, NULL, miShyGuyTagModeBackButton);
    mModeSettingsMenu[21].AddControl(miShyGuyTagModeBackButton, miShyGuyTagModeFreeTimeField, miShyGuyTagModeTagOnSuicideField, miShyGuyTagModeFreeTimeField, NULL);

    mModeSettingsMenu[21].AddNonControl(miShyGuyTagModeLeftHeaderBar);
    mModeSettingsMenu[21].AddNonControl(miShyGuyTagModeRightHeaderBar);
    mModeSettingsMenu[21].AddNonControl(miShyGuyTagModeHeaderText);

    mModeSettingsMenu[21].SetHeadControl(miShyGuyTagModeTagOnSuicideField);
    mModeSettingsMenu[21].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


    //***********************
    // Boss Mode Settings
    //***********************

    miBossModeTypeField = new MI_SelectField(&rm->spr_selectfield, 120, 180, "Type", 400, 180);
    miBossModeTypeField->Add("Hammer", 0, "", false, false);
    miBossModeTypeField->Add("Bomb", 1, "", false, false);
    miBossModeTypeField->Add("Fire", 2, "", false, false);
    miBossModeTypeField->SetData(&game_values.gamemodemenusettings.boss.bosstype, NULL, NULL);
    miBossModeTypeField->SetKey(game_values.gamemodemenusettings.boss.bosstype);

    miBossModeDifficultyField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Difficulty", 400, 180);
    miBossModeDifficultyField->Add("Very Easy", 0, "", false, false, false);
    miBossModeDifficultyField->Add("Easy", 1, "", false, false);
    miBossModeDifficultyField->Add("Moderate", 2, "", false, false);
    miBossModeDifficultyField->Add("Hard", 3, "", false, false);
    miBossModeDifficultyField->Add("Very Hard", 4, "", false, false, false);
    miBossModeDifficultyField->SetData(&game_values.gamemodemenusettings.boss.difficulty, NULL, NULL);
    miBossModeDifficultyField->SetKey(game_values.gamemodemenusettings.boss.difficulty);

    miBossModeHitPointsField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Health", 400, 180);
    miBossModeHitPointsField->Add("1", 1, "", false, false, false);
    miBossModeHitPointsField->Add("2", 2, "", false, false, false);
    miBossModeHitPointsField->Add("3", 3, "", false, false);
    miBossModeHitPointsField->Add("4", 4, "", false, false);
    miBossModeHitPointsField->Add("5", 5, "", false, false);
    miBossModeHitPointsField->Add("6", 6, "", false, false);
    miBossModeHitPointsField->Add("7", 7, "", false, false);
    miBossModeHitPointsField->Add("8", 8, "", false, false);
    miBossModeHitPointsField->Add("9", 9, "", false, false, false);
    miBossModeHitPointsField->Add("10", 10, "", false, false, false);
    miBossModeHitPointsField->Add("11", 11, "", false, false, false);
    miBossModeHitPointsField->Add("12", 12, "", false, false, false);
    miBossModeHitPointsField->Add("13", 13, "", false, false, false);
    miBossModeHitPointsField->Add("14", 14, "", false, false, false);
    miBossModeHitPointsField->Add("15", 15, "", false, false, false);
    miBossModeHitPointsField->Add("16", 16, "", false, false, false);
    miBossModeHitPointsField->Add("17", 17, "", false, false, false);
    miBossModeHitPointsField->Add("18", 18, "", false, false, false);
    miBossModeHitPointsField->Add("19", 19, "", false, false, false);
    miBossModeHitPointsField->Add("20", 20, "", false, false, false);
    miBossModeHitPointsField->SetData(&game_values.gamemodemenusettings.boss.hitpoints, NULL, NULL);
    miBossModeHitPointsField->SetKey(game_values.gamemodemenusettings.boss.hitpoints);

    miBossModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miBossModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miBossModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miBossModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miBossModeHeaderText = new MI_Text("Boss Minigame Menu", 320, 5, 0, 2, 1);

    mBossSettingsMenu.AddControl(miBossModeTypeField, miBossModeBackButton, miBossModeDifficultyField, NULL, miBossModeBackButton);
    mBossSettingsMenu.AddControl(miBossModeDifficultyField, miBossModeTypeField, miBossModeHitPointsField, NULL, miBossModeBackButton);
    mBossSettingsMenu.AddControl(miBossModeHitPointsField, miBossModeDifficultyField, miBossModeBackButton, NULL, miBossModeBackButton);
    mBossSettingsMenu.AddControl(miBossModeBackButton, miBossModeHitPointsField, miBossModeTypeField, miBossModeHitPointsField, NULL);

    mBossSettingsMenu.AddNonControl(miBossModeLeftHeaderBar);
    mBossSettingsMenu.AddNonControl(miBossModeRightHeaderBar);
    mBossSettingsMenu.AddNonControl(miBossModeHeaderText);

    mBossSettingsMenu.SetHeadControl(miBossModeTypeField);
    mBossSettingsMenu.SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);
}

void UI_ModeOptionsMenu::SetControllingTeam(short iControlTeam)
{
    for (short iMode = 0; iMode < GAMEMODE_LAST; iMode++)
        mModeSettingsMenu[iMode].SetControllingTeam(iControlTeam);
}

void UI_ModeOptionsMenu::SetRandomGameModeSettings(short iMode)
{
    if (iMode == game_mode_classic) { //classic
        game_values.gamemodesettings.classic.style = miClassicModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.classic.scoring = miClassicModeScoringField->GetRandomShortValue();
    } else if (iMode == game_mode_frag) { //frag
        game_values.gamemodesettings.frag.style = miFragModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.frag.scoring = miFragModeScoringField->GetRandomShortValue();
    } else if (iMode == game_mode_timelimit) { //time
        game_values.gamemodesettings.time.style = miTimeLimitModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.time.scoring = miTimeLimitModeScoringField->GetRandomShortValue();
        game_values.gamemodesettings.time.percentextratime = miTimeLimitModePercentExtraTime->GetRandomShortValue();
    } else if (iMode == game_mode_jail) { //jail
        game_values.gamemodesettings.jail.style = miJailModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.jail.timetofree = miJailModeTimeFreeField->GetRandomShortValue();
        game_values.gamemodesettings.jail.tagfree = miJailModeTagFreeField->GetRandomBoolValue();
        game_values.gamemodesettings.jail.percentkey = miJailModeJailKeyField->GetRandomShortValue();
    } else if (iMode == game_mode_coins) { //coins
        game_values.gamemodesettings.coins.penalty = miCoinModePenaltyField->GetRandomBoolValue();
        game_values.gamemodesettings.coins.quantity = miCoinModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.coins.percentextracoin = miCoinModePercentExtraCoin->GetRandomShortValue();
    } else if (iMode == game_mode_stomp) { //stomp
        game_values.gamemodesettings.stomp.rate = miStompModeRateField->GetRandomShortValue();

        for (short iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
            game_values.gamemodesettings.stomp.enemyweight[iEnemy] = miStompModeEnemySlider[iEnemy]->GetRandomShortValue();
    } else if (iMode == game_mode_eggs) { //egg
        for (int iEgg = 0; iEgg < 4; iEgg++)
            game_values.gamemodesettings.egg.eggs[iEgg] = miEggModeEggQuantityField[iEgg]->GetRandomShortValue();

        for (int iYoshi = 0; iYoshi < 4; iYoshi++)
            game_values.gamemodesettings.egg.yoshis[iYoshi] = miEggModeYoshiQuantityField[iYoshi]->GetRandomShortValue();

        game_values.gamemodesettings.egg.explode = miEggModeExplosionTimeField->GetRandomShortValue();
    } else if (iMode == game_mode_ctf) { //capture the flag
        game_values.gamemodesettings.flag.speed = miFlagModeSpeedField->GetRandomShortValue();
        game_values.gamemodesettings.flag.touchreturn = miFlagModeTouchReturnField->GetRandomBoolValue();
        game_values.gamemodesettings.flag.pointmove = miFlagModePointMoveField->GetRandomBoolValue();
        game_values.gamemodesettings.flag.autoreturn = miFlagModeAutoReturnField->GetRandomShortValue();
        game_values.gamemodesettings.flag.homescore = miFlagModeHomeScoreField->GetRandomBoolValue();
        game_values.gamemodesettings.flag.centerflag = miFlagModeCenterFlagField->GetRandomBoolValue();
    } else if (iMode == game_mode_chicken) { //chicken
        game_values.gamemodesettings.chicken.usetarget = miChickenModeShowTargetField->GetRandomBoolValue();
        game_values.gamemodesettings.chicken.glide = miChickenModeGlideField->GetRandomBoolValue();
    } else if (iMode == game_mode_tag) { //tag
        game_values.gamemodesettings.tag.tagontouch = miTagModeTagOnTouchField->GetRandomBoolValue();
    } else if (iMode == game_mode_star) { //star
        game_values.gamemodesettings.star.time = miStarModeTimeField->GetRandomShortValue();
        game_values.gamemodesettings.star.shine = miStarModeShineField->GetRandomShortValue();
        game_values.gamemodesettings.star.percentextratime = miStarModePercentExtraTime->GetRandomShortValue();
    } else if (iMode == game_mode_domination) { //domination
        game_values.gamemodesettings.domination.quantity = miDominationModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.domination.loseondeath = miDominationModeLoseOnDeathField->GetRandomBoolValue();
        game_values.gamemodesettings.domination.relocateondeath = miDominationModeRelocateOnDeathField->GetRandomBoolValue();
        game_values.gamemodesettings.domination.stealondeath = miDominationModeStealOnDeathField->GetRandomBoolValue();
        game_values.gamemodesettings.domination.relocationfrequency = miDominationModeRelocateFrequencyField->GetRandomShortValue();
    } else if (iMode == game_mode_koth) { //king of the hill
        game_values.gamemodesettings.kingofthehill.areasize = miKingOfTheHillModeSizeField->GetRandomShortValue();
        game_values.gamemodesettings.kingofthehill.relocationfrequency = miKingOfTheHillModeRelocateFrequencyField->GetRandomShortValue();
        game_values.gamemodesettings.kingofthehill.maxmultiplier = miKingOfTheHillModeMultiplierField->GetRandomShortValue();
    } else if (iMode == game_mode_race) { //race
        game_values.gamemodesettings.race.quantity = miRaceModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.race.speed = miRaceModeSpeedField->GetRandomShortValue();
        game_values.gamemodesettings.race.penalty = miRaceModePenaltyField->GetRandomShortValue();
    } else if (iMode == game_mode_frenzy) { //frenzy
        miFrenzyModeOptions->SetRandomGameModeSettings();
    } else if (iMode == game_mode_survival) { //survival
        game_values.gamemodesettings.survival.density = miSurvivalModeDensityField->GetRandomShortValue();
        game_values.gamemodesettings.survival.speed = miSurvivalModeSpeedField->GetRandomShortValue();
        game_values.gamemodesettings.survival.shield = miSurvivalModeShieldField->GetRandomBoolValue();

        for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
            game_values.gamemodesettings.survival.enemyweight[iEnemy] = miSurvivalModeEnemySlider[iEnemy]->GetRandomShortValue();
    } else if (iMode == game_mode_greed) { //greed
        game_values.gamemodesettings.greed.coinlife = miGreedModeCoinLife->GetRandomShortValue();
        game_values.gamemodesettings.greed.owncoins = miGreedModeOwnCoins->GetRandomBoolValue();
        game_values.gamemodesettings.greed.multiplier = miGreedModeMultiplier->GetRandomShortValue();
        game_values.gamemodesettings.greed.percentextracoin = miGreedModePercentExtraCoin->GetRandomShortValue();
    } else if (iMode == game_mode_health) { //health
        game_values.gamemodesettings.health.startlife = miHealthModeStartLife->GetRandomShortValue();
        game_values.gamemodesettings.health.maxlife = miHealthModeMaxLife->GetRandomShortValue();
        game_values.gamemodesettings.health.percentextralife = miHealthModePercentExtraLife->GetRandomShortValue();
    } else if (iMode == game_mode_collection) { //card collection
        game_values.gamemodesettings.collection.quantity = miCollectionModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.collection.rate = miCollectionModeRateField->GetRandomShortValue();
        game_values.gamemodesettings.collection.banktime = miCollectionModeBankTimeField->GetRandomShortValue();
        game_values.gamemodesettings.collection.cardlife = miCollectionModeCardLifeField->GetRandomShortValue();
    } else if (iMode == game_mode_chase) { //chase (phanto)
        game_values.gamemodesettings.chase.phantospeed = miChaseModeSpeedField->GetRandomShortValue();

        for (short iPhanto = 0; iPhanto < 3; iPhanto++)
            game_values.gamemodesettings.chase.phantoquantity[iPhanto] = miChaseModeQuantitySlider[iPhanto]->GetRandomShortValue();
    } else if (iMode == game_mode_shyguytag) { //shyguy tag
        game_values.gamemodesettings.shyguytag.tagonsuicide = miShyGuyTagModeTagOnSuicideField->GetRandomBoolValue();
        game_values.gamemodesettings.shyguytag.tagtransfer = miShyGuyTagModeTagOnStompField->GetRandomShortValue();
        game_values.gamemodesettings.shyguytag.freetime = miShyGuyTagModeTagOnStompField->GetRandomShortValue();
    } else if (iMode == game_mode_boss_minigame) { //boss
        game_values.gamemodesettings.boss.bosstype = miBossModeTypeField->GetRandomShortValue();
        game_values.gamemodesettings.boss.difficulty = miBossModeDifficultyField->GetRandomShortValue();
        game_values.gamemodesettings.boss.hitpoints = miBossModeHitPointsField->GetRandomShortValue();
    }
}

void UI_ModeOptionsMenu::HealthModeStartLifeChanged()
{
    short iMaxLife = miHealthModeMaxLife->GetShortValue();
    if (miHealthModeStartLife->GetShortValue() > iMaxLife) {
        miHealthModeStartLife->SetKey(iMaxLife);
    }
}

void UI_ModeOptionsMenu::HealthModeMaxLifeChanged()
{
    short iStartLife = miHealthModeStartLife->GetShortValue();
    if (miHealthModeMaxLife->GetShortValue() < iStartLife) {
        miHealthModeMaxLife->SetKey(iStartLife);
    }
}

void UI_ModeOptionsMenu::Refresh()
{
    for (short iMode = 0; iMode < GAMEMODE_LAST; iMode++) {
        mModeSettingsMenu[iMode].Refresh();
    }

    mBossSettingsMenu.Refresh();
}
