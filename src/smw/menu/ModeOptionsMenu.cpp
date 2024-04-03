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

    miClassicModeStyleField = new MI_SelectField<DeathStyle>(&rm->spr_selectfield, 120, 200, "On Kill", 400, 180);
    miClassicModeStyleField->add("Respawn", DeathStyle::Respawn);
    miClassicModeStyleField->add("Shield", DeathStyle::Shield);
    miClassicModeStyleField->setOutputPtr(&game_values.gamemodemenusettings.classic.style);
    miClassicModeStyleField->setCurrentValue(game_values.gamemodemenusettings.classic.style);

    miClassicModeScoringField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 240, "Scoring", 400, 180);
    miClassicModeScoringField->add("All Kills", 0);
    miClassicModeScoringField->add("Push Kills Only", 1, false);
    miClassicModeScoringField->setOutputPtr(&game_values.gamemodemenusettings.classic.scoring);
    miClassicModeScoringField->setCurrentValue(game_values.gamemodemenusettings.classic.scoring);

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

    miFragModeStyleField = new MI_SelectField<DeathStyle>(&rm->spr_selectfield, 120, 200, "On Kill", 400, 180);
    miFragModeStyleField->add("Respawn", DeathStyle::Respawn);
    miFragModeStyleField->add("Shield", DeathStyle::Shield);
    miFragModeStyleField->setOutputPtr(&game_values.gamemodemenusettings.frag.style);
    miFragModeStyleField->setCurrentValue(game_values.gamemodemenusettings.frag.style);

    miFragModeScoringField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 240, "Scoring", 400, 180);
    miFragModeScoringField->add("All Kills", 0);
    miFragModeScoringField->add("Push Kills Only", 1, false);
    miFragModeScoringField->setOutputPtr(&game_values.gamemodemenusettings.frag.scoring);
    miFragModeScoringField->setCurrentValue(game_values.gamemodemenusettings.frag.scoring);

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

    miTimeLimitModeStyleField = new MI_SelectField<DeathStyle>(&rm->spr_selectfield, 120, 180, "On Kill", 400, 150);
    miTimeLimitModeStyleField->add("Respawn", DeathStyle::Respawn);
    miTimeLimitModeStyleField->add("Shield", DeathStyle::Shield);
    miTimeLimitModeStyleField->setOutputPtr(&game_values.gamemodemenusettings.time.style);
    miTimeLimitModeStyleField->setCurrentValue(game_values.gamemodemenusettings.time.style);

    miTimeLimitModeScoringField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 220, "Scoring", 400, 150);
    miTimeLimitModeScoringField->add("All Kills", 0);
    miTimeLimitModeScoringField->add("Push Kills Only", 1, false);
    miTimeLimitModeScoringField->setOutputPtr(&game_values.gamemodemenusettings.time.scoring);
    miTimeLimitModeScoringField->setCurrentValue(game_values.gamemodemenusettings.time.scoring);

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

    miJailModeStyleField = new MI_SelectField<JailStyle>(&rm->spr_selectfield, 120, 160, "Style", 400, 150);
    miJailModeStyleField->add("Classic", JailStyle::Classic);
    miJailModeStyleField->add("Owned", JailStyle::Owned);
    miJailModeStyleField->add("Free For All", JailStyle::FreeForAll);
    miJailModeStyleField->setOutputPtr(&game_values.gamemodemenusettings.jail.style);
    miJailModeStyleField->setCurrentValue(game_values.gamemodemenusettings.jail.style);

    miJailModeTimeFreeField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 200, "Free Timer", 400, 150);
    miJailModeTimeFreeField->add("None", 1);
    miJailModeTimeFreeField->add("5 Seconds", 310, false);
    miJailModeTimeFreeField->add("10 Seconds", 620);
    miJailModeTimeFreeField->add("15 Seconds", 930);
    miJailModeTimeFreeField->add("20 Seconds", 1240);
    miJailModeTimeFreeField->add("25 Seconds", 1550);
    miJailModeTimeFreeField->add("30 Seconds", 1860);
    miJailModeTimeFreeField->add("35 Seconds", 2170);
    miJailModeTimeFreeField->add("40 Seconds", 2480);
    miJailModeTimeFreeField->add("45 Seconds", 2790, false);
    miJailModeTimeFreeField->add("50 Seconds", 3100, false);
    miJailModeTimeFreeField->add("55 Seconds", 3410, false);
    miJailModeTimeFreeField->add("60 Seconds", 3720, false);
    miJailModeTimeFreeField->setOutputPtr(&game_values.gamemodemenusettings.jail.timetofree);
    miJailModeTimeFreeField->setCurrentValue(game_values.gamemodemenusettings.jail.timetofree);

    miJailModeTagFreeField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 240, "Tag Free", 400, 150);
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

    miCoinModePenaltyField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 180, "Penalty", 400, 150);
    miCoinModePenaltyField->add("Off", false);
    miCoinModePenaltyField->add("On", true);
    miCoinModePenaltyField->setOutputPtr(&game_values.gamemodemenusettings.coins.penalty);
    miCoinModePenaltyField->setCurrentValue(game_values.gamemodemenusettings.coins.penalty ? 1 : 0);
    miCoinModePenaltyField->setAutoAdvance(true);

    miCoinModeQuantityField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 220, "Quantity", 400, 150);
    miCoinModeQuantityField->add("1", 1);
    miCoinModeQuantityField->add("2", 2);
    miCoinModeQuantityField->add("3", 3);
    miCoinModeQuantityField->add("4", 4);
    miCoinModeQuantityField->add("5", 5);
    miCoinModeQuantityField->add("6", 6);
    miCoinModeQuantityField->add("7", 7);
    miCoinModeQuantityField->add("8", 8);
    miCoinModeQuantityField->add("9", 9);
    miCoinModeQuantityField->add("10", 10);
    miCoinModeQuantityField->setOutputPtr(&game_values.gamemodemenusettings.coins.quantity);
    miCoinModeQuantityField->setCurrentValue(game_values.gamemodemenusettings.coins.quantity);

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

    miStompModeRateField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 40, "Rate", 400, 180);
    miStompModeRateField->add("Very Slow", 150, false);
    miStompModeRateField->add("Slow", 120);
    miStompModeRateField->add("Moderate", 90);
    miStompModeRateField->add("Fast", 60);
    miStompModeRateField->add("Very Fast", 30, false);
    miStompModeRateField->add("Extremely Fast", 15, false);
    miStompModeRateField->add("Insanely Fast", 5, false);
    miStompModeRateField->setOutputPtr(&game_values.gamemodemenusettings.stomp.rate);
    miStompModeRateField->setCurrentValue(game_values.gamemodemenusettings.stomp.rate);

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

    miEggModeExplosionTimeField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 380, "Explosion Timer", 400, 180);
    miEggModeExplosionTimeField->add("Off", 0);
    miEggModeExplosionTimeField->add("3 Seconds", 3, false);
    miEggModeExplosionTimeField->add("5 Seconds", 5);
    miEggModeExplosionTimeField->add("8 Seconds", 8);
    miEggModeExplosionTimeField->add("10 Seconds", 10);
    miEggModeExplosionTimeField->add("15 Seconds", 15);
    miEggModeExplosionTimeField->add("20 Seconds", 20);
    miEggModeExplosionTimeField->setOutputPtr(&game_values.gamemodemenusettings.egg.explode);
    miEggModeExplosionTimeField->setCurrentValue(game_values.gamemodemenusettings.egg.explode);

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

    miFlagModeTouchReturnField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 160, "Touch Return", 400, 180);
    miFlagModeTouchReturnField->add("Off", false);
    miFlagModeTouchReturnField->add("On", true);
    miFlagModeTouchReturnField->setOutputPtr(&game_values.gamemodemenusettings.flag.touchreturn);
    miFlagModeTouchReturnField->setCurrentValue(game_values.gamemodemenusettings.flag.touchreturn ? 1 : 0);
    miFlagModeTouchReturnField->setAutoAdvance(true);

    miFlagModePointMoveField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 200, "Point Move", 400, 180);
    miFlagModePointMoveField->add("Off", false);
    miFlagModePointMoveField->add("On", true);
    miFlagModePointMoveField->setOutputPtr(&game_values.gamemodemenusettings.flag.pointmove);
    miFlagModePointMoveField->setCurrentValue(game_values.gamemodemenusettings.flag.pointmove ? 1 : 0);
    miFlagModePointMoveField->setAutoAdvance(true);

    miFlagModeAutoReturnField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 240, "Auto Return", 400, 180);
    miFlagModeAutoReturnField->add("None", 0);
    miFlagModeAutoReturnField->add("5 Seconds", 310, false);
    miFlagModeAutoReturnField->add("10 Seconds", 620);
    miFlagModeAutoReturnField->add("15 Seconds", 930, false);
    miFlagModeAutoReturnField->add("20 Seconds", 1240);
    miFlagModeAutoReturnField->add("25 Seconds", 1550, false);
    miFlagModeAutoReturnField->add("30 Seconds", 1860, false);
    miFlagModeAutoReturnField->add("35 Seconds", 2170, false);
    miFlagModeAutoReturnField->add("40 Seconds", 2480, false);
    miFlagModeAutoReturnField->add("45 Seconds", 2790, false);
    miFlagModeAutoReturnField->add("50 Seconds", 3100, false);
    miFlagModeAutoReturnField->add("55 Seconds", 3410, false);
    miFlagModeAutoReturnField->add("60 Seconds", 3720, false);
    miFlagModeAutoReturnField->setOutputPtr(&game_values.gamemodemenusettings.flag.autoreturn);
    miFlagModeAutoReturnField->setCurrentValue(game_values.gamemodemenusettings.flag.autoreturn);

    miFlagModeHomeScoreField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 280, "Need Home", 400, 180);
    miFlagModeHomeScoreField->add("Off", false);
    miFlagModeHomeScoreField->add("On", true);
    miFlagModeHomeScoreField->setOutputPtr(&game_values.gamemodemenusettings.flag.homescore);
    miFlagModeHomeScoreField->setCurrentValue(game_values.gamemodemenusettings.flag.homescore ? 1 : 0);
    miFlagModeHomeScoreField->setAutoAdvance(true);

    miFlagModeCenterFlagField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 320, "Center Flag", 400, 180);
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

    miChickenModeShowTargetField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 200, "Show Target", 400, 180);
    miChickenModeShowTargetField->add("Off", false);
    miChickenModeShowTargetField->add("On", true);
    miChickenModeShowTargetField->setOutputPtr(&game_values.gamemodemenusettings.chicken.usetarget);
    miChickenModeShowTargetField->setCurrentValue(game_values.gamemodemenusettings.chicken.usetarget ? 1 : 0);
    miChickenModeShowTargetField->setAutoAdvance(true);

    miChickenModeGlideField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 240, "Chicken Glide", 400, 180);
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

    miTagModeTagOnTouchField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 220, "Touch Tag", 400, 180);
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

    miStarModeTimeField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 180, "Time", 400, 150);
    miStarModeTimeField->add("5 Seconds", 5, false);
    miStarModeTimeField->add("10 Seconds", 10, false);
    miStarModeTimeField->add("15 Seconds", 15);
    miStarModeTimeField->add("20 Seconds", 20);
    miStarModeTimeField->add("25 Seconds", 25);
    miStarModeTimeField->add("30 Seconds", 30);
    miStarModeTimeField->add("35 Seconds", 35);
    miStarModeTimeField->add("40 Seconds", 40);
    miStarModeTimeField->add("45 Seconds", 45, false);
    miStarModeTimeField->add("50 Seconds", 50, false);
    miStarModeTimeField->add("55 Seconds", 55, false);
    miStarModeTimeField->add("60 Seconds", 60, false);
    miStarModeTimeField->setOutputPtr(&game_values.gamemodemenusettings.star.time);
    miStarModeTimeField->setCurrentValue(game_values.gamemodemenusettings.star.time);

    miStarModeShineField = new MI_SelectField<StarStyle>(&rm->spr_selectfield, 120, 220, "Star Type", 400, 150);
    miStarModeShineField->add("Ztar", StarStyle::Ztar);
    miStarModeShineField->add("Shine", StarStyle::Shine);
    miStarModeShineField->add("Multi Star", StarStyle::Multi);
    miStarModeShineField->add("Random", StarStyle::Random);
    miStarModeShineField->setOutputPtr(&game_values.gamemodemenusettings.star.shine);
    miStarModeShineField->setCurrentValue(game_values.gamemodemenusettings.star.shine);

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

    miDominationModeQuantityField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 120, "Quantity", 400, 180);
    miDominationModeQuantityField->add("1 Base", 1, false);
    miDominationModeQuantityField->add("2 Bases", 2, false);
    miDominationModeQuantityField->add("3 Bases", 3);
    miDominationModeQuantityField->add("4 Bases", 4);
    miDominationModeQuantityField->add("5 Bases", 5);
    miDominationModeQuantityField->add("6 Bases", 6, false);
    miDominationModeQuantityField->add("7 Bases", 7, false);
    miDominationModeQuantityField->add("8 Bases", 8, false);
    miDominationModeQuantityField->add("9 Bases", 9, false);
    miDominationModeQuantityField->add("10 Bases", 10, false);
    miDominationModeQuantityField->add("# Players - 1", 11, false);
    miDominationModeQuantityField->add("# Players", 12, false);
    miDominationModeQuantityField->add("# Players + 1", 13);
    miDominationModeQuantityField->add("# Players + 2", 14);
    miDominationModeQuantityField->add("# Players + 3", 15);
    miDominationModeQuantityField->add("# Players + 4", 16);
    miDominationModeQuantityField->add("# Players + 5", 17);
    miDominationModeQuantityField->add("# Players + 6", 18, false);
    miDominationModeQuantityField->add("2x Players - 3", 19, false);
    miDominationModeQuantityField->add("2x Players - 2", 20, false);
    miDominationModeQuantityField->add("2x Players - 1", 21);
    miDominationModeQuantityField->add("2x Players", 22);
    miDominationModeQuantityField->add("2x Players + 1", 23, false);
    miDominationModeQuantityField->add("2x Players + 2", 24, false);
    miDominationModeQuantityField->setOutputPtr(&game_values.gamemodemenusettings.domination.quantity);
    miDominationModeQuantityField->setCurrentValue(game_values.gamemodemenusettings.domination.quantity);

    miDominationModeRelocateFrequencyField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 160, "Relocate", 400, 180);
    miDominationModeRelocateFrequencyField->add("Never", 0);
    miDominationModeRelocateFrequencyField->add("5 Seconds", 310, false);
    miDominationModeRelocateFrequencyField->add("10 Seconds", 620);
    miDominationModeRelocateFrequencyField->add("15 Seconds", 930);
    miDominationModeRelocateFrequencyField->add("20 Seconds", 1240);
    miDominationModeRelocateFrequencyField->add("30 Seconds", 1860);
    miDominationModeRelocateFrequencyField->add("45 Seconds", 2790);
    miDominationModeRelocateFrequencyField->add("1 Minute", 3720);
    miDominationModeRelocateFrequencyField->add("1.5 Minutes", 5580);
    miDominationModeRelocateFrequencyField->add("2 Minutes", 7440);
    miDominationModeRelocateFrequencyField->add("2.5 Minutes", 9300);
    miDominationModeRelocateFrequencyField->add("3 Minutes", 11160);
    miDominationModeRelocateFrequencyField->setOutputPtr(&game_values.gamemodemenusettings.domination.relocationfrequency);
    miDominationModeRelocateFrequencyField->setCurrentValue(game_values.gamemodemenusettings.domination.relocationfrequency);

    miDominationModeDeathText = new MI_Text("On Death", 120, 210, 0, true, TextAlign::LEFT);

    miDominationModeLoseOnDeathField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 240, "Lose Bases", 400, 180);
    miDominationModeLoseOnDeathField->add("Off", false);
    miDominationModeLoseOnDeathField->add("On", true);
    miDominationModeLoseOnDeathField->setOutputPtr(&game_values.gamemodemenusettings.domination.loseondeath);
    miDominationModeLoseOnDeathField->setCurrentValue(game_values.gamemodemenusettings.domination.loseondeath ? 1 : 0);
    miDominationModeLoseOnDeathField->setAutoAdvance(true);

    miDominationModeRelocateOnDeathField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 280, "Move Bases", 400, 180);
    miDominationModeRelocateOnDeathField->add("Off", false);
    miDominationModeRelocateOnDeathField->add("On", true);
    miDominationModeRelocateOnDeathField->setOutputPtr(&game_values.gamemodemenusettings.domination.relocateondeath);
    miDominationModeRelocateOnDeathField->setCurrentValue(game_values.gamemodemenusettings.domination.relocateondeath ? 1 : 0);
    miDominationModeRelocateOnDeathField->setAutoAdvance(true);

    miDominationModeStealOnDeathField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 320, "Steal Bases", 400, 180);
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

    miKingOfTheHillModeSizeField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 180, "Size", 400, 180);
    miKingOfTheHillModeSizeField->add("2 x 2", 2);
    miKingOfTheHillModeSizeField->add("3 x 3", 3);
    miKingOfTheHillModeSizeField->add("4 x 4", 4);
    miKingOfTheHillModeSizeField->add("5 x 5", 5);
    miKingOfTheHillModeSizeField->add("6 x 6", 6);
    miKingOfTheHillModeSizeField->add("7 x 7", 7);
    miKingOfTheHillModeSizeField->setOutputPtr(&game_values.gamemodemenusettings.kingofthehill.areasize);
    miKingOfTheHillModeSizeField->setCurrentValue(game_values.gamemodemenusettings.kingofthehill.areasize);

    miKingOfTheHillModeRelocateFrequencyField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 220, "Relocate", 400, 180);
    miKingOfTheHillModeRelocateFrequencyField->add("Never", 0);
    miKingOfTheHillModeRelocateFrequencyField->add("5 Seconds", 310, false);
    miKingOfTheHillModeRelocateFrequencyField->add("10 Seconds", 620);
    miKingOfTheHillModeRelocateFrequencyField->add("15 Seconds", 930);
    miKingOfTheHillModeRelocateFrequencyField->add("20 Seconds", 1240);
    miKingOfTheHillModeRelocateFrequencyField->add("30 Seconds", 1860);
    miKingOfTheHillModeRelocateFrequencyField->add("45 Seconds", 2790);
    miKingOfTheHillModeRelocateFrequencyField->add("1 Minute", 3720);
    miKingOfTheHillModeRelocateFrequencyField->add("1.5 Minutes", 5580);
    miKingOfTheHillModeRelocateFrequencyField->add("2 Minutes", 7440);
    miKingOfTheHillModeRelocateFrequencyField->add("2.5 Minutes", 9300);
    miKingOfTheHillModeRelocateFrequencyField->add("3 Minutes", 11160);
    miKingOfTheHillModeRelocateFrequencyField->setOutputPtr(&game_values.gamemodemenusettings.kingofthehill.relocationfrequency);
    miKingOfTheHillModeRelocateFrequencyField->setCurrentValue(game_values.gamemodemenusettings.kingofthehill.relocationfrequency);

    miKingOfTheHillModeMultiplierField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 260, "Max Multiplier", 400, 180);
    miKingOfTheHillModeMultiplierField->add("None", 1);
    miKingOfTheHillModeMultiplierField->add("2", 2);
    miKingOfTheHillModeMultiplierField->add("3", 3);
    miKingOfTheHillModeMultiplierField->add("4", 4);
    miKingOfTheHillModeMultiplierField->add("5", 5);
    miKingOfTheHillModeMultiplierField->setOutputPtr(&game_values.gamemodemenusettings.kingofthehill.maxmultiplier);
    miKingOfTheHillModeMultiplierField->setCurrentValue(game_values.gamemodemenusettings.kingofthehill.maxmultiplier);

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

    miRaceModeQuantityField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 180, "Quantity", 400, 180);
    miRaceModeQuantityField->add("2", 2, false);
    miRaceModeQuantityField->add("3", 3);
    miRaceModeQuantityField->add("4", 4);
    miRaceModeQuantityField->add("5", 5);
    miRaceModeQuantityField->add("6", 6);
    miRaceModeQuantityField->add("7", 7);
    miRaceModeQuantityField->add("8", MAXRACEGOALS);
    miRaceModeQuantityField->setOutputPtr(&game_values.gamemodemenusettings.race.quantity);
    miRaceModeQuantityField->setCurrentValue(game_values.gamemodemenusettings.race.quantity);

    miRaceModeSpeedField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 220, "Speed", 400, 180);
    miRaceModeSpeedField->add("Stationary", 0);
    miRaceModeSpeedField->add("Very Slow", 2);
    miRaceModeSpeedField->add("Slow", 3);
    miRaceModeSpeedField->add("Moderate", 4);
    miRaceModeSpeedField->add("Fast", 6);
    miRaceModeSpeedField->add("Very Fast", 8, false);
    miRaceModeSpeedField->add("Extremely Fast", 15, false);
    miRaceModeSpeedField->add("Insanely Fast", 30, false);
    miRaceModeSpeedField->setOutputPtr(&game_values.gamemodemenusettings.race.speed);
    miRaceModeSpeedField->setCurrentValue(game_values.gamemodemenusettings.race.speed);

    miRaceModePenaltyField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 260, "Penalty", 400, 180);
    miRaceModePenaltyField->add("None", 0);
    miRaceModePenaltyField->add("One Goal", 1);
    miRaceModePenaltyField->add("All Goals", 2);
    miRaceModePenaltyField->setOutputPtr(&game_values.gamemodemenusettings.race.penalty);
    miRaceModePenaltyField->setCurrentValue(game_values.gamemodemenusettings.race.penalty);

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

    miSurvivalModeDensityField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 240, "Density", 400, 180);
    miSurvivalModeDensityField->add("Very Low", 40, false);
    miSurvivalModeDensityField->add("Low", 30);
    miSurvivalModeDensityField->add("Medium", 20);
    miSurvivalModeDensityField->add("High", 15);
    miSurvivalModeDensityField->add("Very High", 10, false);
    miSurvivalModeDensityField->add("Extremely High", 6);
    miSurvivalModeDensityField->add("Insanely High", 2, false);
    miSurvivalModeDensityField->setOutputPtr(&game_values.gamemodemenusettings.survival.density);
    miSurvivalModeDensityField->setCurrentValue(game_values.gamemodemenusettings.survival.density);

    miSurvivalModeSpeedField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 280, "Speed", 400, 180);
    miSurvivalModeSpeedField->add("Very Slow", 2, false);
    miSurvivalModeSpeedField->add("Slow", 3);
    miSurvivalModeSpeedField->add("Moderate", 4);
    miSurvivalModeSpeedField->add("Fast", 6);
    miSurvivalModeSpeedField->add("Very Fast", 8, false);
    miSurvivalModeSpeedField->add("Extremely Fast", 15, false);
    miSurvivalModeSpeedField->add("Insanely Fast", 30, false);
    miSurvivalModeSpeedField->setOutputPtr(&game_values.gamemodemenusettings.survival.speed);
    miSurvivalModeSpeedField->setCurrentValue(game_values.gamemodemenusettings.survival.speed);

    miSurvivalModeShieldField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 320, "Shield", 400, 180);
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
    miGreedModeCoinLife = new MI_SelectField<short>(&rm->spr_selectfield, 120, 160, "Coin Life", 400, 150);
    miGreedModeCoinLife->add("1 Second", 62, false);
    miGreedModeCoinLife->add("2 Seconds", 124, false);
    miGreedModeCoinLife->add("3 Seconds", 186);
    miGreedModeCoinLife->add("4 Seconds", 248);
    miGreedModeCoinLife->add("5 Seconds", 310);
    miGreedModeCoinLife->add("6 Seconds", 372);
    miGreedModeCoinLife->add("7 Seconds", 434);
    miGreedModeCoinLife->add("8 Seconds", 496);
    miGreedModeCoinLife->add("9 Seconds", 558);
    miGreedModeCoinLife->add("10 Seconds", 620);
    miGreedModeCoinLife->add("12 Seconds", 744);
    miGreedModeCoinLife->add("15 Seconds", 930);
    miGreedModeCoinLife->add("18 Seconds", 1116);
    miGreedModeCoinLife->add("20 Seconds", 1240);
    miGreedModeCoinLife->add("25 Seconds", 1550);
    miGreedModeCoinLife->add("30 Seconds", 1860);
    miGreedModeCoinLife->setOutputPtr(&game_values.gamemodemenusettings.greed.coinlife);
    miGreedModeCoinLife->setCurrentValue(game_values.gamemodemenusettings.greed.coinlife);

    miGreedModeOwnCoins = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 200, "Own Coins", 400, 150);
    miGreedModeOwnCoins->add("Yes", true);
    miGreedModeOwnCoins->add("No", false);
    miGreedModeOwnCoins->setOutputPtr(&game_values.gamemodemenusettings.greed.owncoins);
    miGreedModeOwnCoins->setCurrentValue(game_values.gamemodemenusettings.greed.owncoins);
    miGreedModeOwnCoins->setAutoAdvance(true);

    miGreedModeMultiplier = new MI_SelectField<short>(&rm->spr_selectfield, 120, 240, "Multipler", 400, 150);
    miGreedModeMultiplier->add("0.5", 1, false);
    miGreedModeMultiplier->add("1", 2);
    miGreedModeMultiplier->add("1.5", 3);
    miGreedModeMultiplier->add("2", 4);
    miGreedModeMultiplier->add("2.5", 5, false);
    miGreedModeMultiplier->add("3", 6, false);
    miGreedModeMultiplier->setOutputPtr(&game_values.gamemodemenusettings.greed.multiplier);
    miGreedModeMultiplier->setCurrentValue(game_values.gamemodemenusettings.greed.multiplier);

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
    miHealthModeStartLife = new MI_SelectField<short>(&rm->spr_selectfield, 120, 180, "Start Life", 400, 150);
    miHealthModeStartLife->add("2", 2, false);
    miHealthModeStartLife->add("3", 3, false);
    miHealthModeStartLife->add("4", 4);
    miHealthModeStartLife->add("5", 5);
    miHealthModeStartLife->add("6", 6);
    miHealthModeStartLife->add("7", 7, false);
    miHealthModeStartLife->add("8", 8, false);
    miHealthModeStartLife->add("9", 9, false);
    miHealthModeStartLife->add("10", 10, false);
    miHealthModeStartLife->setOutputPtr(&game_values.gamemodemenusettings.health.startlife);
    miHealthModeStartLife->setCurrentValue(game_values.gamemodemenusettings.health.startlife);
    miHealthModeStartLife->allowWrap(false);
    miHealthModeStartLife->setItemChangedCode(MENU_CODE_HEALTH_MODE_START_LIFE_CHANGED);

    miHealthModeMaxLife = new MI_SelectField<short>(&rm->spr_selectfield, 120, 220, "Max Life", 400, 150);
    miHealthModeMaxLife->add("2", 2, false);
    miHealthModeMaxLife->add("3", 3, false);
    miHealthModeMaxLife->add("4", 4, false);
    miHealthModeMaxLife->add("5", 5, false);
    miHealthModeMaxLife->add("6", 6);
    miHealthModeMaxLife->add("7", 7);
    miHealthModeMaxLife->add("8", 8);
    miHealthModeMaxLife->add("9", 9);
    miHealthModeMaxLife->add("10", 10);
    miHealthModeMaxLife->setOutputPtr(&game_values.gamemodemenusettings.health.maxlife);
    miHealthModeMaxLife->setCurrentValue(game_values.gamemodemenusettings.health.maxlife);
    miHealthModeMaxLife->allowWrap(false);
    miHealthModeMaxLife->setItemChangedCode(MENU_CODE_HEALTH_MODE_MAX_LIFE_CHANGED);

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

    miCollectionModeQuantityField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 160, "Limit", 400, 180);
    miCollectionModeQuantityField->add("1 Card", 1);
    miCollectionModeQuantityField->add("2 Cards", 2);
    miCollectionModeQuantityField->add("3 Cards", 3);
    miCollectionModeQuantityField->add("4 Cards", 4, false);
    miCollectionModeQuantityField->add("5 Cards", 5, false);
    miCollectionModeQuantityField->add("# Players - 1", 6);
    miCollectionModeQuantityField->add("# Players", 7);
    miCollectionModeQuantityField->add("# Players + 1", 8, false);
    miCollectionModeQuantityField->add("# Players + 2", 9, false);
    miCollectionModeQuantityField->add("# Players + 3", 10, false);
    miCollectionModeQuantityField->setOutputPtr(&game_values.gamemodemenusettings.collection.quantity);
    miCollectionModeQuantityField->setCurrentValue(game_values.gamemodemenusettings.collection.quantity);

    miCollectionModeRateField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 200, "Rate", 400, 180);
    miCollectionModeRateField->add("Instant", 0);
    miCollectionModeRateField->add("1 Second", 62);
    miCollectionModeRateField->add("2 Seconds", 124);
    miCollectionModeRateField->add("3 Seconds", 186);
    miCollectionModeRateField->add("5 Seconds", 310);
    miCollectionModeRateField->add("10 Seconds", 620, false);
    miCollectionModeRateField->add("15 Seconds", 930, false);
    miCollectionModeRateField->add("20 Seconds", 1240, false);
    miCollectionModeRateField->add("25 Seconds", 1550, false);
    miCollectionModeRateField->add("30 Seconds", 1860, false);
    miCollectionModeRateField->setOutputPtr(&game_values.gamemodemenusettings.collection.rate);
    miCollectionModeRateField->setCurrentValue(game_values.gamemodemenusettings.collection.rate);

    miCollectionModeBankTimeField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 240, "Bank Time", 400, 180);
    miCollectionModeBankTimeField->add("Instant", 0, false);
    miCollectionModeBankTimeField->add("1 Second", 62, false);
    miCollectionModeBankTimeField->add("2 Seconds", 124);
    miCollectionModeBankTimeField->add("3 Seconds", 186);
    miCollectionModeBankTimeField->add("4 Seconds", 248);
    miCollectionModeBankTimeField->add("5 Seconds", 310);
    miCollectionModeBankTimeField->add("6 Seconds", 372, false);
    miCollectionModeBankTimeField->add("7 Seconds", 434, false);
    miCollectionModeBankTimeField->add("8 Seconds", 496, false);
    miCollectionModeBankTimeField->add("9 Seconds", 558, false);
    miCollectionModeBankTimeField->add("10 Seconds", 620, false);
    miCollectionModeBankTimeField->setOutputPtr(&game_values.gamemodemenusettings.collection.banktime);
    miCollectionModeBankTimeField->setCurrentValue(game_values.gamemodemenusettings.collection.banktime);

    miCollectionModeCardLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 280, "Card Life", 400, 180);
    miCollectionModeCardLifeField->add("1 Second", 62, false);
    miCollectionModeCardLifeField->add("2 Seconds", 124, false);
    miCollectionModeCardLifeField->add("3 Seconds", 186);
    miCollectionModeCardLifeField->add("4 Seconds", 248);
    miCollectionModeCardLifeField->add("5 Seconds", 310);
    miCollectionModeCardLifeField->add("6 Seconds", 372);
    miCollectionModeCardLifeField->add("7 Seconds", 434);
    miCollectionModeCardLifeField->add("8 Seconds", 496);
    miCollectionModeCardLifeField->add("9 Seconds", 558);
    miCollectionModeCardLifeField->add("10 Seconds", 620);
    miCollectionModeCardLifeField->add("12 Seconds", 744);
    miCollectionModeCardLifeField->add("15 Seconds", 930);
    miCollectionModeCardLifeField->add("18 Seconds", 1116);
    miCollectionModeCardLifeField->add("20 Seconds", 1240);
    miCollectionModeCardLifeField->add("25 Seconds", 1550);
    miCollectionModeCardLifeField->add("30 Seconds", 1860);
    miCollectionModeCardLifeField->setOutputPtr(&game_values.gamemodemenusettings.collection.cardlife);
    miCollectionModeCardLifeField->setCurrentValue(game_values.gamemodemenusettings.collection.cardlife);

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

    miChaseModeSpeedField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 160, "Speed", 400, 180);
    miChaseModeSpeedField->add("Very Slow", 3, false);
    miChaseModeSpeedField->add("Slow", 4);
    miChaseModeSpeedField->add("Moderate", 5);
    miChaseModeSpeedField->add("Fast", 6);
    miChaseModeSpeedField->add("Very Fast", 7);
    miChaseModeSpeedField->add("Extremely Fast", 8, false);
    miChaseModeSpeedField->add("Insanely Fast", 10, false);
    miChaseModeSpeedField->setOutputPtr(&game_values.gamemodemenusettings.chase.phantospeed);
    miChaseModeSpeedField->setCurrentValue(game_values.gamemodemenusettings.chase.phantospeed);

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

    miShyGuyTagModeTagOnSuicideField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 180, "Suicide Tag", 400, 180);
    miShyGuyTagModeTagOnSuicideField->add("Off", false);
    miShyGuyTagModeTagOnSuicideField->add("On", true);
    miShyGuyTagModeTagOnSuicideField->setOutputPtr(&game_values.gamemodemenusettings.shyguytag.tagonsuicide);
    miShyGuyTagModeTagOnSuicideField->setCurrentValue(game_values.gamemodemenusettings.shyguytag.tagonsuicide ? 1 : 0);
    miShyGuyTagModeTagOnSuicideField->setAutoAdvance(true);

    miShyGuyTagModeTagOnStompField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 220, "Tag Transfer", 400, 180);
    miShyGuyTagModeTagOnStompField->add("Touch Only", 0);
    miShyGuyTagModeTagOnStompField->add("Kills Only", 1);
    miShyGuyTagModeTagOnStompField->add("Touch and Kills", 2);
    miShyGuyTagModeTagOnStompField->setOutputPtr(&game_values.gamemodemenusettings.shyguytag.tagtransfer);
    miShyGuyTagModeTagOnStompField->setCurrentValue(game_values.gamemodemenusettings.shyguytag.tagtransfer);

    miShyGuyTagModeFreeTimeField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 260, "Free Time", 400, 180);
    miShyGuyTagModeFreeTimeField->add("Instant", 0);
    miShyGuyTagModeFreeTimeField->add("1 Second", 1);
    miShyGuyTagModeFreeTimeField->add("2 Seconds", 2);
    miShyGuyTagModeFreeTimeField->add("3 Seconds", 3);
    miShyGuyTagModeFreeTimeField->add("4 Seconds", 4);
    miShyGuyTagModeFreeTimeField->add("5 Seconds", 5);
    miShyGuyTagModeFreeTimeField->add("6 Seconds", 6);
    miShyGuyTagModeFreeTimeField->add("7 Seconds", 7);
    miShyGuyTagModeFreeTimeField->add("8 Seconds", 8);
    miShyGuyTagModeFreeTimeField->add("9 Seconds", 9);
    miShyGuyTagModeFreeTimeField->add("10 Seconds", 10);
    miShyGuyTagModeFreeTimeField->setOutputPtr(&game_values.gamemodemenusettings.shyguytag.freetime);
    miShyGuyTagModeFreeTimeField->setCurrentValue(game_values.gamemodemenusettings.shyguytag.freetime);

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

    miBossModeTypeField = new MI_SelectField<Boss>(&rm->spr_selectfield, 120, 180, "Type", 400, 180);
    miBossModeTypeField->add("Hammer", Boss::Hammer);
    miBossModeTypeField->add("Bomb", Boss::Bomb);
    miBossModeTypeField->add("Fire", Boss::Fire);
    miBossModeTypeField->setOutputPtr(&game_values.gamemodemenusettings.boss.bosstype);
    miBossModeTypeField->setCurrentValue(game_values.gamemodemenusettings.boss.bosstype);

    miBossModeDifficultyField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 220, "Difficulty", 400, 180);
    miBossModeDifficultyField->add("Very Easy", 0, false);
    miBossModeDifficultyField->add("Easy", 1);
    miBossModeDifficultyField->add("Moderate", 2);
    miBossModeDifficultyField->add("Hard", 3);
    miBossModeDifficultyField->add("Very Hard", 4, false);
    miBossModeDifficultyField->setOutputPtr(&game_values.gamemodemenusettings.boss.difficulty);
    miBossModeDifficultyField->setCurrentValue(game_values.gamemodemenusettings.boss.difficulty);

    miBossModeHitPointsField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 260, "Health", 400, 180);
    miBossModeHitPointsField->add("1", 1, false);
    miBossModeHitPointsField->add("2", 2, false);
    miBossModeHitPointsField->add("3", 3);
    miBossModeHitPointsField->add("4", 4);
    miBossModeHitPointsField->add("5", 5);
    miBossModeHitPointsField->add("6", 6);
    miBossModeHitPointsField->add("7", 7);
    miBossModeHitPointsField->add("8", 8);
    miBossModeHitPointsField->add("9", 9, false);
    miBossModeHitPointsField->add("10", 10, false);
    miBossModeHitPointsField->add("11", 11, false);
    miBossModeHitPointsField->add("12", 12, false);
    miBossModeHitPointsField->add("13", 13, false);
    miBossModeHitPointsField->add("14", 14, false);
    miBossModeHitPointsField->add("15", 15, false);
    miBossModeHitPointsField->add("16", 16, false);
    miBossModeHitPointsField->add("17", 17, false);
    miBossModeHitPointsField->add("18", 18, false);
    miBossModeHitPointsField->add("19", 19, false);
    miBossModeHitPointsField->add("20", 20, false);
    miBossModeHitPointsField->setOutputPtr(&game_values.gamemodemenusettings.boss.hitpoints);
    miBossModeHitPointsField->setCurrentValue(game_values.gamemodemenusettings.boss.hitpoints);

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
        game_values.gamemodesettings.classic.style = miClassicModeStyleField->randomValue();
        game_values.gamemodesettings.classic.scoring = miClassicModeScoringField->randomValue();
    } else if (iMode == game_mode_frag) { // frag
        game_values.gamemodesettings.frag.style = miFragModeStyleField->randomValue();
        game_values.gamemodesettings.frag.scoring = miFragModeScoringField->randomValue();
    } else if (iMode == game_mode_timelimit) { // time
        game_values.gamemodesettings.time.style = miTimeLimitModeStyleField->randomValue();
        game_values.gamemodesettings.time.scoring = miTimeLimitModeScoringField->randomValue();
        game_values.gamemodesettings.time.percentextratime = miTimeLimitModePercentExtraTime->currentValue();
    } else if (iMode == game_mode_jail) { // jail
        game_values.gamemodesettings.jail.style = miJailModeStyleField->randomValue();
        game_values.gamemodesettings.jail.timetofree = miJailModeTimeFreeField->randomValue();
        game_values.gamemodesettings.jail.tagfree = miJailModeTagFreeField->randomValue();
        game_values.gamemodesettings.jail.percentkey = miJailModeJailKeyField->currentValue();
    } else if (iMode == game_mode_coins) { // coins
        game_values.gamemodesettings.coins.penalty = miCoinModePenaltyField->randomValue();
        game_values.gamemodesettings.coins.quantity = miCoinModeQuantityField->randomValue();
        game_values.gamemodesettings.coins.percentextracoin = miCoinModePercentExtraCoin->currentValue();
    } else if (iMode == game_mode_stomp) { // stomp
        game_values.gamemodesettings.stomp.rate = miStompModeRateField->randomValue();

        for (short iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
            game_values.gamemodesettings.stomp.enemyweight[iEnemy] = miStompModeEnemySlider[iEnemy]->currentValue();
    } else if (iMode == game_mode_eggs) { // egg
        for (int iEgg = 0; iEgg < 4; iEgg++)
            game_values.gamemodesettings.egg.eggs[iEgg] = miEggModeEggQuantityField[iEgg]->currentValue();

        for (int iYoshi = 0; iYoshi < 4; iYoshi++)
            game_values.gamemodesettings.egg.yoshis[iYoshi] = miEggModeYoshiQuantityField[iYoshi]->currentValue();

        game_values.gamemodesettings.egg.explode = miEggModeExplosionTimeField->randomValue();
    } else if (iMode == game_mode_ctf) { // capture the flag
        game_values.gamemodesettings.flag.speed = miFlagModeSpeedField->currentValue();
        game_values.gamemodesettings.flag.touchreturn = miFlagModeTouchReturnField->randomValue();
        game_values.gamemodesettings.flag.pointmove = miFlagModePointMoveField->randomValue();
        game_values.gamemodesettings.flag.autoreturn = miFlagModeAutoReturnField->randomValue();
        game_values.gamemodesettings.flag.homescore = miFlagModeHomeScoreField->randomValue();
        game_values.gamemodesettings.flag.centerflag = miFlagModeCenterFlagField->randomValue();
    } else if (iMode == game_mode_chicken) { // chicken
        game_values.gamemodesettings.chicken.usetarget = miChickenModeShowTargetField->randomValue();
        game_values.gamemodesettings.chicken.glide = miChickenModeGlideField->randomValue();
    } else if (iMode == game_mode_tag) { // tag
        game_values.gamemodesettings.tag.tagontouch = miTagModeTagOnTouchField->randomValue();
    } else if (iMode == game_mode_star) { // star
        game_values.gamemodesettings.star.time = miStarModeTimeField->randomValue();
        game_values.gamemodesettings.star.shine = miStarModeShineField->randomValue();
        game_values.gamemodesettings.star.percentextratime = miStarModePercentExtraTime->currentValue();
    } else if (iMode == game_mode_domination) { // domination
        game_values.gamemodesettings.domination.quantity = miDominationModeQuantityField->randomValue();
        game_values.gamemodesettings.domination.loseondeath = miDominationModeLoseOnDeathField->randomValue();
        game_values.gamemodesettings.domination.relocateondeath = miDominationModeRelocateOnDeathField->randomValue();
        game_values.gamemodesettings.domination.stealondeath = miDominationModeStealOnDeathField->randomValue();
        game_values.gamemodesettings.domination.relocationfrequency = miDominationModeRelocateFrequencyField->randomValue();
    } else if (iMode == game_mode_koth) { // king of the hill
        game_values.gamemodesettings.kingofthehill.areasize = miKingOfTheHillModeSizeField->randomValue();
        game_values.gamemodesettings.kingofthehill.relocationfrequency = miKingOfTheHillModeRelocateFrequencyField->randomValue();
        game_values.gamemodesettings.kingofthehill.maxmultiplier = miKingOfTheHillModeMultiplierField->randomValue();
    } else if (iMode == game_mode_race) { // race
        game_values.gamemodesettings.race.quantity = miRaceModeQuantityField->randomValue();
        game_values.gamemodesettings.race.speed = miRaceModeSpeedField->randomValue();
        game_values.gamemodesettings.race.penalty = miRaceModePenaltyField->randomValue();
    } else if (iMode == game_mode_frenzy) { // frenzy
        miFrenzyModeOptions->SetRandomGameModeSettings();
    } else if (iMode == game_mode_survival) { // survival
        game_values.gamemodesettings.survival.density = miSurvivalModeDensityField->randomValue();
        game_values.gamemodesettings.survival.speed = miSurvivalModeSpeedField->randomValue();
        game_values.gamemodesettings.survival.shield = miSurvivalModeShieldField->randomValue();

        for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
            game_values.gamemodesettings.survival.enemyweight[iEnemy] = miSurvivalModeEnemySlider[iEnemy]->currentValue();
    } else if (iMode == game_mode_greed) { // greed
        game_values.gamemodesettings.greed.coinlife = miGreedModeCoinLife->randomValue();
        game_values.gamemodesettings.greed.owncoins = miGreedModeOwnCoins->randomValue();
        game_values.gamemodesettings.greed.multiplier = miGreedModeMultiplier->randomValue();
        game_values.gamemodesettings.greed.percentextracoin = miGreedModePercentExtraCoin->currentValue();
    } else if (iMode == game_mode_health) { // health
        game_values.gamemodesettings.health.startlife = miHealthModeStartLife->randomValue();
        game_values.gamemodesettings.health.maxlife = miHealthModeMaxLife->randomValue();
        game_values.gamemodesettings.health.percentextralife = miHealthModePercentExtraLife->currentValue();
    } else if (iMode == game_mode_collection) { // card collection
        game_values.gamemodesettings.collection.quantity = miCollectionModeQuantityField->randomValue();
        game_values.gamemodesettings.collection.rate = miCollectionModeRateField->randomValue();
        game_values.gamemodesettings.collection.banktime = miCollectionModeBankTimeField->randomValue();
        game_values.gamemodesettings.collection.cardlife = miCollectionModeCardLifeField->randomValue();
    } else if (iMode == game_mode_chase) { // chase (phanto)
        game_values.gamemodesettings.chase.phantospeed = miChaseModeSpeedField->randomValue();

        for (short iPhanto = 0; iPhanto < 3; iPhanto++)
            game_values.gamemodesettings.chase.phantoquantity[iPhanto] = miChaseModeQuantitySlider[iPhanto]->currentValue();
    } else if (iMode == game_mode_shyguytag) { // shyguy tag
        game_values.gamemodesettings.shyguytag.tagonsuicide = miShyGuyTagModeTagOnSuicideField->randomValue();
        game_values.gamemodesettings.shyguytag.tagtransfer = miShyGuyTagModeTagOnStompField->randomValue();
        game_values.gamemodesettings.shyguytag.freetime = miShyGuyTagModeTagOnStompField->randomValue();
    } else if (iMode == game_mode_boss_minigame) { // boss
        game_values.gamemodesettings.boss.bosstype = miBossModeTypeField->randomValue();
        game_values.gamemodesettings.boss.difficulty = miBossModeDifficultyField->randomValue();
        game_values.gamemodesettings.boss.hitpoints = miBossModeHitPointsField->randomValue();
    }
}

void UI_ModeOptionsMenu::HealthModeStartLifeChanged()
{
    short iMaxLife = miHealthModeMaxLife->currentValue();
    if (miHealthModeStartLife->currentValue() > iMaxLife) {
        miHealthModeStartLife->setCurrentValue(iMaxLife);
    }
}

void UI_ModeOptionsMenu::HealthModeMaxLifeChanged()
{
    short iStartLife = miHealthModeStartLife->currentValue();
    if (miHealthModeMaxLife->currentValue() < iStartLife) {
        miHealthModeMaxLife->setCurrentValue(iStartLife);
    }
}

void UI_ModeOptionsMenu::Refresh()
{
    for (short iMode = 0; iMode < GAMEMODE_LAST; iMode++) {
        mModeSettingsMenu[iMode].Refresh();
    }

    mBossSettingsMenu.Refresh();
}
