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
    miRespawnField = new MI_SelectField(&rm->spr_selectfield, 70, 40, "Respawn Time", 500, 220);
    miRespawnField->Add("Instant", 0);
    miRespawnField->Add("0.5 Seconds", 1);
    miRespawnField->Add("1.0 Seconds", 2);
    miRespawnField->Add("1.5 Seconds", 3);
    miRespawnField->Add("2.0 Seconds", 4);
    miRespawnField->Add("2.5 Seconds", 5);
    miRespawnField->Add("3.0 Seconds", 6);
    miRespawnField->Add("3.5 Seconds", 7);
    miRespawnField->Add("4.0 Seconds", 8);
    miRespawnField->Add("4.5 Seconds", 9);
    miRespawnField->Add("5.0 Seconds", 10);
    miRespawnField->Add("5.5 Seconds", 11);
    miRespawnField->Add("6.0 Seconds", 12);
    miRespawnField->Add("6.5 Seconds", 13);
    miRespawnField->Add("7.0 Seconds", 14);
    miRespawnField->Add("7.5 Seconds", 15);
    miRespawnField->Add("8.0 Seconds", 16);
    miRespawnField->Add("8.5 Seconds", 17);
    miRespawnField->Add("9.0 Seconds", 18);
    miRespawnField->Add("9.5 Seconds", 19);
    miRespawnField->Add("10.0 Seconds", 20);
    miRespawnField->SetData(&game_values.respawn, NULL, NULL);
    miRespawnField->SetKey(game_values.respawn);

    miShieldStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 80, "Shield Style", 500, 220);
    miShieldStyleField->Add("No Shield", 0);
    miShieldStyleField->Add("Soft", 1);
    miShieldStyleField->Add("Soft with Stomp", 2);
    miShieldStyleField->Add("Hard", 3);
    miShieldStyleField->SetData(&game_values.shieldstyle, NULL, NULL);
    miShieldStyleField->SetKey(game_values.shieldstyle);

    miShieldTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 120, "Shield Time", 500, 220);
    miShieldTimeField->Add("0.5 Seconds", 31);
    miShieldTimeField->Add("1.0 Seconds", 62);
    miShieldTimeField->Add("1.5 Seconds", 93);
    miShieldTimeField->Add("2.0 Seconds", 124);
    miShieldTimeField->Add("2.5 Seconds", 155);
    miShieldTimeField->Add("3.0 Seconds", 186);
    miShieldTimeField->Add("3.5 Seconds", 217);
    miShieldTimeField->Add("4.0 Seconds", 248);
    miShieldTimeField->Add("4.5 Seconds", 279);
    miShieldTimeField->Add("5.0 Seconds", 310);
    miShieldTimeField->SetData(&game_values.shieldtime, NULL, NULL);
    miShieldTimeField->SetKey(game_values.shieldtime);

    miBoundsTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 160, "Bounds Time", 500, 220);
    miBoundsTimeField->Add("Infinite", 0);
    miBoundsTimeField->Add("1 Second", 1);
    miBoundsTimeField->Add("2 Seconds", 2);
    miBoundsTimeField->Add("3 Seconds", 3);
    miBoundsTimeField->Add("4 Seconds", 4);
    miBoundsTimeField->Add("5 Seconds", 5);
    miBoundsTimeField->Add("6 Seconds", 6);
    miBoundsTimeField->Add("7 Seconds", 7);
    miBoundsTimeField->Add("8 Seconds", 8);
    miBoundsTimeField->Add("9 Seconds", 9);
    miBoundsTimeField->Add("10 Seconds", 10);
    miBoundsTimeField->SetData(&game_values.outofboundstime, NULL, NULL);
    miBoundsTimeField->SetKey(game_values.outofboundstime);

    miSuicideTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 200, "Suicide Time", 500, 220);
    miSuicideTimeField->Add("Off", 0);
    miSuicideTimeField->Add("3 Seconds", 186);
    miSuicideTimeField->Add("5 Seconds", 310);
    miSuicideTimeField->Add("8 Seconds", 496);
    miSuicideTimeField->Add("10 Seconds", 620);
    miSuicideTimeField->Add("15 Seconds", 930);
    miSuicideTimeField->Add("20 Seconds", 1240);
    miSuicideTimeField->SetData(&game_values.suicidetime, NULL, NULL);
    miSuicideTimeField->SetKey(game_values.suicidetime);

    miWarpLockStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 240, "Warp Lock Style", 500, 220);
    miWarpLockStyleField->Add("Entrance Only", 0);
    miWarpLockStyleField->Add("Exit Only", 1);
    miWarpLockStyleField->Add("Entrance and Exit", 2);
    miWarpLockStyleField->Add("Entire Connection", 3);
    miWarpLockStyleField->Add("All Warps", 4);
    miWarpLockStyleField->SetData(&game_values.warplockstyle, NULL, NULL);
    miWarpLockStyleField->SetKey(game_values.warplockstyle);

    miWarpLockTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 280, "Warp Lock Time", 500, 220);
    miWarpLockTimeField->Add("Off", 0);
    miWarpLockTimeField->Add("1 Second", 62);
    miWarpLockTimeField->Add("2 Seconds", 124);
    miWarpLockTimeField->Add("3 Seconds", 186);
    miWarpLockTimeField->Add("4 Seconds", 248);
    miWarpLockTimeField->Add("5 Seconds", 310);
    miWarpLockTimeField->Add("6 Seconds", 372);
    miWarpLockTimeField->Add("7 Seconds", 434);
    miWarpLockTimeField->Add("8 Seconds", 496);
    miWarpLockTimeField->Add("9 Seconds", 558);
    miWarpLockTimeField->Add("10 Seconds", 620);
    miWarpLockTimeField->SetData(&game_values.warplocktime, NULL, NULL);
    miWarpLockTimeField->SetKey(game_values.warplocktime);

    miBotsField = new MI_SelectField(&rm->spr_selectfield, 70, 320, "Bot Difficulty", 500, 220);
    miBotsField->Add("Very Easy", 0);
    miBotsField->Add("Easy", 1);
    miBotsField->Add("Moderate", 2);
    miBotsField->Add("Hard", 3);
    miBotsField->Add("Very Hard", 4);
    miBotsField->SetData(&game_values.cpudifficulty, NULL, NULL);
    miBotsField->SetKey(game_values.cpudifficulty);

    miPointSpeedField = new MI_SelectField(&rm->spr_selectfield, 70, 360, "Point Speed", 500, 220);
    miPointSpeedField->Add("Very Slow", 60);
    miPointSpeedField->Add("Slow", 40);
    miPointSpeedField->Add("Moderate", 20);
    miPointSpeedField->Add("Fast", 10);
    miPointSpeedField->Add("Very Fast", 5);
    miPointSpeedField->SetData(&game_values.pointspeed, NULL, NULL);
    miPointSpeedField->SetKey(game_values.pointspeed);

    miSecretsField = new MI_SelectField(&rm->spr_selectfield, 70, 400, "Secrets", 500, 220);
    miSecretsField->Add("Off", 0);
    miSecretsField->Add("On", 1, "", true, false);
    miSecretsField->SetData(NULL, NULL, &game_values.secretsenabled);
    miSecretsField->SetKey(game_values.secretsenabled ? 1 : 0);

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
