#include "ProjectileLimitsMenu.h"

#include "uicontrol.h"
#include "ui/MI_SelectField.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_ProjectileLimitsMenu::UI_ProjectileLimitsMenu() : UI_Menu()
{
    miFireballLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 60, "Fireball Limit", 500, 220);
    miFireballLimitField->Add("Unlimited", 0, "", false, false);
    miFireballLimitField->Add("2", 2, "", false, false);
    miFireballLimitField->Add("5", 5, "", false, false);
    miFireballLimitField->Add("8", 8, "", false, false);
    miFireballLimitField->Add("10", 10, "", false, false);
    miFireballLimitField->Add("12", 12, "", false, false);
    miFireballLimitField->Add("15", 15, "", false, false);
    miFireballLimitField->Add("20", 20, "", false, false);
    miFireballLimitField->Add("25", 25, "", false, false);
    miFireballLimitField->Add("30", 30, "", false, false);
    miFireballLimitField->Add("40", 40, "", false, false);
    miFireballLimitField->Add("50", 50, "", false, false);
    miFireballLimitField->SetData(&game_values.fireballlimit, NULL, NULL);
    miFireballLimitField->SetKey(game_values.fireballlimit);

    miHammerLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 100, "Hammer Limit", 500, 220);
    miHammerLimitField->Add("Unlimited", 0, "", false, false);
    miHammerLimitField->Add("2", 2, "", false, false);
    miHammerLimitField->Add("5", 5, "", false, false);
    miHammerLimitField->Add("8", 8, "", false, false);
    miHammerLimitField->Add("10", 10, "", false, false);
    miHammerLimitField->Add("12", 12, "", false, false);
    miHammerLimitField->Add("15", 15, "", false, false);
    miHammerLimitField->Add("20", 20, "", false, false);
    miHammerLimitField->Add("25", 25, "", false, false);
    miHammerLimitField->Add("30", 30, "", false, false);
    miHammerLimitField->Add("40", 40, "", false, false);
    miHammerLimitField->Add("50", 50, "", false, false);
    miHammerLimitField->SetData(&game_values.hammerlimit, NULL, NULL);
    miHammerLimitField->SetKey(game_values.hammerlimit);

    miBoomerangLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 140, "Boomerang Limit", 500, 220);
    miBoomerangLimitField->Add("Unlimited", 0, "", false, false);
    miBoomerangLimitField->Add("2", 2, "", false, false);
    miBoomerangLimitField->Add("5", 5, "", false, false);
    miBoomerangLimitField->Add("8", 8, "", false, false);
    miBoomerangLimitField->Add("10", 10, "", false, false);
    miBoomerangLimitField->Add("12", 12, "", false, false);
    miBoomerangLimitField->Add("15", 15, "", false, false);
    miBoomerangLimitField->Add("20", 20, "", false, false);
    miBoomerangLimitField->Add("25", 25, "", false, false);
    miBoomerangLimitField->Add("30", 30, "", false, false);
    miBoomerangLimitField->Add("40", 40, "", false, false);
    miBoomerangLimitField->Add("50", 50, "", false, false);
    miBoomerangLimitField->SetData(&game_values.boomeranglimit, NULL, NULL);
    miBoomerangLimitField->SetKey(game_values.boomeranglimit);

    miFeatherLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 180, "Feather Limit", 500, 220);
    miFeatherLimitField->Add("Unlimited", 0, "", false, false);
    miFeatherLimitField->Add("2", 2, "", false, false);
    miFeatherLimitField->Add("5", 5, "", false, false);
    miFeatherLimitField->Add("8", 8, "", false, false);
    miFeatherLimitField->Add("10", 10, "", false, false);
    miFeatherLimitField->Add("12", 12, "", false, false);
    miFeatherLimitField->Add("15", 15, "", false, false);
    miFeatherLimitField->Add("20", 20, "", false, false);
    miFeatherLimitField->Add("25", 25, "", false, false);
    miFeatherLimitField->Add("30", 30, "", false, false);
    miFeatherLimitField->Add("40", 40, "", false, false);
    miFeatherLimitField->Add("50", 50, "", false, false);
    miFeatherLimitField->SetData(&game_values.featherlimit, NULL, NULL);
    miFeatherLimitField->SetKey(game_values.featherlimit);

    miLeafLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 220, "Leaf Limit", 500, 220);
    miLeafLimitField->Add("Unlimited", 0, "", false, false);
    miLeafLimitField->Add("2", 2, "", false, false);
    miLeafLimitField->Add("5", 5, "", false, false);
    miLeafLimitField->Add("8", 8, "", false, false);
    miLeafLimitField->Add("10", 10, "", false, false);
    miLeafLimitField->Add("12", 12, "", false, false);
    miLeafLimitField->Add("15", 15, "", false, false);
    miLeafLimitField->Add("20", 20, "", false, false);
    miLeafLimitField->Add("25", 25, "", false, false);
    miLeafLimitField->Add("30", 30, "", false, false);
    miLeafLimitField->Add("40", 40, "", false, false);
    miLeafLimitField->Add("50", 50, "", false, false);
    miLeafLimitField->SetData(&game_values.leaflimit, NULL, NULL);
    miLeafLimitField->SetKey(game_values.leaflimit);

    miPwingsLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 260, "P-Wings Limit", 500, 220);
    miPwingsLimitField->Add("Unlimited", 0, "", false, false);
    miPwingsLimitField->Add("2", 2, "", false, false);
    miPwingsLimitField->Add("5", 5, "", false, false);
    miPwingsLimitField->Add("8", 8, "", false, false);
    miPwingsLimitField->Add("10", 10, "", false, false);
    miPwingsLimitField->Add("12", 12, "", false, false);
    miPwingsLimitField->Add("15", 15, "", false, false);
    miPwingsLimitField->Add("20", 20, "", false, false);
    miPwingsLimitField->Add("25", 25, "", false, false);
    miPwingsLimitField->Add("30", 30, "", false, false);
    miPwingsLimitField->Add("40", 40, "", false, false);
    miPwingsLimitField->Add("50", 50, "", false, false);
    miPwingsLimitField->SetData(&game_values.pwingslimit, NULL, NULL);
    miPwingsLimitField->SetKey(game_values.pwingslimit);

    miTanookiLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 300, "Tanooki Limit", 500, 220);
    miTanookiLimitField ->Add("Unlimited", 0, "", false, false);
    miTanookiLimitField ->Add("2", 2, "", false, false);
    miTanookiLimitField ->Add("5", 5, "", false, false);
    miTanookiLimitField ->Add("8", 8, "", false, false);
    miTanookiLimitField ->Add("10", 10, "", false, false);
    miTanookiLimitField ->Add("12", 12, "", false, false);
    miTanookiLimitField ->Add("15", 15, "", false, false);
    miTanookiLimitField ->Add("20", 20, "", false, false);
    miTanookiLimitField ->Add("25", 25, "", false, false);
    miTanookiLimitField ->Add("30", 30, "", false, false);
    miTanookiLimitField ->Add("40", 40, "", false, false);
    miTanookiLimitField ->Add("50", 50, "", false, false);
    miTanookiLimitField ->SetData(&game_values.tanookilimit, NULL, NULL);
    miTanookiLimitField ->SetKey(game_values.tanookilimit);

    miBombLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 340, "Bomb Limit", 500, 220);
    miBombLimitField ->Add("Unlimited", 0, "", false, false);
    miBombLimitField ->Add("2", 2, "", false, false);
    miBombLimitField ->Add("5", 5, "", false, false);
    miBombLimitField ->Add("8", 8, "", false, false);
    miBombLimitField ->Add("10", 10, "", false, false);
    miBombLimitField ->Add("12", 12, "", false, false);
    miBombLimitField ->Add("15", 15, "", false, false);
    miBombLimitField ->Add("20", 20, "", false, false);
    miBombLimitField ->Add("25", 25, "", false, false);
    miBombLimitField ->Add("30", 30, "", false, false);
    miBombLimitField ->Add("40", 40, "", false, false);
    miBombLimitField ->Add("50", 50, "", false, false);
    miBombLimitField ->SetData(&game_values.bombslimit, NULL, NULL);
    miBombLimitField ->SetKey(game_values.bombslimit);

    miWandLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 380, "Wand Limit", 500, 220);
    miWandLimitField ->Add("Unlimited", 0, "", false, false);
    miWandLimitField ->Add("2", 2, "", false, false);
    miWandLimitField ->Add("5", 5, "", false, false);
    miWandLimitField ->Add("8", 8, "", false, false);
    miWandLimitField ->Add("10", 10, "", false, false);
    miWandLimitField ->Add("12", 12, "", false, false);
    miWandLimitField ->Add("15", 15, "", false, false);
    miWandLimitField ->Add("20", 20, "", false, false);
    miWandLimitField ->Add("25", 25, "", false, false);
    miWandLimitField ->Add("30", 30, "", false, false);
    miWandLimitField ->Add("40", 40, "", false, false);
    miWandLimitField ->Add("50", 50, "", false, false);
    miWandLimitField ->SetData(&game_values.wandlimit, NULL, NULL);
    miWandLimitField ->SetKey(game_values.wandlimit);

    miProjectilesLimitsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miProjectilesLimitsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miProjectilesLimitsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miProjectilesLimitsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miProjectilesLimitsMenuHeaderText = new MI_Text("Weapon Use Limits Menu", 320, 5, 0, 2, 1);

    AddControl(miFireballLimitField, miProjectilesLimitsMenuBackButton, miHammerLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miHammerLimitField, miFireballLimitField, miBoomerangLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miBoomerangLimitField, miHammerLimitField, miFeatherLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miFeatherLimitField, miBoomerangLimitField, miLeafLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miLeafLimitField, miFeatherLimitField, miPwingsLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miPwingsLimitField, miLeafLimitField, miTanookiLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miTanookiLimitField, miPwingsLimitField, miBombLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miBombLimitField, miTanookiLimitField, miWandLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miWandLimitField, miBombLimitField, miProjectilesLimitsMenuBackButton, NULL, miProjectilesLimitsMenuBackButton);

    AddControl(miProjectilesLimitsMenuBackButton, miWandLimitField, miFireballLimitField, miWandLimitField, NULL);

    AddNonControl(miProjectilesLimitsMenuLeftHeaderBar);
    AddNonControl(miProjectilesLimitsMenuRightHeaderBar);
    AddNonControl(miProjectilesLimitsMenuHeaderText);

    SetHeadControl(miFireballLimitField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};

UI_ProjectileLimitsMenu::~UI_ProjectileLimitsMenu() {
}
