#include "OptionsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_OptionsMenu::UI_OptionsMenu() : UI_Menu()
{
    miGameplayOptionsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 40, "Gameplay", 400, 1);
    miGameplayOptionsMenuButton->SetCode(MENU_CODE_TO_GAMEPLAY_OPTIONS_MENU);

    miTeamOptionsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 80, "Team", 400, 1);
    miTeamOptionsMenuButton->SetCode(MENU_CODE_TO_TEAM_OPTIONS_MENU);

    miPowerupOptionsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 120, "Item Selection", 400, 1);
    miPowerupOptionsMenuButton->SetCode(MENU_CODE_TO_POWERUP_SELECTION_MENU);

    miPowerupSettingsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 160, "Item Settings", 400, 1);
    miPowerupSettingsMenuButton->SetCode(MENU_CODE_TO_POWERUP_SETTINGS_MENU);

    miProjectilesOptionsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 200, "Weapons & Projectiles", 400, 1);
    miProjectilesOptionsMenuButton->SetCode(MENU_CODE_TO_PROJECTILES_OPTIONS_MENU);

    miProjectilesLimitsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 240, "Weapon Use Limits", 400, 1);
    miProjectilesLimitsMenuButton->SetCode(MENU_CODE_TO_PROJECTILES_LIMITS_MENU);

    miGraphicsOptionsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 280, "Graphics", 400, 1);
    miGraphicsOptionsMenuButton->SetCode(MENU_CODE_TO_GRAPHICS_OPTIONS_MENU);

    miEyeCandyOptionsMenuButton = new MI_Button(&rm->spr_selectfield, 120, 320, "Eye Candy", 400, 1);
    miEyeCandyOptionsMenuButton->SetCode(MENU_CODE_TO_EYECANDY_OPTIONS_MENU);

    miSoundOptionsMenuButton = new MI_Button((game_values.soundcapable ? &rm->spr_selectfield : &rm->spr_selectfielddisabled), 120, 360, "Music & Sound", 400, 1);

    if (game_values.soundcapable)
        miSoundOptionsMenuButton->SetCode(MENU_CODE_TO_SOUND_OPTIONS_MENU);

    miGenerateMapThumbsButton = new MI_Button(&rm->spr_selectfield, 120, 400, "Refresh Maps", 400, 1);
    miGenerateMapThumbsButton->SetCode(MENU_CODE_SAVE_ALL_MAP_THUMBNAILS);

    miOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miOptionsMenuBackButton->SetCode(MENU_CODE_TO_MAIN_MENU);

    miOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miOptionsMenuHeaderText = new MI_Text("Options Menu", 320, 5, 0, 2, 1);

    miGenerateThumbsDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miGenerateThumbsDialogAreYouText = new MI_Text("Are You", 320, 195, 0, 2, 1);
    miGenerateThumbsDialogSureText = new MI_Text("Sure?", 320, 220, 0, 2, 1);
    miGenerateThumbsDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, 1);
    miGenerateThumbsDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, 1);

    miGenerateThumbsDialogYesButton->SetCode(MENU_CODE_GENERATE_THUMBS_RESET_YES);
    miGenerateThumbsDialogNoButton->SetCode(MENU_CODE_GENERATE_THUMBS_RESET_NO);

    miGenerateThumbsDialogImage->Show(false);
    miGenerateThumbsDialogAreYouText->Show(false);
    miGenerateThumbsDialogSureText->Show(false);
    miGenerateThumbsDialogYesButton->Show(false);
    miGenerateThumbsDialogNoButton->Show(false);

    AddControl(miGameplayOptionsMenuButton, miOptionsMenuBackButton, miTeamOptionsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miTeamOptionsMenuButton, miGameplayOptionsMenuButton, miPowerupOptionsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miPowerupOptionsMenuButton, miTeamOptionsMenuButton, miPowerupSettingsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miPowerupSettingsMenuButton, miPowerupOptionsMenuButton, miProjectilesOptionsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miProjectilesOptionsMenuButton, miPowerupSettingsMenuButton, miProjectilesLimitsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miProjectilesLimitsMenuButton, miProjectilesOptionsMenuButton, miGraphicsOptionsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miGraphicsOptionsMenuButton, miProjectilesLimitsMenuButton, miEyeCandyOptionsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miEyeCandyOptionsMenuButton, miGraphicsOptionsMenuButton, miSoundOptionsMenuButton, NULL, miOptionsMenuBackButton);
    AddControl(miSoundOptionsMenuButton, miEyeCandyOptionsMenuButton, miGenerateMapThumbsButton, NULL, miOptionsMenuBackButton);
    AddControl(miGenerateMapThumbsButton, miSoundOptionsMenuButton, miOptionsMenuBackButton, NULL, miOptionsMenuBackButton);

    AddControl(miOptionsMenuBackButton, miGenerateMapThumbsButton, miGameplayOptionsMenuButton, miGenerateMapThumbsButton, NULL);

    AddNonControl(miOptionsMenuLeftHeaderBar);
    AddNonControl(miOptionsMenuRightHeaderBar);
    AddNonControl(miOptionsMenuHeaderText);

    AddNonControl(miGenerateThumbsDialogImage);
    AddNonControl(miGenerateThumbsDialogAreYouText);
    AddNonControl(miGenerateThumbsDialogSureText);

    AddControl(miGenerateThumbsDialogYesButton, NULL, NULL, NULL, miGenerateThumbsDialogNoButton);
    AddControl(miGenerateThumbsDialogNoButton, NULL, NULL, miGenerateThumbsDialogYesButton, NULL);

    SetHeadControl(miGameplayOptionsMenuButton);
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);
};

UI_OptionsMenu::~UI_OptionsMenu() {
}

void UI_OptionsMenu::showThumbnailsPopup() {
    miGenerateThumbsDialogImage->Show(true);
    miGenerateThumbsDialogAreYouText->Show(true);
    miGenerateThumbsDialogSureText->Show(true);
    miGenerateThumbsDialogYesButton->Show(true);
    miGenerateThumbsDialogNoButton->Show(true);

    RememberCurrent();

    SetHeadControl(miGenerateThumbsDialogNoButton);
    SetCancelCode(MENU_CODE_GENERATE_THUMBS_RESET_NO);
}

void UI_OptionsMenu::hideThumbnailsPopup() {
    miGenerateThumbsDialogImage->Show(false);
    miGenerateThumbsDialogAreYouText->Show(false);
    miGenerateThumbsDialogSureText->Show(false);
    miGenerateThumbsDialogYesButton->Show(false);
    miGenerateThumbsDialogNoButton->Show(false);

    SetHeadControl(miGameplayOptionsMenuButton);
    SetCancelCode(MENU_CODE_TO_MAIN_MENU);

    RestoreCurrent();
}
