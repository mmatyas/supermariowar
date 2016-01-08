#include "GameplayOptionsMenu.h"

#include "uicontrol.h"
#include "ui/MI_SelectField.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_GameplayOptionsMenu::UI_GameplayOptionsMenu() : UI_Menu()
{
	miRespawnField = new MI_SelectField(&rm->spr_selectfield, 70, 60, "Respawn Time", 500, 220);
    miRespawnField->Add("Instant", 0, "", false, false);
    miRespawnField->Add("0.5 Seconds", 1, "", false, false);
    miRespawnField->Add("1.0 Seconds", 2, "", false, false);
    miRespawnField->Add("1.5 Seconds", 3, "", false, false);
    miRespawnField->Add("2.0 Seconds", 4, "", false, false);
    miRespawnField->Add("2.5 Seconds", 5, "", false, false);
    miRespawnField->Add("3.0 Seconds", 6, "", false, false);
    miRespawnField->Add("3.5 Seconds", 7, "", false, false);
    miRespawnField->Add("4.0 Seconds", 8, "", false, false);
    miRespawnField->Add("4.5 Seconds", 9, "", false, false);
    miRespawnField->Add("5.0 Seconds", 10, "", false, false);
    miRespawnField->Add("5.5 Seconds", 11, "", false, false);
    miRespawnField->Add("6.0 Seconds", 12, "", false, false);
    miRespawnField->Add("6.5 Seconds", 13, "", false, false);
    miRespawnField->Add("7.0 Seconds", 14, "", false, false);
    miRespawnField->Add("7.5 Seconds", 15, "", false, false);
    miRespawnField->Add("8.0 Seconds", 16, "", false, false);
    miRespawnField->Add("8.5 Seconds", 17, "", false, false);
    miRespawnField->Add("9.0 Seconds", 18, "", false, false);
    miRespawnField->Add("9.5 Seconds", 19, "", false, false);
    miRespawnField->Add("10.0 Seconds", 20, "", false, false);
    miRespawnField->SetData(&game_values.respawn, NULL, NULL);
    miRespawnField->SetKey(game_values.respawn);

    miShieldStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 100, "Shield Style", 500, 220);
    miShieldStyleField->Add("No Shield", 0, "", false, false);
    miShieldStyleField->Add("Soft", 1, "", false, false);
    miShieldStyleField->Add("Soft with Stomp", 2, "", false, false);
    miShieldStyleField->Add("Hard", 3, "", false, false);
    miShieldStyleField->SetData(&game_values.shieldstyle, NULL, NULL);
    miShieldStyleField->SetKey(game_values.shieldstyle);

    miShieldTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 140, "Shield Time", 500, 220);
    miShieldTimeField->Add("0.5 Seconds", 31, "", false, false);
    miShieldTimeField->Add("1.0 Seconds", 62, "", false, false);
    miShieldTimeField->Add("1.5 Seconds", 93, "", false, false);
    miShieldTimeField->Add("2.0 Seconds", 124, "", false, false);
    miShieldTimeField->Add("2.5 Seconds", 155, "", false, false);
    miShieldTimeField->Add("3.0 Seconds", 186, "", false, false);
    miShieldTimeField->Add("3.5 Seconds", 217, "", false, false);
    miShieldTimeField->Add("4.0 Seconds", 248, "", false, false);
    miShieldTimeField->Add("4.5 Seconds", 279, "", false, false);
    miShieldTimeField->Add("5.0 Seconds", 310, "", false, false);
    miShieldTimeField->SetData(&game_values.shieldtime, NULL, NULL);
    miShieldTimeField->SetKey(game_values.shieldtime);

    miBoundsTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 180, "Bounds Time", 500, 220);
    miBoundsTimeField->Add("Infinite", 0, "", false, false);
    miBoundsTimeField->Add("1 Second", 1, "", false, false);
    miBoundsTimeField->Add("2 Seconds", 2, "", false, false);
    miBoundsTimeField->Add("3 Seconds", 3, "", false, false);
    miBoundsTimeField->Add("4 Seconds", 4, "", false, false);
    miBoundsTimeField->Add("5 Seconds", 5, "", false, false);
    miBoundsTimeField->Add("6 Seconds", 6, "", false, false);
    miBoundsTimeField->Add("7 Seconds", 7, "", false, false);
    miBoundsTimeField->Add("8 Seconds", 8, "", false, false);
    miBoundsTimeField->Add("9 Seconds", 9, "", false, false);
    miBoundsTimeField->Add("10 Seconds", 10, "", false, false);
    miBoundsTimeField->SetData(&game_values.outofboundstime, NULL, NULL);
    miBoundsTimeField->SetKey(game_values.outofboundstime);

    miSuicideTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 220, "Suicide Time", 500, 220);
    miSuicideTimeField->Add("Off", 0, "", false, false);
    miSuicideTimeField->Add("3 Seconds", 186, "", false, false);
    miSuicideTimeField->Add("5 Seconds", 310, "", false, false);
    miSuicideTimeField->Add("8 Seconds", 496, "", false, false);
    miSuicideTimeField->Add("10 Seconds", 620, "", false, false);
    miSuicideTimeField->Add("15 Seconds", 930, "", false, false);
    miSuicideTimeField->Add("20 Seconds", 1240, "", false, false);
    miSuicideTimeField->SetData(&game_values.suicidetime, NULL, NULL);
    miSuicideTimeField->SetKey(game_values.suicidetime);

    miWarpLockStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 260, "Warp Lock Style", 500, 220);
    miWarpLockStyleField->Add("Entrance Only", 0, "", false, false);
    miWarpLockStyleField->Add("Exit Only", 1, "", false, false);
    miWarpLockStyleField->Add("Entrance and Exit", 2, "", false, false);
    miWarpLockStyleField->Add("Entire Connection", 3, "", false, false);
    miWarpLockStyleField->Add("All Warps", 4, "", false, false);
    miWarpLockStyleField->SetData(&game_values.warplockstyle, NULL, NULL);
    miWarpLockStyleField->SetKey(game_values.warplockstyle);

    miWarpLockTimeField = new MI_SelectField(&rm->spr_selectfield, 70, 300, "Warp Lock Time", 500, 220);
    miWarpLockTimeField->Add("Off", 0, "", false, false);
    miWarpLockTimeField->Add("1 Second", 62, "", false, false);
    miWarpLockTimeField->Add("2 Seconds", 124, "", false, false);
    miWarpLockTimeField->Add("3 Seconds", 186, "", false, false);
    miWarpLockTimeField->Add("4 Seconds", 248, "", false, false);
    miWarpLockTimeField->Add("5 Seconds", 310, "", false, false);
    miWarpLockTimeField->Add("6 Seconds", 372, "", false, false);
    miWarpLockTimeField->Add("7 Seconds", 434, "", false, false);
    miWarpLockTimeField->Add("8 Seconds", 496, "", false, false);
    miWarpLockTimeField->Add("9 Seconds", 558, "", false, false);
    miWarpLockTimeField->Add("10 Seconds", 620, "", false, false);
    miWarpLockTimeField->SetData(&game_values.warplocktime, NULL, NULL);
    miWarpLockTimeField->SetKey(game_values.warplocktime);

    miBotsField = new MI_SelectField(&rm->spr_selectfield, 70, 340, "Bot Difficulty", 500, 220);
    miBotsField->Add("Very Easy", 0, "", false, false);
    miBotsField->Add("Easy", 1, "", false, false);
    miBotsField->Add("Moderate", 2, "", false, false);
    miBotsField->Add("Hard", 3, "", false, false);
    miBotsField->Add("Very Hard", 4, "", false, false);
    miBotsField->SetData(&game_values.cpudifficulty, NULL, NULL);
    miBotsField->SetKey(game_values.cpudifficulty);

    miPointSpeedField = new MI_SelectField(&rm->spr_selectfield, 70, 380, "Point Speed", 500, 220);
    miPointSpeedField->Add("Very Slow", 60, "", false, false);
    miPointSpeedField->Add("Slow", 40, "", false, false);
    miPointSpeedField->Add("Moderate", 20, "", false, false);
    miPointSpeedField->Add("Fast", 10, "", false, false);
    miPointSpeedField->Add("Very Fast", 5, "", false, false);
    miPointSpeedField->SetData(&game_values.pointspeed, NULL, NULL);
    miPointSpeedField->SetKey(game_values.pointspeed);

    miGameplayOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miGameplayOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miGameplayOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miGameplayOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miGameplayOptionsMenuHeaderText = new MI_Text("Gameplay Options Menu", 320, 5, 0, 2, 1);

    AddControl(miRespawnField, miGameplayOptionsMenuBackButton, miShieldStyleField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miShieldStyleField, miRespawnField, miShieldTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miShieldTimeField, miShieldStyleField, miBoundsTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miBoundsTimeField, miShieldTimeField, miSuicideTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miSuicideTimeField, miBoundsTimeField, miWarpLockStyleField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miWarpLockStyleField, miSuicideTimeField, miWarpLockTimeField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miWarpLockTimeField, miWarpLockStyleField, miBotsField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miBotsField, miWarpLockTimeField, miPointSpeedField, NULL, miGameplayOptionsMenuBackButton);
    AddControl(miPointSpeedField, miBotsField, miGameplayOptionsMenuBackButton, NULL, miGameplayOptionsMenuBackButton);

    AddControl(miGameplayOptionsMenuBackButton, miPointSpeedField, miRespawnField, miPointSpeedField, NULL);

    AddNonControl(miGameplayOptionsMenuLeftHeaderBar);
    AddNonControl(miGameplayOptionsMenuRightHeaderBar);
    AddNonControl(miGameplayOptionsMenuHeaderText);

    SetHeadControl(miRespawnField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};

UI_GameplayOptionsMenu::~UI_GameplayOptionsMenu() {
}
