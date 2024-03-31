#include "ModeOptionsMenu.h"

#include "GameMode.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_FrenzyModeOptions.h"
#include "ui/MI_Image.h"
#include "ui/MI_PowerupSlider.h"
#include "ui/MI_Text.h"

extern CGameValues game_values;
extern CResourceManager* rm;


UI_ModeOptionsMenu::UI_ModeOptionsMenu()
    : UI_Menu()
{

    //***********************
    // Classic Mode Settings
    //***********************

    miClassicModeStyleField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "On Kill", 400, 180);
    miClassicModeStyleField->Add("Respawn", 0);
    miClassicModeStyleField->Add("Shield", 1);
    miClassicModeStyleField->SetData(&game_values.gamemodemenusettings.classic.style, NULL, NULL);
    miClassicModeStyleField->SetKey(game_values.gamemodemenusettings.classic.style);

    miClassicModeScoringField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Scoring", 400, 180);
    miClassicModeScoringField->Add("All Kills", 0);
    miClassicModeScoringField->Add("Push Kills Only", 1, false, false);
    miClassicModeScoringField->SetData(&game_values.gamemodemenusettings.classic.scoring, NULL, NULL);
    miClassicModeScoringField->SetKey(game_values.gamemodemenusettings.classic.scoring);

    miClassicModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miClassicModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miClassicModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miClassicModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miClassicModeHeaderText = new MI_HeaderText("Classic Mode Menu", 320, 5);

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
    miFragModeStyleField->Add("Respawn", 0);
    miFragModeStyleField->Add("Shield", 1);
    miFragModeStyleField->SetData(&game_values.gamemodemenusettings.frag.style, NULL, NULL);
    miFragModeStyleField->SetKey(game_values.gamemodemenusettings.frag.style);

    miFragModeScoringField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Scoring", 400, 180);
    miFragModeScoringField->Add("All Kills", 0);
    miFragModeScoringField->Add("Push Kills Only", 1, false, false);
    miFragModeScoringField->SetData(&game_values.gamemodemenusettings.frag.scoring, NULL, NULL);
    miFragModeScoringField->SetKey(game_values.gamemodemenusettings.frag.scoring);

    miFragModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miFragModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miFragModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miFragModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miFragModeHeaderText = new MI_HeaderText("Frag Mode Menu", 320, 5);

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
    miTimeLimitModeStyleField->Add("Respawn", 0);
    miTimeLimitModeStyleField->Add("Shield", 1);
    miTimeLimitModeStyleField->SetData(&game_values.gamemodemenusettings.time.style, NULL, NULL);
    miTimeLimitModeStyleField->SetKey(game_values.gamemodemenusettings.time.style);

    miTimeLimitModeScoringField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Scoring", 400, 150);
    miTimeLimitModeScoringField->Add("All Kills", 0);
    miTimeLimitModeScoringField->Add("Push Kills Only", 1, false, false);
    miTimeLimitModeScoringField->SetData(&game_values.gamemodemenusettings.time.scoring, NULL, NULL);
    miTimeLimitModeScoringField->SetKey(game_values.gamemodemenusettings.time.scoring);

    miTimeLimitModePercentExtraTime = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Time", 400, 150, 384);
    miTimeLimitModePercentExtraTime->add("0", 0, false);
    miTimeLimitModePercentExtraTime->add("5", 5, false);
    miTimeLimitModePercentExtraTime->add("10", 10);
    miTimeLimitModePercentExtraTime->add("15", 15);
    miTimeLimitModePercentExtraTime->add("20", 20);
    miTimeLimitModePercentExtraTime->add("25", 25);
    miTimeLimitModePercentExtraTime->add("30", 30);
    miTimeLimitModePercentExtraTime->add("35", 35);
    miTimeLimitModePercentExtraTime->add("40", 40);
    miTimeLimitModePercentExtraTime->add("45", 45);
    miTimeLimitModePercentExtraTime->add("50", 50);
    miTimeLimitModePercentExtraTime->add("55", 55, false);
    miTimeLimitModePercentExtraTime->add("60", 60, false);
    miTimeLimitModePercentExtraTime->add("65", 65, false);
    miTimeLimitModePercentExtraTime->add("70", 70, false);
    miTimeLimitModePercentExtraTime->add("75", 75, false);
    miTimeLimitModePercentExtraTime->add("80", 80, false);
    miTimeLimitModePercentExtraTime->add("85", 85, false);
    miTimeLimitModePercentExtraTime->add("90", 90, false);
    miTimeLimitModePercentExtraTime->add("95", 95, false);
    miTimeLimitModePercentExtraTime->add("100", 100, false);
    miTimeLimitModePercentExtraTime->setOutputPtr(&game_values.gamemodemenusettings.time.percentextratime);
    miTimeLimitModePercentExtraTime->setCurrentValue(game_values.gamemodemenusettings.time.percentextratime);
    miTimeLimitModePercentExtraTime->allowWrap(false);

    miTimeLimitModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miTimeLimitModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miTimeLimitModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTimeLimitModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTimeLimitModeHeaderText = new MI_HeaderText("Time Mode Menu", 320, 5);

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
    miJailModeStyleField->Add("Classic", 0);
    miJailModeStyleField->Add("Owned", 1, "", true, false);
    miJailModeStyleField->Add("Free For All", 2, "", true, false);
    miJailModeStyleField->SetData(&game_values.gamemodemenusettings.jail.style, NULL, NULL);
    miJailModeStyleField->SetKey(game_values.gamemodemenusettings.jail.style);

    miJailModeTimeFreeField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "Free Timer", 400, 150);
    miJailModeTimeFreeField->Add("None", 1);
    miJailModeTimeFreeField->Add("5 Seconds", 310, false, false);
    miJailModeTimeFreeField->Add("10 Seconds", 620);
    miJailModeTimeFreeField->Add("15 Seconds", 930);
    miJailModeTimeFreeField->Add("20 Seconds", 1240);
    miJailModeTimeFreeField->Add("25 Seconds", 1550);
    miJailModeTimeFreeField->Add("30 Seconds", 1860);
    miJailModeTimeFreeField->Add("35 Seconds", 2170);
    miJailModeTimeFreeField->Add("40 Seconds", 2480);
    miJailModeTimeFreeField->Add("45 Seconds", 2790, false, false);
    miJailModeTimeFreeField->Add("50 Seconds", 3100, false, false);
    miJailModeTimeFreeField->Add("55 Seconds", 3410, false, false);
    miJailModeTimeFreeField->Add("60 Seconds", 3720, false, false);
    miJailModeTimeFreeField->SetData(&game_values.gamemodemenusettings.jail.timetofree, NULL, NULL);
    miJailModeTimeFreeField->SetKey(game_values.gamemodemenusettings.jail.timetofree);

    miJailModeTagFreeField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 240, "Tag Free", 400, 150);
    miJailModeTagFreeField->add("Off", false);
    miJailModeTagFreeField->add("On", true);
    miJailModeTagFreeField->setOutputPtr(&game_values.gamemodemenusettings.jail.tagfree);
    miJailModeTagFreeField->setCurrentValue(game_values.gamemodemenusettings.jail.tagfree ? 1 : 0);
    miJailModeTagFreeField->setAutoAdvance(true);

    miJailModeJailKeyField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 280, "Jail Key", 400, 150, 384);
    miJailModeJailKeyField->add("0", 0, false);
    miJailModeJailKeyField->add("5", 5, false);
    miJailModeJailKeyField->add("10", 10);
    miJailModeJailKeyField->add("15", 15);
    miJailModeJailKeyField->add("20", 20);
    miJailModeJailKeyField->add("25", 25);
    miJailModeJailKeyField->add("30", 30);
    miJailModeJailKeyField->add("35", 35);
    miJailModeJailKeyField->add("40", 40);
    miJailModeJailKeyField->add("45", 45);
    miJailModeJailKeyField->add("50", 50);
    miJailModeJailKeyField->add("55", 55, false);
    miJailModeJailKeyField->add("60", 60, false);
    miJailModeJailKeyField->add("65", 65, false);
    miJailModeJailKeyField->add("70", 70, false);
    miJailModeJailKeyField->add("75", 75, false);
    miJailModeJailKeyField->add("80", 80, false);
    miJailModeJailKeyField->add("85", 85, false);
    miJailModeJailKeyField->add("90", 90, false);
    miJailModeJailKeyField->add("95", 95, false);
    miJailModeJailKeyField->add("100", 100, false);
    miJailModeJailKeyField->setOutputPtr(&game_values.gamemodemenusettings.jail.percentkey);
    miJailModeJailKeyField->setCurrentValue(game_values.gamemodemenusettings.jail.percentkey);
    miJailModeJailKeyField->allowWrap(false);

    miJailModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miJailModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miJailModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miJailModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miJailModeHeaderText = new MI_HeaderText("Jail Mode Menu", 320, 5);

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

    miCoinModePenaltyField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 180, "Penalty", 400, 150);
    miCoinModePenaltyField->add("Off", false);
    miCoinModePenaltyField->add("On", true);
    miCoinModePenaltyField->setOutputPtr(&game_values.gamemodemenusettings.coins.penalty);
    miCoinModePenaltyField->setCurrentValue(game_values.gamemodemenusettings.coins.penalty ? 1 : 0);
    miCoinModePenaltyField->setAutoAdvance(true);

    miCoinModeQuantityField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Quantity", 400, 150);
    miCoinModeQuantityField->Add("1", 1);
    miCoinModeQuantityField->Add("2", 2);
    miCoinModeQuantityField->Add("3", 3);
    miCoinModeQuantityField->Add("4", 4);
    miCoinModeQuantityField->Add("5", 5);
    miCoinModeQuantityField->Add("6", 6);
    miCoinModeQuantityField->Add("7", 7);
    miCoinModeQuantityField->Add("8", 8);
    miCoinModeQuantityField->Add("9", 9);
    miCoinModeQuantityField->Add("10", 10);
    miCoinModeQuantityField->SetData(&game_values.gamemodemenusettings.coins.quantity, NULL, NULL);
    miCoinModeQuantityField->SetKey(game_values.gamemodemenusettings.coins.quantity);

    miCoinModePercentExtraCoin = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Coins", 400, 150, 384);
    miCoinModePercentExtraCoin->add("0", 0, false);
    miCoinModePercentExtraCoin->add("5", 5, false);
    miCoinModePercentExtraCoin->add("10", 10);
    miCoinModePercentExtraCoin->add("15", 15);
    miCoinModePercentExtraCoin->add("20", 20);
    miCoinModePercentExtraCoin->add("25", 25);
    miCoinModePercentExtraCoin->add("30", 30);
    miCoinModePercentExtraCoin->add("35", 35);
    miCoinModePercentExtraCoin->add("40", 40);
    miCoinModePercentExtraCoin->add("45", 45);
    miCoinModePercentExtraCoin->add("50", 50);
    miCoinModePercentExtraCoin->add("55", 55, false);
    miCoinModePercentExtraCoin->add("60", 60, false);
    miCoinModePercentExtraCoin->add("65", 65, false);
    miCoinModePercentExtraCoin->add("70", 70, false);
    miCoinModePercentExtraCoin->add("75", 75, false);
    miCoinModePercentExtraCoin->add("80", 80, false);
    miCoinModePercentExtraCoin->add("85", 85, false);
    miCoinModePercentExtraCoin->add("90", 90, false);
    miCoinModePercentExtraCoin->add("95", 95, false);
    miCoinModePercentExtraCoin->add("100", 100, false);
    miCoinModePercentExtraCoin->setOutputPtr(&game_values.gamemodemenusettings.coins.percentextracoin);
    miCoinModePercentExtraCoin->setCurrentValue(game_values.gamemodemenusettings.coins.percentextracoin);
    miCoinModePercentExtraCoin->allowWrap(false);

    miCoinModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miCoinModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miCoinModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miCoinModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miCoinModeHeaderText = new MI_HeaderText("Coin Collection Mode Menu", 320, 5);

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
    miStompModeRateField->Add("Very Slow", 150, false, false);
    miStompModeRateField->Add("Slow", 120);
    miStompModeRateField->Add("Moderate", 90);
    miStompModeRateField->Add("Fast", 60);
    miStompModeRateField->Add("Very Fast", 30, false, false);
    miStompModeRateField->Add("Extremely Fast", 15, false, false);
    miStompModeRateField->Add("Insanely Fast", 5, false, false);
    miStompModeRateField->SetData(&game_values.gamemodemenusettings.stomp.rate, NULL, NULL);
    miStompModeRateField->SetKey(game_values.gamemodemenusettings.stomp.rate);

    for (short iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++) {
        miStompModeEnemySlider[iEnemy] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->menu_stomp, 120, 80 + 40 * iEnemy, 400, iEnemy);
        miStompModeEnemySlider[iEnemy]->add("", 0);
        miStompModeEnemySlider[iEnemy]->add("", 1);
        miStompModeEnemySlider[iEnemy]->add("", 2);
        miStompModeEnemySlider[iEnemy]->add("", 3);
        miStompModeEnemySlider[iEnemy]->add("", 4);
        miStompModeEnemySlider[iEnemy]->add("", 5);
        miStompModeEnemySlider[iEnemy]->add("", 6);
        miStompModeEnemySlider[iEnemy]->add("", 7);
        miStompModeEnemySlider[iEnemy]->add("", 8);
        miStompModeEnemySlider[iEnemy]->add("", 9);
        miStompModeEnemySlider[iEnemy]->add("", 10);
        miStompModeEnemySlider[iEnemy]->allowWrap(false);
        miStompModeEnemySlider[iEnemy]->setOutputPtr(&game_values.gamemodemenusettings.stomp.enemyweight[iEnemy]);
        miStompModeEnemySlider[iEnemy]->setCurrentValue(game_values.gamemodemenusettings.stomp.enemyweight[iEnemy]);
    }

    miStompModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miStompModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miStompModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miStompModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miStompModeHeaderText = new MI_HeaderText("Stomp Mode Menu", 320, 5);


    mModeSettingsMenu[5].AddControl(miStompModeRateField, miStompModeBackButton, miStompModeEnemySlider[0], NULL, miStompModeBackButton);

    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[0], miStompModeRateField, miStompModeEnemySlider[1], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[1], miStompModeEnemySlider[0], miStompModeEnemySlider[2], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[2], miStompModeEnemySlider[1], miStompModeEnemySlider[3], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[3], miStompModeEnemySlider[2], miStompModeEnemySlider[4], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[4], miStompModeEnemySlider[3], miStompModeEnemySlider[5], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[5], miStompModeEnemySlider[4], miStompModeEnemySlider[6], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[6], miStompModeEnemySlider[5], miStompModeEnemySlider[7], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[7], miStompModeEnemySlider[6], miStompModeEnemySlider[8], NULL, miStompModeBackButton);
    mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[8], miStompModeEnemySlider[7], miStompModeBackButton, NULL, miStompModeBackButton);

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
        miEggModeEggQuantityField[iEggField]->add("0", 0, iEggField == 0 ? false : true);
        miEggModeEggQuantityField[iEggField]->add("1", 1, iEggField >= 2 ? false : true);
        miEggModeEggQuantityField[iEggField]->add("2", 2, iEggField >= 2 ? false : true);
        miEggModeEggQuantityField[iEggField]->add("3", 3, iEggField >= 1 ? false : true);
        miEggModeEggQuantityField[iEggField]->add("4", 4);
        miEggModeEggQuantityField[iEggField]->setOutputPtr(&game_values.gamemodemenusettings.egg.eggs[iEggField]);
        miEggModeEggQuantityField[iEggField]->setCurrentValue(game_values.gamemodemenusettings.egg.eggs[iEggField]);
        miEggModeEggQuantityField[iEggField]->allowWrap(false);
    }

    for (short iYoshiField = 0; iYoshiField < 4; iYoshiField++) {
        miEggModeYoshiQuantityField[iYoshiField] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->menu_egg, 170, 220 + 40 * iYoshiField, 300, iYoshiField + 4);
        miEggModeYoshiQuantityField[iYoshiField]->add("0", 0, iYoshiField == 0 ? false : true);
        miEggModeYoshiQuantityField[iYoshiField]->add("1", 1, iYoshiField >= 2 ? false : true);
        miEggModeYoshiQuantityField[iYoshiField]->add("2", 2, iYoshiField >= 1 ? false : true);
        miEggModeYoshiQuantityField[iYoshiField]->add("3", 3);
        miEggModeYoshiQuantityField[iYoshiField]->add("4", 4);
        miEggModeYoshiQuantityField[iYoshiField]->setOutputPtr(&game_values.gamemodemenusettings.egg.yoshis[iYoshiField]);
        miEggModeYoshiQuantityField[iYoshiField]->setCurrentValue(game_values.gamemodemenusettings.egg.yoshis[iYoshiField]);
        miEggModeYoshiQuantityField[iYoshiField]->allowWrap(false);
    }

    miEggModeExplosionTimeField = new MI_SelectField(&rm->spr_selectfield, 120, 380, "Explosion Timer", 400, 180);
    miEggModeExplosionTimeField->Add("Off", 0);
    miEggModeExplosionTimeField->Add("3 Seconds", 3, false, false);
    miEggModeExplosionTimeField->Add("5 Seconds", 5);
    miEggModeExplosionTimeField->Add("8 Seconds", 8);
    miEggModeExplosionTimeField->Add("10 Seconds", 10);
    miEggModeExplosionTimeField->Add("15 Seconds", 15);
    miEggModeExplosionTimeField->Add("20 Seconds", 20);
    miEggModeExplosionTimeField->SetData(&game_values.gamemodemenusettings.egg.explode, NULL, NULL);
    miEggModeExplosionTimeField->SetKey(game_values.gamemodemenusettings.egg.explode);

    miEggModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miEggModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miEggModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miEggModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miEggModeHeaderText = new MI_HeaderText("Yoshi's Eggs Mode Menu", 320, 5);

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
    miFlagModeSpeedField->add("-", -1);
    miFlagModeSpeedField->add("0", 0);
    miFlagModeSpeedField->add("1", 1);
    miFlagModeSpeedField->add("2", 2);
    miFlagModeSpeedField->add("3", 3);
    miFlagModeSpeedField->add("4", 4);
    miFlagModeSpeedField->add("5", 5);
    miFlagModeSpeedField->add("6", 6, false);
    miFlagModeSpeedField->add("7", 7, false);
    miFlagModeSpeedField->add("8", 8, false);
    miFlagModeSpeedField->setOutputPtr(&game_values.gamemodemenusettings.flag.speed);
    miFlagModeSpeedField->setCurrentValue(game_values.gamemodemenusettings.flag.speed);
    miFlagModeSpeedField->allowWrap(false);

    miFlagModeTouchReturnField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 160, "Touch Return", 400, 180);
    miFlagModeTouchReturnField->add("Off", false);
    miFlagModeTouchReturnField->add("On", true);
    miFlagModeTouchReturnField->setOutputPtr(&game_values.gamemodemenusettings.flag.touchreturn);
    miFlagModeTouchReturnField->setCurrentValue(game_values.gamemodemenusettings.flag.touchreturn ? 1 : 0);
    miFlagModeTouchReturnField->setAutoAdvance(true);

    miFlagModePointMoveField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 200, "Point Move", 400, 180);
    miFlagModePointMoveField->add("Off", false);
    miFlagModePointMoveField->add("On", true);
    miFlagModePointMoveField->setOutputPtr(&game_values.gamemodemenusettings.flag.pointmove);
    miFlagModePointMoveField->setCurrentValue(game_values.gamemodemenusettings.flag.pointmove ? 1 : 0);
    miFlagModePointMoveField->setAutoAdvance(true);

    miFlagModeAutoReturnField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Auto Return", 400, 180);
    miFlagModeAutoReturnField->Add("None", 0);
    miFlagModeAutoReturnField->Add("5 Seconds", 310, false, false);
    miFlagModeAutoReturnField->Add("10 Seconds", 620);
    miFlagModeAutoReturnField->Add("15 Seconds", 930, false, false);
    miFlagModeAutoReturnField->Add("20 Seconds", 1240);
    miFlagModeAutoReturnField->Add("25 Seconds", 1550, false, false);
    miFlagModeAutoReturnField->Add("30 Seconds", 1860, false, false);
    miFlagModeAutoReturnField->Add("35 Seconds", 2170, false, false);
    miFlagModeAutoReturnField->Add("40 Seconds", 2480, false, false);
    miFlagModeAutoReturnField->Add("45 Seconds", 2790, false, false);
    miFlagModeAutoReturnField->Add("50 Seconds", 3100, false, false);
    miFlagModeAutoReturnField->Add("55 Seconds", 3410, false, false);
    miFlagModeAutoReturnField->Add("60 Seconds", 3720, false, false);
    miFlagModeAutoReturnField->SetData(&game_values.gamemodemenusettings.flag.autoreturn, NULL, NULL);
    miFlagModeAutoReturnField->SetKey(game_values.gamemodemenusettings.flag.autoreturn);

    miFlagModeHomeScoreField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 280, "Need Home", 400, 180);
    miFlagModeHomeScoreField->add("Off", false);
    miFlagModeHomeScoreField->add("On", true);
    miFlagModeHomeScoreField->setOutputPtr(&game_values.gamemodemenusettings.flag.homescore);
    miFlagModeHomeScoreField->setCurrentValue(game_values.gamemodemenusettings.flag.homescore ? 1 : 0);
    miFlagModeHomeScoreField->setAutoAdvance(true);

    miFlagModeCenterFlagField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 320, "Center Flag", 400, 180);
    miFlagModeCenterFlagField->add("Off", false);
    miFlagModeCenterFlagField->add("On", true);
    miFlagModeCenterFlagField->setOutputPtr(&game_values.gamemodemenusettings.flag.centerflag);
    miFlagModeCenterFlagField->setCurrentValue(game_values.gamemodemenusettings.flag.centerflag ? 1 : 0);
    miFlagModeCenterFlagField->setAutoAdvance(true);

    miFlagModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miFlagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miFlagModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miFlagModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miFlagModeHeaderText = new MI_HeaderText("Capture The Flag Mode Menu", 320, 5);

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

    miChickenModeShowTargetField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 200, "Show Target", 400, 180);
    miChickenModeShowTargetField->add("Off", false);
    miChickenModeShowTargetField->add("On", true);
    miChickenModeShowTargetField->setOutputPtr(&game_values.gamemodemenusettings.chicken.usetarget);
    miChickenModeShowTargetField->setCurrentValue(game_values.gamemodemenusettings.chicken.usetarget ? 1 : 0);
    miChickenModeShowTargetField->setAutoAdvance(true);

    miChickenModeGlideField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 240, "Chicken Glide", 400, 180);
    miChickenModeGlideField->add("Off", false);
    miChickenModeGlideField->add("On", true);
    miChickenModeGlideField->setOutputPtr(&game_values.gamemodemenusettings.chicken.glide);
    miChickenModeGlideField->setCurrentValue(game_values.gamemodemenusettings.chicken.glide ? 1 : 0);
    miChickenModeGlideField->setAutoAdvance(true);

    miChickenModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miChickenModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miChickenModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miChickenModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miChickenModeHeaderText = new MI_HeaderText("Chicken Mode Menu", 320, 5);


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

    miTagModeTagOnTouchField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 220, "Touch Tag", 400, 180);
    miTagModeTagOnTouchField->add("Off", false);
    miTagModeTagOnTouchField->add("On", true);
    miTagModeTagOnTouchField->setOutputPtr(&game_values.gamemodemenusettings.tag.tagontouch);
    miTagModeTagOnTouchField->setCurrentValue(game_values.gamemodemenusettings.tag.tagontouch ? 1 : 0);
    miTagModeTagOnTouchField->setAutoAdvance(true);

    miTagModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miTagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miTagModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTagModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTagModeHeaderText = new MI_HeaderText("Tag Mode Menu", 320, 5);


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
    miStarModeTimeField->Add("5 Seconds", 5, false, false);
    miStarModeTimeField->Add("10 Seconds", 10, false, false);
    miStarModeTimeField->Add("15 Seconds", 15);
    miStarModeTimeField->Add("20 Seconds", 20);
    miStarModeTimeField->Add("25 Seconds", 25);
    miStarModeTimeField->Add("30 Seconds", 30);
    miStarModeTimeField->Add("35 Seconds", 35);
    miStarModeTimeField->Add("40 Seconds", 40);
    miStarModeTimeField->Add("45 Seconds", 45, false, false);
    miStarModeTimeField->Add("50 Seconds", 50, false, false);
    miStarModeTimeField->Add("55 Seconds", 55, false, false);
    miStarModeTimeField->Add("60 Seconds", 60, false, false);
    miStarModeTimeField->SetData(&game_values.gamemodemenusettings.star.time, NULL, NULL);
    miStarModeTimeField->SetKey(game_values.gamemodemenusettings.star.time);

    miStarModeShineField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Star Type", 400, 150);
    miStarModeShineField->Add("Ztar", 0);
    miStarModeShineField->Add("Shine", 1);
    miStarModeShineField->Add("Multi Star", 2);
    miStarModeShineField->Add("Random", 3);
    miStarModeShineField->SetData(&game_values.gamemodemenusettings.star.shine, NULL, NULL);
    miStarModeShineField->SetKey(game_values.gamemodemenusettings.star.shine);

    miStarModePercentExtraTime = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Time", 400, 150, 384);
    miStarModePercentExtraTime->add("0", 0, false);
    miStarModePercentExtraTime->add("5", 5, false);
    miStarModePercentExtraTime->add("10", 10);
    miStarModePercentExtraTime->add("15", 15);
    miStarModePercentExtraTime->add("20", 20);
    miStarModePercentExtraTime->add("25", 25);
    miStarModePercentExtraTime->add("30", 30);
    miStarModePercentExtraTime->add("35", 35);
    miStarModePercentExtraTime->add("40", 40);
    miStarModePercentExtraTime->add("45", 45);
    miStarModePercentExtraTime->add("50", 50);
    miStarModePercentExtraTime->add("55", 55, false);
    miStarModePercentExtraTime->add("60", 60, false);
    miStarModePercentExtraTime->add("65", 65, false);
    miStarModePercentExtraTime->add("70", 70, false);
    miStarModePercentExtraTime->add("75", 75, false);
    miStarModePercentExtraTime->add("80", 80, false);
    miStarModePercentExtraTime->add("85", 85, false);
    miStarModePercentExtraTime->add("90", 90, false);
    miStarModePercentExtraTime->add("95", 95, false);
    miStarModePercentExtraTime->add("100", 100, false);
    miStarModePercentExtraTime->setOutputPtr(&game_values.gamemodemenusettings.star.percentextratime);
    miStarModePercentExtraTime->setCurrentValue(game_values.gamemodemenusettings.star.percentextratime);
    miStarModePercentExtraTime->allowWrap(false);

    miStarModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miStarModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miStarModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miStarModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miStarModeHeaderText = new MI_HeaderText("Star Mode Menu", 320, 5);

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
    miDominationModeQuantityField->Add("1 Base", 1, false, false);
    miDominationModeQuantityField->Add("2 Bases", 2, false, false);
    miDominationModeQuantityField->Add("3 Bases", 3);
    miDominationModeQuantityField->Add("4 Bases", 4);
    miDominationModeQuantityField->Add("5 Bases", 5);
    miDominationModeQuantityField->Add("6 Bases", 6, false, false);
    miDominationModeQuantityField->Add("7 Bases", 7, false, false);
    miDominationModeQuantityField->Add("8 Bases", 8, false, false);
    miDominationModeQuantityField->Add("9 Bases", 9, false, false);
    miDominationModeQuantityField->Add("10 Bases", 10, false, false);
    miDominationModeQuantityField->Add("# Players - 1", 11, false, false);
    miDominationModeQuantityField->Add("# Players", 12, false, false);
    miDominationModeQuantityField->Add("# Players + 1", 13);
    miDominationModeQuantityField->Add("# Players + 2", 14);
    miDominationModeQuantityField->Add("# Players + 3", 15);
    miDominationModeQuantityField->Add("# Players + 4", 16);
    miDominationModeQuantityField->Add("# Players + 5", 17);
    miDominationModeQuantityField->Add("# Players + 6", 18, false, false);
    miDominationModeQuantityField->Add("2x Players - 3", 19, false, false);
    miDominationModeQuantityField->Add("2x Players - 2", 20, false, false);
    miDominationModeQuantityField->Add("2x Players - 1", 21);
    miDominationModeQuantityField->Add("2x Players", 22);
    miDominationModeQuantityField->Add("2x Players + 1", 23, false, false);
    miDominationModeQuantityField->Add("2x Players + 2", 24, false, false);
    miDominationModeQuantityField->SetData(&game_values.gamemodemenusettings.domination.quantity, NULL, NULL);
    miDominationModeQuantityField->SetKey(game_values.gamemodemenusettings.domination.quantity);

    miDominationModeRelocateFrequencyField = new MI_SelectField(&rm->spr_selectfield, 120, 160, "Relocate", 400, 180);
    miDominationModeRelocateFrequencyField->Add("Never", 0);
    miDominationModeRelocateFrequencyField->Add("5 Seconds", 310, false, false);
    miDominationModeRelocateFrequencyField->Add("10 Seconds", 620);
    miDominationModeRelocateFrequencyField->Add("15 Seconds", 930);
    miDominationModeRelocateFrequencyField->Add("20 Seconds", 1240);
    miDominationModeRelocateFrequencyField->Add("30 Seconds", 1860);
    miDominationModeRelocateFrequencyField->Add("45 Seconds", 2790);
    miDominationModeRelocateFrequencyField->Add("1 Minute", 3720);
    miDominationModeRelocateFrequencyField->Add("1.5 Minutes", 5580);
    miDominationModeRelocateFrequencyField->Add("2 Minutes", 7440);
    miDominationModeRelocateFrequencyField->Add("2.5 Minutes", 9300);
    miDominationModeRelocateFrequencyField->Add("3 Minutes", 11160);
    miDominationModeRelocateFrequencyField->SetData(&game_values.gamemodemenusettings.domination.relocationfrequency, NULL, NULL);
    miDominationModeRelocateFrequencyField->SetKey(game_values.gamemodemenusettings.domination.relocationfrequency);

    miDominationModeDeathText = new MI_Text("On Death", 120, 210, 0, true, TextAlign::LEFT);

    miDominationModeLoseOnDeathField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 240, "Lose Bases", 400, 180);
    miDominationModeLoseOnDeathField->add("Off", false);
    miDominationModeLoseOnDeathField->add("On", true);
    miDominationModeLoseOnDeathField->setOutputPtr(&game_values.gamemodemenusettings.domination.loseondeath);
    miDominationModeLoseOnDeathField->setCurrentValue(game_values.gamemodemenusettings.domination.loseondeath ? 1 : 0);
    miDominationModeLoseOnDeathField->setAutoAdvance(true);

    miDominationModeRelocateOnDeathField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 280, "Move Bases", 400, 180);
    miDominationModeRelocateOnDeathField->add("Off", false);
    miDominationModeRelocateOnDeathField->add("On", true);
    miDominationModeRelocateOnDeathField->setOutputPtr(&game_values.gamemodemenusettings.domination.relocateondeath);
    miDominationModeRelocateOnDeathField->setCurrentValue(game_values.gamemodemenusettings.domination.relocateondeath ? 1 : 0);
    miDominationModeRelocateOnDeathField->setAutoAdvance(true);

    miDominationModeStealOnDeathField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 320, "Steal Bases", 400, 180);
    miDominationModeStealOnDeathField->add("Off", false);
    miDominationModeStealOnDeathField->add("On", true);
    miDominationModeStealOnDeathField->setOutputPtr(&game_values.gamemodemenusettings.domination.stealondeath);
    miDominationModeStealOnDeathField->setCurrentValue(game_values.gamemodemenusettings.domination.stealondeath ? 1 : 0);
    miDominationModeStealOnDeathField->setAutoAdvance(true);


    miDominationModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miDominationModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miDominationModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miDominationModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miDominationModeHeaderText = new MI_HeaderText("Domination Mode Menu", 320, 5);

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
    miKingOfTheHillModeSizeField->Add("2 x 2", 2);
    miKingOfTheHillModeSizeField->Add("3 x 3", 3);
    miKingOfTheHillModeSizeField->Add("4 x 4", 4);
    miKingOfTheHillModeSizeField->Add("5 x 5", 5);
    miKingOfTheHillModeSizeField->Add("6 x 6", 6);
    miKingOfTheHillModeSizeField->Add("7 x 7", 7);
    miKingOfTheHillModeSizeField->SetData(&game_values.gamemodemenusettings.kingofthehill.areasize, NULL, NULL);
    miKingOfTheHillModeSizeField->SetKey(game_values.gamemodemenusettings.kingofthehill.areasize);

    miKingOfTheHillModeRelocateFrequencyField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Relocate", 400, 180);
    miKingOfTheHillModeRelocateFrequencyField->Add("Never", 0);
    miKingOfTheHillModeRelocateFrequencyField->Add("5 Seconds", 310, false, false);
    miKingOfTheHillModeRelocateFrequencyField->Add("10 Seconds", 620);
    miKingOfTheHillModeRelocateFrequencyField->Add("15 Seconds", 930);
    miKingOfTheHillModeRelocateFrequencyField->Add("20 Seconds", 1240);
    miKingOfTheHillModeRelocateFrequencyField->Add("30 Seconds", 1860);
    miKingOfTheHillModeRelocateFrequencyField->Add("45 Seconds", 2790);
    miKingOfTheHillModeRelocateFrequencyField->Add("1 Minute", 3720);
    miKingOfTheHillModeRelocateFrequencyField->Add("1.5 Minutes", 5580);
    miKingOfTheHillModeRelocateFrequencyField->Add("2 Minutes", 7440);
    miKingOfTheHillModeRelocateFrequencyField->Add("2.5 Minutes", 9300);
    miKingOfTheHillModeRelocateFrequencyField->Add("3 Minutes", 11160);
    miKingOfTheHillModeRelocateFrequencyField->SetData(&game_values.gamemodemenusettings.kingofthehill.relocationfrequency, NULL, NULL);
    miKingOfTheHillModeRelocateFrequencyField->SetKey(game_values.gamemodemenusettings.kingofthehill.relocationfrequency);

    miKingOfTheHillModeMultiplierField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Max Multiplier", 400, 180);
    miKingOfTheHillModeMultiplierField->Add("None", 1);
    miKingOfTheHillModeMultiplierField->Add("2", 2);
    miKingOfTheHillModeMultiplierField->Add("3", 3);
    miKingOfTheHillModeMultiplierField->Add("4", 4);
    miKingOfTheHillModeMultiplierField->Add("5", 5);
    miKingOfTheHillModeMultiplierField->SetData(&game_values.gamemodemenusettings.kingofthehill.maxmultiplier, NULL, NULL);
    miKingOfTheHillModeMultiplierField->SetKey(game_values.gamemodemenusettings.kingofthehill.maxmultiplier);

    miKingOfTheHillModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miKingOfTheHillModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miKingOfTheHillModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miKingOfTheHillModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miKingOfTheHillModeHeaderText = new MI_HeaderText("King of the Hill Mode Menu", 320, 5);

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
    miRaceModeQuantityField->Add("2", 2, false, false);
    miRaceModeQuantityField->Add("3", 3);
    miRaceModeQuantityField->Add("4", 4);
    miRaceModeQuantityField->Add("5", 5);
    miRaceModeQuantityField->Add("6", 6);
    miRaceModeQuantityField->Add("7", 7);
    miRaceModeQuantityField->Add("8", MAXRACEGOALS);
    miRaceModeQuantityField->SetData(&game_values.gamemodemenusettings.race.quantity, NULL, NULL);
    miRaceModeQuantityField->SetKey(game_values.gamemodemenusettings.race.quantity);

    miRaceModeSpeedField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Speed", 400, 180);
    miRaceModeSpeedField->Add("Stationary", 0);
    miRaceModeSpeedField->Add("Very Slow", 2);
    miRaceModeSpeedField->Add("Slow", 3);
    miRaceModeSpeedField->Add("Moderate", 4);
    miRaceModeSpeedField->Add("Fast", 6);
    miRaceModeSpeedField->Add("Very Fast", 8, false, false);
    miRaceModeSpeedField->Add("Extremely Fast", 15, false, false);
    miRaceModeSpeedField->Add("Insanely Fast", 30, false, false);
    miRaceModeSpeedField->SetData(&game_values.gamemodemenusettings.race.speed, NULL, NULL);
    miRaceModeSpeedField->SetKey(game_values.gamemodemenusettings.race.speed);

    miRaceModePenaltyField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Penalty", 400, 180);
    miRaceModePenaltyField->Add("None", 0);
    miRaceModePenaltyField->Add("One Goal", 1);
    miRaceModePenaltyField->Add("All Goals", 2);
    miRaceModePenaltyField->SetData(&game_values.gamemodemenusettings.race.penalty, NULL, NULL);
    miRaceModePenaltyField->SetKey(game_values.gamemodemenusettings.race.penalty);

    miRaceModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miRaceModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miRaceModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miRaceModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miRaceModeHeaderText = new MI_HeaderText("Race Mode Menu", 320, 5);

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
    miFrenzyModeHeaderText = new MI_HeaderText("Frenzy Mode Menu", 320, 5);

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
        miSurvivalModeEnemySlider[iEnemy]->add("", 0);
        miSurvivalModeEnemySlider[iEnemy]->add("", 1);
        miSurvivalModeEnemySlider[iEnemy]->add("", 2);
        miSurvivalModeEnemySlider[iEnemy]->add("", 3);
        miSurvivalModeEnemySlider[iEnemy]->add("", 4);
        miSurvivalModeEnemySlider[iEnemy]->add("", 5);
        miSurvivalModeEnemySlider[iEnemy]->add("", 6);
        miSurvivalModeEnemySlider[iEnemy]->add("", 7);
        miSurvivalModeEnemySlider[iEnemy]->add("", 8);
        miSurvivalModeEnemySlider[iEnemy]->add("", 9);
        miSurvivalModeEnemySlider[iEnemy]->add("", 10);
        miSurvivalModeEnemySlider[iEnemy]->allowWrap(false);
        miSurvivalModeEnemySlider[iEnemy]->setOutputPtr(&game_values.gamemodemenusettings.survival.enemyweight[iEnemy]);
        miSurvivalModeEnemySlider[iEnemy]->setCurrentValue(game_values.gamemodemenusettings.survival.enemyweight[iEnemy]);
    }

    miSurvivalModeDensityField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Density", 400, 180);
    miSurvivalModeDensityField->Add("Very Low", 40, false, false);
    miSurvivalModeDensityField->Add("Low", 30);
    miSurvivalModeDensityField->Add("Medium", 20);
    miSurvivalModeDensityField->Add("High", 15);
    miSurvivalModeDensityField->Add("Very High", 10, false, false);
    miSurvivalModeDensityField->Add("Extremely High", 6);
    miSurvivalModeDensityField->Add("Insanely High", 2, false, false);
    miSurvivalModeDensityField->SetData(&game_values.gamemodemenusettings.survival.density, NULL, NULL);
    miSurvivalModeDensityField->SetKey(game_values.gamemodemenusettings.survival.density);

    miSurvivalModeSpeedField = new MI_SelectField(&rm->spr_selectfield, 120, 280, "Speed", 400, 180);
    miSurvivalModeSpeedField->Add("Very Slow", 2, false, false);
    miSurvivalModeSpeedField->Add("Slow", 3);
    miSurvivalModeSpeedField->Add("Moderate", 4);
    miSurvivalModeSpeedField->Add("Fast", 6);
    miSurvivalModeSpeedField->Add("Very Fast", 8, false, false);
    miSurvivalModeSpeedField->Add("Extremely Fast", 15, false, false);
    miSurvivalModeSpeedField->Add("Insanely Fast", 30, false, false);
    miSurvivalModeSpeedField->SetData(&game_values.gamemodemenusettings.survival.speed, NULL, NULL);
    miSurvivalModeSpeedField->SetKey(game_values.gamemodemenusettings.survival.speed);

    miSurvivalModeShieldField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 320, "Shield", 400, 180);
    miSurvivalModeShieldField->add("Off", false);
    miSurvivalModeShieldField->add("On", true);
    miSurvivalModeShieldField->setOutputPtr(&game_values.gamemodemenusettings.survival.shield);
    miSurvivalModeShieldField->setCurrentValue(game_values.gamemodemenusettings.survival.shield ? 1 : 0);
    miSurvivalModeShieldField->setAutoAdvance(true);

    miSurvivalModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miSurvivalModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miSurvivalModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miSurvivalModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miSurvivalModeHeaderText = new MI_HeaderText("Survival Mode Menu", 320, 5);

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
    miGreedModeCoinLife->Add("1 Second", 62, false, false);
    miGreedModeCoinLife->Add("2 Seconds", 124, false, false);
    miGreedModeCoinLife->Add("3 Seconds", 186);
    miGreedModeCoinLife->Add("4 Seconds", 248);
    miGreedModeCoinLife->Add("5 Seconds", 310);
    miGreedModeCoinLife->Add("6 Seconds", 372);
    miGreedModeCoinLife->Add("7 Seconds", 434);
    miGreedModeCoinLife->Add("8 Seconds", 496);
    miGreedModeCoinLife->Add("9 Seconds", 558);
    miGreedModeCoinLife->Add("10 Seconds", 620);
    miGreedModeCoinLife->Add("12 Seconds", 744);
    miGreedModeCoinLife->Add("15 Seconds", 930);
    miGreedModeCoinLife->Add("18 Seconds", 1116);
    miGreedModeCoinLife->Add("20 Seconds", 1240);
    miGreedModeCoinLife->Add("25 Seconds", 1550);
    miGreedModeCoinLife->Add("30 Seconds", 1860);
    miGreedModeCoinLife->SetData(&game_values.gamemodemenusettings.greed.coinlife, NULL, NULL);
    miGreedModeCoinLife->SetKey(game_values.gamemodemenusettings.greed.coinlife);

    miGreedModeOwnCoins = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 200, "Own Coins", 400, 150);
    miGreedModeOwnCoins->add("Yes", true);
    miGreedModeOwnCoins->add("No", false);
    miGreedModeOwnCoins->setOutputPtr(&game_values.gamemodemenusettings.greed.owncoins);
    miGreedModeOwnCoins->setCurrentValue(game_values.gamemodemenusettings.greed.owncoins);
    miGreedModeOwnCoins->setAutoAdvance(true);

    miGreedModeMultiplier = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Multipler", 400, 150);
    miGreedModeMultiplier->Add("0.5", 1, false, false);
    miGreedModeMultiplier->Add("1", 2);
    miGreedModeMultiplier->Add("1.5", 3);
    miGreedModeMultiplier->Add("2", 4);
    miGreedModeMultiplier->Add("2.5", 5, false, false);
    miGreedModeMultiplier->Add("3", 6, false, false);
    miGreedModeMultiplier->SetData(&game_values.gamemodemenusettings.greed.multiplier, NULL, NULL);
    miGreedModeMultiplier->SetKey(game_values.gamemodemenusettings.greed.multiplier);

    miGreedModePercentExtraCoin = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 280, "Extra Coins", 400, 150, 384);
    miGreedModePercentExtraCoin->add("0", 0, false);
    miGreedModePercentExtraCoin->add("5", 5, false);
    miGreedModePercentExtraCoin->add("10", 10);
    miGreedModePercentExtraCoin->add("15", 15);
    miGreedModePercentExtraCoin->add("20", 20);
    miGreedModePercentExtraCoin->add("25", 25);
    miGreedModePercentExtraCoin->add("30", 30);
    miGreedModePercentExtraCoin->add("35", 35);
    miGreedModePercentExtraCoin->add("40", 40);
    miGreedModePercentExtraCoin->add("45", 45);
    miGreedModePercentExtraCoin->add("50", 50);
    miGreedModePercentExtraCoin->add("55", 55, false);
    miGreedModePercentExtraCoin->add("60", 60, false);
    miGreedModePercentExtraCoin->add("65", 65, false);
    miGreedModePercentExtraCoin->add("70", 70, false);
    miGreedModePercentExtraCoin->add("75", 75, false);
    miGreedModePercentExtraCoin->add("80", 80, false);
    miGreedModePercentExtraCoin->add("85", 85, false);
    miGreedModePercentExtraCoin->add("90", 90, false);
    miGreedModePercentExtraCoin->add("95", 95, false);
    miGreedModePercentExtraCoin->add("100", 100, false);
    miGreedModePercentExtraCoin->setOutputPtr(&game_values.gamemodemenusettings.greed.percentextracoin);
    miGreedModePercentExtraCoin->setCurrentValue(game_values.gamemodemenusettings.greed.percentextracoin);
    miGreedModePercentExtraCoin->allowWrap(false);

    miGreedModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miGreedModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miGreedModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miGreedModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miGreedModeHeaderText = new MI_HeaderText("Greed Mode Menu", 320, 5);

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
    miHealthModeStartLife->Add("2", 2, false, false);
    miHealthModeStartLife->Add("3", 3, false, false);
    miHealthModeStartLife->Add("4", 4);
    miHealthModeStartLife->Add("5", 5);
    miHealthModeStartLife->Add("6", 6);
    miHealthModeStartLife->Add("7", 7, false, false);
    miHealthModeStartLife->Add("8", 8, false, false);
    miHealthModeStartLife->Add("9", 9, false, false);
    miHealthModeStartLife->Add("10", 10, false, false);
    miHealthModeStartLife->SetData(&game_values.gamemodemenusettings.health.startlife, NULL, NULL);
    miHealthModeStartLife->SetKey(game_values.gamemodemenusettings.health.startlife);
    miHealthModeStartLife->SetNoWrap(true);
    miHealthModeStartLife->SetItemChangedCode(MENU_CODE_HEALTH_MODE_START_LIFE_CHANGED);

    miHealthModeMaxLife = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Max Life", 400, 150);
    miHealthModeMaxLife->Add("2", 2, false, false);
    miHealthModeMaxLife->Add("3", 3, false, false);
    miHealthModeMaxLife->Add("4", 4, false, false);
    miHealthModeMaxLife->Add("5", 5, false, false);
    miHealthModeMaxLife->Add("6", 6);
    miHealthModeMaxLife->Add("7", 7);
    miHealthModeMaxLife->Add("8", 8);
    miHealthModeMaxLife->Add("9", 9);
    miHealthModeMaxLife->Add("10", 10);
    miHealthModeMaxLife->SetData(&game_values.gamemodemenusettings.health.maxlife, NULL, NULL);
    miHealthModeMaxLife->SetKey(game_values.gamemodemenusettings.health.maxlife);
    miHealthModeMaxLife->SetNoWrap(true);
    miHealthModeMaxLife->SetItemChangedCode(MENU_CODE_HEALTH_MODE_MAX_LIFE_CHANGED);

    miHealthModePercentExtraLife = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 120, 260, "Extra Life", 400, 150, 384);
    miHealthModePercentExtraLife->add("0", 0, false);
    miHealthModePercentExtraLife->add("5", 5, false);
    miHealthModePercentExtraLife->add("10", 10);
    miHealthModePercentExtraLife->add("15", 15);
    miHealthModePercentExtraLife->add("20", 20);
    miHealthModePercentExtraLife->add("25", 25);
    miHealthModePercentExtraLife->add("30", 30);
    miHealthModePercentExtraLife->add("35", 35);
    miHealthModePercentExtraLife->add("40", 40);
    miHealthModePercentExtraLife->add("45", 45);
    miHealthModePercentExtraLife->add("50", 50);
    miHealthModePercentExtraLife->add("55", 55, false);
    miHealthModePercentExtraLife->add("60", 60, false);
    miHealthModePercentExtraLife->add("65", 65, false);
    miHealthModePercentExtraLife->add("70", 70, false);
    miHealthModePercentExtraLife->add("75", 75, false);
    miHealthModePercentExtraLife->add("80", 80, false);
    miHealthModePercentExtraLife->add("85", 85, false);
    miHealthModePercentExtraLife->add("90", 90, false);
    miHealthModePercentExtraLife->add("95", 95, false);
    miHealthModePercentExtraLife->add("100", 100, false);
    miHealthModePercentExtraLife->setOutputPtr(&game_values.gamemodemenusettings.health.percentextralife);
    miHealthModePercentExtraLife->setCurrentValue(game_values.gamemodemenusettings.health.percentextralife);
    miHealthModePercentExtraLife->allowWrap(false);

    miHealthModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miHealthModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miHealthModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miHealthModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miHealthModeHeaderText = new MI_HeaderText("Health Mode Menu", 320, 5);

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
    miCollectionModeQuantityField->Add("1 Card", 1);
    miCollectionModeQuantityField->Add("2 Cards", 2);
    miCollectionModeQuantityField->Add("3 Cards", 3);
    miCollectionModeQuantityField->Add("4 Cards", 4, false, false);
    miCollectionModeQuantityField->Add("5 Cards", 5, false, false);
    miCollectionModeQuantityField->Add("# Players - 1", 6);
    miCollectionModeQuantityField->Add("# Players", 7);
    miCollectionModeQuantityField->Add("# Players + 1", 8, false, false);
    miCollectionModeQuantityField->Add("# Players + 2", 9, false, false);
    miCollectionModeQuantityField->Add("# Players + 3", 10, false, false);
    miCollectionModeQuantityField->SetData(&game_values.gamemodemenusettings.collection.quantity, NULL, NULL);
    miCollectionModeQuantityField->SetKey(game_values.gamemodemenusettings.collection.quantity);

    miCollectionModeRateField = new MI_SelectField(&rm->spr_selectfield, 120, 200, "Rate", 400, 180);
    miCollectionModeRateField->Add("Instant", 0);
    miCollectionModeRateField->Add("1 Second", 62);
    miCollectionModeRateField->Add("2 Seconds", 124);
    miCollectionModeRateField->Add("3 Seconds", 186);
    miCollectionModeRateField->Add("5 Seconds", 310);
    miCollectionModeRateField->Add("10 Seconds", 620, false, false);
    miCollectionModeRateField->Add("15 Seconds", 930, false, false);
    miCollectionModeRateField->Add("20 Seconds", 1240, false, false);
    miCollectionModeRateField->Add("25 Seconds", 1550, false, false);
    miCollectionModeRateField->Add("30 Seconds", 1860, false, false);
    miCollectionModeRateField->SetData(&game_values.gamemodemenusettings.collection.rate, NULL, NULL);
    miCollectionModeRateField->SetKey(game_values.gamemodemenusettings.collection.rate);

    miCollectionModeBankTimeField = new MI_SelectField(&rm->spr_selectfield, 120, 240, "Bank Time", 400, 180);
    miCollectionModeBankTimeField->Add("Instant", 0, false, false);
    miCollectionModeBankTimeField->Add("1 Second", 62, false, false);
    miCollectionModeBankTimeField->Add("2 Seconds", 124);
    miCollectionModeBankTimeField->Add("3 Seconds", 186);
    miCollectionModeBankTimeField->Add("4 Seconds", 248);
    miCollectionModeBankTimeField->Add("5 Seconds", 310);
    miCollectionModeBankTimeField->Add("6 Seconds", 372, false, false);
    miCollectionModeBankTimeField->Add("7 Seconds", 434, false, false);
    miCollectionModeBankTimeField->Add("8 Seconds", 496, false, false);
    miCollectionModeBankTimeField->Add("9 Seconds", 558, false, false);
    miCollectionModeBankTimeField->Add("10 Seconds", 620, false, false);
    miCollectionModeBankTimeField->SetData(&game_values.gamemodemenusettings.collection.banktime, NULL, NULL);
    miCollectionModeBankTimeField->SetKey(game_values.gamemodemenusettings.collection.banktime);

    miCollectionModeCardLifeField = new MI_SelectField(&rm->spr_selectfield, 120, 280, "Card Life", 400, 180);
    miCollectionModeCardLifeField->Add("1 Second", 62, false, false);
    miCollectionModeCardLifeField->Add("2 Seconds", 124, false, false);
    miCollectionModeCardLifeField->Add("3 Seconds", 186);
    miCollectionModeCardLifeField->Add("4 Seconds", 248);
    miCollectionModeCardLifeField->Add("5 Seconds", 310);
    miCollectionModeCardLifeField->Add("6 Seconds", 372);
    miCollectionModeCardLifeField->Add("7 Seconds", 434);
    miCollectionModeCardLifeField->Add("8 Seconds", 496);
    miCollectionModeCardLifeField->Add("9 Seconds", 558);
    miCollectionModeCardLifeField->Add("10 Seconds", 620);
    miCollectionModeCardLifeField->Add("12 Seconds", 744);
    miCollectionModeCardLifeField->Add("15 Seconds", 930);
    miCollectionModeCardLifeField->Add("18 Seconds", 1116);
    miCollectionModeCardLifeField->Add("20 Seconds", 1240);
    miCollectionModeCardLifeField->Add("25 Seconds", 1550);
    miCollectionModeCardLifeField->Add("30 Seconds", 1860);
    miCollectionModeCardLifeField->SetData(&game_values.gamemodemenusettings.collection.cardlife, NULL, NULL);
    miCollectionModeCardLifeField->SetKey(game_values.gamemodemenusettings.collection.cardlife);

    miCollectionModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miCollectionModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miCollectionModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miCollectionModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miCollectionModeHeaderText = new MI_HeaderText("Card Collection Mode Menu", 320, 5);

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
    miChaseModeSpeedField->Add("Very Slow", 3, false, false);
    miChaseModeSpeedField->Add("Slow", 4);
    miChaseModeSpeedField->Add("Moderate", 5);
    miChaseModeSpeedField->Add("Fast", 6);
    miChaseModeSpeedField->Add("Very Fast", 7);
    miChaseModeSpeedField->Add("Extremely Fast", 8, false, false);
    miChaseModeSpeedField->Add("Insanely Fast", 10, false, false);
    miChaseModeSpeedField->SetData(&game_values.gamemodemenusettings.chase.phantospeed, NULL, NULL);
    miChaseModeSpeedField->SetKey(game_values.gamemodemenusettings.chase.phantospeed);

    for (short iPhanto = 0; iPhanto < 3; iPhanto++) {
        miChaseModeQuantitySlider[iPhanto] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->spr_phanto, 120, 200 + 40 * iPhanto, 400, iPhanto);
        miChaseModeQuantitySlider[iPhanto]->add("", 0, iPhanto == 0 ? false : true);
        miChaseModeQuantitySlider[iPhanto]->add("", 1);
        miChaseModeQuantitySlider[iPhanto]->add("", 2);
        miChaseModeQuantitySlider[iPhanto]->add("", 3, false);
        miChaseModeQuantitySlider[iPhanto]->add("", 4, false);
        miChaseModeQuantitySlider[iPhanto]->add("", 5, false);
        miChaseModeQuantitySlider[iPhanto]->allowWrap(false);
        miChaseModeQuantitySlider[iPhanto]->setOutputPtr(&game_values.gamemodemenusettings.chase.phantoquantity[iPhanto]);
        miChaseModeQuantitySlider[iPhanto]->setCurrentValue(game_values.gamemodemenusettings.chase.phantoquantity[iPhanto]);
    }

    miChaseModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miChaseModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miChaseModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miChaseModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miChaseModeHeaderText = new MI_HeaderText("Phanto Mode Menu", 320, 5);

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

    miShyGuyTagModeTagOnSuicideField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 120, 180, "Suicide Tag", 400, 180);
    miShyGuyTagModeTagOnSuicideField->add("Off", false);
    miShyGuyTagModeTagOnSuicideField->add("On", true);
    miShyGuyTagModeTagOnSuicideField->setOutputPtr(&game_values.gamemodemenusettings.shyguytag.tagonsuicide);
    miShyGuyTagModeTagOnSuicideField->setCurrentValue(game_values.gamemodemenusettings.shyguytag.tagonsuicide ? 1 : 0);
    miShyGuyTagModeTagOnSuicideField->setAutoAdvance(true);

    miShyGuyTagModeTagOnStompField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Tag Transfer", 400, 180);
    miShyGuyTagModeTagOnStompField->Add("Touch Only", 0);
    miShyGuyTagModeTagOnStompField->Add("Kills Only", 1);
    miShyGuyTagModeTagOnStompField->Add("Touch and Kills", 2);
    miShyGuyTagModeTagOnStompField->SetData(&game_values.gamemodemenusettings.shyguytag.tagtransfer, NULL, NULL);
    miShyGuyTagModeTagOnStompField->SetKey(game_values.gamemodemenusettings.shyguytag.tagtransfer);

    miShyGuyTagModeFreeTimeField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Free Time", 400, 180);
    miShyGuyTagModeFreeTimeField->Add("Instant", 0);
    miShyGuyTagModeFreeTimeField->Add("1 Second", 1);
    miShyGuyTagModeFreeTimeField->Add("2 Seconds", 2);
    miShyGuyTagModeFreeTimeField->Add("3 Seconds", 3);
    miShyGuyTagModeFreeTimeField->Add("4 Seconds", 4);
    miShyGuyTagModeFreeTimeField->Add("5 Seconds", 5);
    miShyGuyTagModeFreeTimeField->Add("6 Seconds", 6);
    miShyGuyTagModeFreeTimeField->Add("7 Seconds", 7);
    miShyGuyTagModeFreeTimeField->Add("8 Seconds", 8);
    miShyGuyTagModeFreeTimeField->Add("9 Seconds", 9);
    miShyGuyTagModeFreeTimeField->Add("10 Seconds", 10);
    miShyGuyTagModeFreeTimeField->SetData(&game_values.gamemodemenusettings.shyguytag.freetime, NULL, NULL);
    miShyGuyTagModeFreeTimeField->SetKey(game_values.gamemodemenusettings.shyguytag.freetime);

    miShyGuyTagModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miShyGuyTagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miShyGuyTagModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miShyGuyTagModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miShyGuyTagModeHeaderText = new MI_HeaderText("Shyguy Tag Mode Menu", 320, 5);

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
    miBossModeTypeField->Add("Hammer", 0);
    miBossModeTypeField->Add("Bomb", 1);
    miBossModeTypeField->Add("Fire", 2);
    miBossModeTypeField->SetData(&game_values.gamemodemenusettings.boss.bosstype, NULL, NULL);
    miBossModeTypeField->SetKey(game_values.gamemodemenusettings.boss.bosstype);

    miBossModeDifficultyField = new MI_SelectField(&rm->spr_selectfield, 120, 220, "Difficulty", 400, 180);
    miBossModeDifficultyField->Add("Very Easy", 0, false, false);
    miBossModeDifficultyField->Add("Easy", 1);
    miBossModeDifficultyField->Add("Moderate", 2);
    miBossModeDifficultyField->Add("Hard", 3);
    miBossModeDifficultyField->Add("Very Hard", 4, false, false);
    miBossModeDifficultyField->SetData(&game_values.gamemodemenusettings.boss.difficulty, NULL, NULL);
    miBossModeDifficultyField->SetKey(game_values.gamemodemenusettings.boss.difficulty);

    miBossModeHitPointsField = new MI_SelectField(&rm->spr_selectfield, 120, 260, "Health", 400, 180);
    miBossModeHitPointsField->Add("1", 1, false, false);
    miBossModeHitPointsField->Add("2", 2, false, false);
    miBossModeHitPointsField->Add("3", 3);
    miBossModeHitPointsField->Add("4", 4);
    miBossModeHitPointsField->Add("5", 5);
    miBossModeHitPointsField->Add("6", 6);
    miBossModeHitPointsField->Add("7", 7);
    miBossModeHitPointsField->Add("8", 8);
    miBossModeHitPointsField->Add("9", 9, false, false);
    miBossModeHitPointsField->Add("10", 10, false, false);
    miBossModeHitPointsField->Add("11", 11, false, false);
    miBossModeHitPointsField->Add("12", 12, false, false);
    miBossModeHitPointsField->Add("13", 13, false, false);
    miBossModeHitPointsField->Add("14", 14, false, false);
    miBossModeHitPointsField->Add("15", 15, false, false);
    miBossModeHitPointsField->Add("16", 16, false, false);
    miBossModeHitPointsField->Add("17", 17, false, false);
    miBossModeHitPointsField->Add("18", 18, false, false);
    miBossModeHitPointsField->Add("19", 19, false, false);
    miBossModeHitPointsField->Add("20", 20, false, false);
    miBossModeHitPointsField->SetData(&game_values.gamemodemenusettings.boss.hitpoints, NULL, NULL);
    miBossModeHitPointsField->SetKey(game_values.gamemodemenusettings.boss.hitpoints);

    miBossModeBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miBossModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miBossModeLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miBossModeRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miBossModeHeaderText = new MI_HeaderText("Boss Minigame Menu", 320, 5);

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
    if (iMode == game_mode_classic) { // classic
        game_values.gamemodesettings.classic.style = miClassicModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.classic.scoring = miClassicModeScoringField->GetRandomShortValue();
    } else if (iMode == game_mode_frag) { // frag
        game_values.gamemodesettings.frag.style = miFragModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.frag.scoring = miFragModeScoringField->GetRandomShortValue();
    } else if (iMode == game_mode_timelimit) { // time
        game_values.gamemodesettings.time.style = miTimeLimitModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.time.scoring = miTimeLimitModeScoringField->GetRandomShortValue();
        game_values.gamemodesettings.time.percentextratime = miTimeLimitModePercentExtraTime->currentValue();
    } else if (iMode == game_mode_jail) { // jail
        game_values.gamemodesettings.jail.style = miJailModeStyleField->GetRandomShortValue();
        game_values.gamemodesettings.jail.timetofree = miJailModeTimeFreeField->GetRandomShortValue();
        game_values.gamemodesettings.jail.tagfree = miJailModeTagFreeField->randomValue();
        game_values.gamemodesettings.jail.percentkey = miJailModeJailKeyField->currentValue();
    } else if (iMode == game_mode_coins) { // coins
        game_values.gamemodesettings.coins.penalty = miCoinModePenaltyField->randomValue();
        game_values.gamemodesettings.coins.quantity = miCoinModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.coins.percentextracoin = miCoinModePercentExtraCoin->currentValue();
    } else if (iMode == game_mode_stomp) { // stomp
        game_values.gamemodesettings.stomp.rate = miStompModeRateField->GetRandomShortValue();

        for (short iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
            game_values.gamemodesettings.stomp.enemyweight[iEnemy] = miStompModeEnemySlider[iEnemy]->currentValue();
    } else if (iMode == game_mode_eggs) { // egg
        for (int iEgg = 0; iEgg < 4; iEgg++)
            game_values.gamemodesettings.egg.eggs[iEgg] = miEggModeEggQuantityField[iEgg]->currentValue();

        for (int iYoshi = 0; iYoshi < 4; iYoshi++)
            game_values.gamemodesettings.egg.yoshis[iYoshi] = miEggModeYoshiQuantityField[iYoshi]->currentValue();

        game_values.gamemodesettings.egg.explode = miEggModeExplosionTimeField->GetRandomShortValue();
    } else if (iMode == game_mode_ctf) { // capture the flag
        game_values.gamemodesettings.flag.speed = miFlagModeSpeedField->currentValue();
        game_values.gamemodesettings.flag.touchreturn = miFlagModeTouchReturnField->randomValue();
        game_values.gamemodesettings.flag.pointmove = miFlagModePointMoveField->randomValue();
        game_values.gamemodesettings.flag.autoreturn = miFlagModeAutoReturnField->GetRandomShortValue();
        game_values.gamemodesettings.flag.homescore = miFlagModeHomeScoreField->randomValue();
        game_values.gamemodesettings.flag.centerflag = miFlagModeCenterFlagField->randomValue();
    } else if (iMode == game_mode_chicken) { // chicken
        game_values.gamemodesettings.chicken.usetarget = miChickenModeShowTargetField->randomValue();
        game_values.gamemodesettings.chicken.glide = miChickenModeGlideField->randomValue();
    } else if (iMode == game_mode_tag) { // tag
        game_values.gamemodesettings.tag.tagontouch = miTagModeTagOnTouchField->randomValue();
    } else if (iMode == game_mode_star) { // star
        game_values.gamemodesettings.star.time = miStarModeTimeField->GetRandomShortValue();
        game_values.gamemodesettings.star.shine = miStarModeShineField->GetRandomShortValue();
        game_values.gamemodesettings.star.percentextratime = miStarModePercentExtraTime->currentValue();
    } else if (iMode == game_mode_domination) { // domination
        game_values.gamemodesettings.domination.quantity = miDominationModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.domination.loseondeath = miDominationModeLoseOnDeathField->randomValue();
        game_values.gamemodesettings.domination.relocateondeath = miDominationModeRelocateOnDeathField->randomValue();
        game_values.gamemodesettings.domination.stealondeath = miDominationModeStealOnDeathField->randomValue();
        game_values.gamemodesettings.domination.relocationfrequency = miDominationModeRelocateFrequencyField->GetRandomShortValue();
    } else if (iMode == game_mode_koth) { // king of the hill
        game_values.gamemodesettings.kingofthehill.areasize = miKingOfTheHillModeSizeField->GetRandomShortValue();
        game_values.gamemodesettings.kingofthehill.relocationfrequency = miKingOfTheHillModeRelocateFrequencyField->GetRandomShortValue();
        game_values.gamemodesettings.kingofthehill.maxmultiplier = miKingOfTheHillModeMultiplierField->GetRandomShortValue();
    } else if (iMode == game_mode_race) { // race
        game_values.gamemodesettings.race.quantity = miRaceModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.race.speed = miRaceModeSpeedField->GetRandomShortValue();
        game_values.gamemodesettings.race.penalty = miRaceModePenaltyField->GetRandomShortValue();
    } else if (iMode == game_mode_frenzy) { // frenzy
        miFrenzyModeOptions->SetRandomGameModeSettings();
    } else if (iMode == game_mode_survival) { // survival
        game_values.gamemodesettings.survival.density = miSurvivalModeDensityField->GetRandomShortValue();
        game_values.gamemodesettings.survival.speed = miSurvivalModeSpeedField->GetRandomShortValue();
        game_values.gamemodesettings.survival.shield = miSurvivalModeShieldField->randomValue();

        for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
            game_values.gamemodesettings.survival.enemyweight[iEnemy] = miSurvivalModeEnemySlider[iEnemy]->currentValue();
    } else if (iMode == game_mode_greed) { // greed
        game_values.gamemodesettings.greed.coinlife = miGreedModeCoinLife->GetRandomShortValue();
        game_values.gamemodesettings.greed.owncoins = miGreedModeOwnCoins->randomValue();
        game_values.gamemodesettings.greed.multiplier = miGreedModeMultiplier->GetRandomShortValue();
        game_values.gamemodesettings.greed.percentextracoin = miGreedModePercentExtraCoin->currentValue();
    } else if (iMode == game_mode_health) { // health
        game_values.gamemodesettings.health.startlife = miHealthModeStartLife->GetRandomShortValue();
        game_values.gamemodesettings.health.maxlife = miHealthModeMaxLife->GetRandomShortValue();
        game_values.gamemodesettings.health.percentextralife = miHealthModePercentExtraLife->currentValue();
    } else if (iMode == game_mode_collection) { // card collection
        game_values.gamemodesettings.collection.quantity = miCollectionModeQuantityField->GetRandomShortValue();
        game_values.gamemodesettings.collection.rate = miCollectionModeRateField->GetRandomShortValue();
        game_values.gamemodesettings.collection.banktime = miCollectionModeBankTimeField->GetRandomShortValue();
        game_values.gamemodesettings.collection.cardlife = miCollectionModeCardLifeField->GetRandomShortValue();
    } else if (iMode == game_mode_chase) { // chase (phanto)
        game_values.gamemodesettings.chase.phantospeed = miChaseModeSpeedField->GetRandomShortValue();

        for (short iPhanto = 0; iPhanto < 3; iPhanto++)
            game_values.gamemodesettings.chase.phantoquantity[iPhanto] = miChaseModeQuantitySlider[iPhanto]->currentValue();
    } else if (iMode == game_mode_shyguytag) { // shyguy tag
        game_values.gamemodesettings.shyguytag.tagonsuicide = miShyGuyTagModeTagOnSuicideField->randomValue();
        game_values.gamemodesettings.shyguytag.tagtransfer = miShyGuyTagModeTagOnStompField->GetRandomShortValue();
        game_values.gamemodesettings.shyguytag.freetime = miShyGuyTagModeTagOnStompField->GetRandomShortValue();
    } else if (iMode == game_mode_boss_minigame) { // boss
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
