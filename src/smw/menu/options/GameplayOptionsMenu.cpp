#include "GameplayOptionsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;
extern CGameValues game_values;


UI_GameplayOptionsMenu::UI_GameplayOptionsMenu()
    : UI_Menu()
{
    miRespawnField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 40, "Respawn Time", 500, 220);
    miRespawnField->add("Instant", 0);
    miRespawnField->add("0.5 Seconds", 1);
    miRespawnField->add("1.0 Seconds", 2);
    miRespawnField->add("1.5 Seconds", 3);
    miRespawnField->add("2.0 Seconds", 4);
    miRespawnField->add("2.5 Seconds", 5);
    miRespawnField->add("3.0 Seconds", 6);
    miRespawnField->add("3.5 Seconds", 7);
    miRespawnField->add("4.0 Seconds", 8);
    miRespawnField->add("4.5 Seconds", 9);
    miRespawnField->add("5.0 Seconds", 10);
    miRespawnField->add("5.5 Seconds", 11);
    miRespawnField->add("6.0 Seconds", 12);
    miRespawnField->add("6.5 Seconds", 13);
    miRespawnField->add("7.0 Seconds", 14);
    miRespawnField->add("7.5 Seconds", 15);
    miRespawnField->add("8.0 Seconds", 16);
    miRespawnField->add("8.5 Seconds", 17);
    miRespawnField->add("9.0 Seconds", 18);
    miRespawnField->add("9.5 Seconds", 19);
    miRespawnField->add("10.0 Seconds", 20);
    miRespawnField->setOutputPtr(&game_values.respawn);
    miRespawnField->setCurrentValue(game_values.respawn);

    miShieldStyleField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 80, "Shield Style", 500, 220);
    miShieldStyleField->add("No Shield", 0);
    miShieldStyleField->add("Soft", 1);
    miShieldStyleField->add("Soft with Stomp", 2);
    miShieldStyleField->add("Hard", 3);
    miShieldStyleField->setOutputPtr(&game_values.shieldstyle);
    miShieldStyleField->setCurrentValue(game_values.shieldstyle);

    miShieldTimeField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 120, "Shield Time", 500, 220);
    miShieldTimeField->add("0.5 Seconds", 31);
    miShieldTimeField->add("1.0 Seconds", 62);
    miShieldTimeField->add("1.5 Seconds", 93);
    miShieldTimeField->add("2.0 Seconds", 124);
    miShieldTimeField->add("2.5 Seconds", 155);
    miShieldTimeField->add("3.0 Seconds", 186);
    miShieldTimeField->add("3.5 Seconds", 217);
    miShieldTimeField->add("4.0 Seconds", 248);
    miShieldTimeField->add("4.5 Seconds", 279);
    miShieldTimeField->add("5.0 Seconds", 310);
    miShieldTimeField->setOutputPtr(&game_values.shieldtime);
    miShieldTimeField->setCurrentValue(game_values.shieldtime);

    miBoundsTimeField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 160, "Bounds Time", 500, 220);
    miBoundsTimeField->add("Infinite", 0);
    miBoundsTimeField->add("1 Second", 1);
    miBoundsTimeField->add("2 Seconds", 2);
    miBoundsTimeField->add("3 Seconds", 3);
    miBoundsTimeField->add("4 Seconds", 4);
    miBoundsTimeField->add("5 Seconds", 5);
    miBoundsTimeField->add("6 Seconds", 6);
    miBoundsTimeField->add("7 Seconds", 7);
    miBoundsTimeField->add("8 Seconds", 8);
    miBoundsTimeField->add("9 Seconds", 9);
    miBoundsTimeField->add("10 Seconds", 10);
    miBoundsTimeField->setOutputPtr(&game_values.outofboundstime);
    miBoundsTimeField->setCurrentValue(game_values.outofboundstime);

    miSuicideTimeField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 200, "Suicide Time", 500, 220);
    miSuicideTimeField->add("Off", 0);
    miSuicideTimeField->add("3 Seconds", 186);
    miSuicideTimeField->add("5 Seconds", 310);
    miSuicideTimeField->add("8 Seconds", 496);
    miSuicideTimeField->add("10 Seconds", 620);
    miSuicideTimeField->add("15 Seconds", 930);
    miSuicideTimeField->add("20 Seconds", 1240);
    miSuicideTimeField->setOutputPtr(&game_values.suicidetime);
    miSuicideTimeField->setCurrentValue(game_values.suicidetime);

    miWarpLockStyleField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 240, "Warp Lock Style", 500, 220);
    miWarpLockStyleField->add("Entrance Only", 0);
    miWarpLockStyleField->add("Exit Only", 1);
    miWarpLockStyleField->add("Entrance and Exit", 2);
    miWarpLockStyleField->add("Entire Connection", 3);
    miWarpLockStyleField->add("All Warps", 4);
    miWarpLockStyleField->setOutputPtr(&game_values.warplockstyle);
    miWarpLockStyleField->setCurrentValue(game_values.warplockstyle);

    miWarpLockTimeField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 280, "Warp Lock Time", 500, 220);
    miWarpLockTimeField->add("Off", 0);
    miWarpLockTimeField->add("1 Second", 62);
    miWarpLockTimeField->add("2 Seconds", 124);
    miWarpLockTimeField->add("3 Seconds", 186);
    miWarpLockTimeField->add("4 Seconds", 248);
    miWarpLockTimeField->add("5 Seconds", 310);
    miWarpLockTimeField->add("6 Seconds", 372);
    miWarpLockTimeField->add("7 Seconds", 434);
    miWarpLockTimeField->add("8 Seconds", 496);
    miWarpLockTimeField->add("9 Seconds", 558);
    miWarpLockTimeField->add("10 Seconds", 620);
    miWarpLockTimeField->setOutputPtr(&game_values.warplocktime);
    miWarpLockTimeField->setCurrentValue(game_values.warplocktime);

    miBotsField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 320, "Bot Difficulty", 500, 220);
    miBotsField->add("Very Easy", 0);
    miBotsField->add("Easy", 1);
    miBotsField->add("Moderate", 2);
    miBotsField->add("Hard", 3);
    miBotsField->add("Very Hard", 4);
    miBotsField->setOutputPtr(&game_values.cpudifficulty);
    miBotsField->setCurrentValue(game_values.cpudifficulty);

    miPointSpeedField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 360, "Point Speed", 500, 220);
    miPointSpeedField->add("Very Slow", 60);
    miPointSpeedField->add("Slow", 40);
    miPointSpeedField->add("Moderate", 20);
    miPointSpeedField->add("Fast", 10);
    miPointSpeedField->add("Very Fast", 5);
    miPointSpeedField->setOutputPtr(&game_values.pointspeed);
    miPointSpeedField->setCurrentValue(game_values.pointspeed);

    miSecretsField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 70, 400, "Secrets", 500, 220);
    miSecretsField->add("Off", false);
    miSecretsField->add("On", true);
    miSecretsField->setOutputPtr(&game_values.secretsenabled);
    miSecretsField->setCurrentValue(game_values.secretsenabled ? 1 : 0);

    miGameplayOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miGameplayOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miGameplayOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miGameplayOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miGameplayOptionsMenuHeaderText = new MI_HeaderText("Gameplay Options Menu", 320, 5);

    AddControl(miRespawnField, miGameplayOptionsMenuBackButton, miShieldStyleField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miShieldStyleField, miRespawnField, miShieldTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miShieldTimeField, miShieldStyleField, miBoundsTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miBoundsTimeField, miShieldTimeField, miSuicideTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miSuicideTimeField, miBoundsTimeField, miWarpLockStyleField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miWarpLockStyleField, miSuicideTimeField, miWarpLockTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miWarpLockTimeField, miWarpLockStyleField, miBotsField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miBotsField, miWarpLockTimeField, miPointSpeedField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miPointSpeedField, miBotsField, miSecretsField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miSecretsField, miPointSpeedField, miGameplayOptionsMenuBackButton, NULL, miGameplayOptionsMenuBackButton);

    AddControl(miGameplayOptionsMenuBackButton, miSecretsField, miRespawnField, miSecretsField, NULL);

    AddNonControl(miGameplayOptionsMenuLeftHeaderBar);
    AddNonControl(miGameplayOptionsMenuRightHeaderBar);
    AddNonControl(miGameplayOptionsMenuHeaderText);

    SetHeadControl(miRespawnField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};
