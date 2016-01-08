#include "ProjectileOptionsMenu.h"

#include "uicontrol.h"
#include "ui/MI_SelectField.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_ProjectileOptionsMenu::UI_ProjectileOptionsMenu() : UI_Menu()
{
    miFireballLifeField = new MI_SelectField(&rm->spr_selectfield, 10, 80, "Life", 305, 120);
    miFireballLifeField->Add("1 Second", 62, "", false, false);
    miFireballLifeField->Add("2 Seconds", 124, "", false, false);
    miFireballLifeField->Add("3 Seconds", 186, "", false, false);
    miFireballLifeField->Add("4 Seconds", 248, "", false, false);
    miFireballLifeField->Add("5 Seconds", 310, "", false, false);
    miFireballLifeField->Add("6 Seconds", 372, "", false, false);
    miFireballLifeField->Add("7 Seconds", 434, "", false, false);
    miFireballLifeField->Add("8 Seconds", 496, "", false, false);
    miFireballLifeField->Add("9 Seconds", 558, "", false, false);
    miFireballLifeField->Add("10 Seconds", 620, "", false, false);
    miFireballLifeField->SetData(&game_values.fireballttl, NULL, NULL);
    miFireballLifeField->SetKey(game_values.fireballttl);

    miFeatherJumpsField = new MI_SelectField(&rm->spr_selectfield, 10, 150, "Jumps", 305, 120);
    miFeatherJumpsField->Add("1", 1, "", false, false);
    miFeatherJumpsField->Add("2", 2, "", false, false);
    miFeatherJumpsField->Add("3", 3, "", false, false);
    miFeatherJumpsField->Add("4", 4, "", false, false);
    miFeatherJumpsField->Add("5", 5, "", false, false);
    miFeatherJumpsField->SetData(&game_values.featherjumps, NULL, NULL);
    miFeatherJumpsField->SetKey(game_values.featherjumps);

    miBoomerangStyleField = new MI_SelectField(&rm->spr_selectfield, 10, 220, "Style", 305, 120);
    miBoomerangStyleField->Add("Flat", 0, "", false, false);
    miBoomerangStyleField->Add("SMB3", 1, "", false, false);
    miBoomerangStyleField->Add("Zelda", 2, "", false, false);
    miBoomerangStyleField->Add("Random", 3, "", false, false);
    miBoomerangStyleField->SetData(&game_values.boomerangstyle, NULL, NULL);
    miBoomerangStyleField->SetKey(game_values.boomerangstyle);

    miBoomerangLifeField = new MI_SelectField(&rm->spr_selectfield, 10, 260, "Life", 305, 120);
    miBoomerangLifeField->Add("1 Second", 62, "", false, false);
    miBoomerangLifeField->Add("2 Seconds", 124, "", false, false);
    miBoomerangLifeField->Add("3 Seconds", 186, "", false, false);
    miBoomerangLifeField->Add("4 Seconds", 248, "", false, false);
    miBoomerangLifeField->Add("5 Seconds", 310, "", false, false);
    miBoomerangLifeField->Add("6 Seconds", 372, "", false, false);
    miBoomerangLifeField->Add("7 Seconds", 434, "", false, false);
    miBoomerangLifeField->Add("8 Seconds", 496, "", false, false);
    miBoomerangLifeField->Add("9 Seconds", 558, "", false, false);
    miBoomerangLifeField->Add("10 Seconds", 620, "", false, false);
    miBoomerangLifeField->SetData(&game_values.boomeranglife, NULL, NULL);
    miBoomerangLifeField->SetKey(game_values.boomeranglife);

    miHammerLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 80, "Life", 305, 120);
    miHammerLifeField->Add("No Limit", 310, "", false, false);
    miHammerLifeField->Add("0.5 Seconds", 31, "", false, false);
    miHammerLifeField->Add("0.6 Seconds", 37, "", false, false);
    miHammerLifeField->Add("0.7 Seconds", 43, "", false, false);
    miHammerLifeField->Add("0.8 Seconds", 49, "", false, false);
    miHammerLifeField->Add("0.9 Seconds", 55, "", false, false);
    miHammerLifeField->Add("1.0 Seconds", 62, "", false, false);
    miHammerLifeField->Add("1.1 Seconds", 68, "", false, false);
    miHammerLifeField->Add("1.2 Seconds", 74, "", false, false);
    miHammerLifeField->SetData(&game_values.hammerttl, NULL, NULL);
    miHammerLifeField->SetKey(game_values.hammerttl);

    miHammerDelayField = new MI_SelectField(&rm->spr_selectfield, 325, 120, "Delay", 305, 120);
    miHammerDelayField->Add("None", 0, "", false, false);
    miHammerDelayField->Add("0.1 Seconds", 6, "", false, false);
    miHammerDelayField->Add("0.2 Seconds", 12, "", false, false);
    miHammerDelayField->Add("0.3 Seconds", 19, "", false, false);
    miHammerDelayField->Add("0.4 Seconds", 25, "", false, false);
    miHammerDelayField->Add("0.5 Seconds", 31, "", false, false);
    miHammerDelayField->Add("0.6 Seconds", 37, "", false, false);
    miHammerDelayField->Add("0.7 Seconds", 43, "", false, false);
    miHammerDelayField->Add("0.8 Seconds", 49, "", false, false);
    miHammerDelayField->Add("0.9 Seconds", 55, "", false, false);
    miHammerDelayField->Add("1.0 Seconds", 62, "", false, false);
    miHammerDelayField->SetData(&game_values.hammerdelay, NULL, NULL);
    miHammerDelayField->SetKey(game_values.hammerdelay);

    miHammerOneKillField = new MI_SelectField(&rm->spr_selectfield, 325, 160, "Power", 305, 120);
    miHammerOneKillField->Add("One Kill", 0, "", true, false);
    miHammerOneKillField->Add("Multiple Kills", 1, "", false, false);
    miHammerOneKillField->SetData(NULL, NULL, &game_values.hammerpower);
    miHammerOneKillField->SetKey(game_values.hammerpower ? 0 : 1);
    miHammerOneKillField->SetAutoAdvance(true);

    miShellLifeField = new MI_SelectField(&rm->spr_selectfield, 10, 330, "Life", 305, 120);
    miShellLifeField->Add("Unlimited", 0, "", false, false);
    miShellLifeField->Add("1 Second", 62, "", false, false);
    miShellLifeField->Add("2 Seconds", 124, "", false, false);
    miShellLifeField->Add("3 Seconds", 186, "", false, false);
    miShellLifeField->Add("4 Seconds", 248, "", false, false);
    miShellLifeField->Add("5 Seconds", 310, "", false, false);
    miShellLifeField->Add("6 Seconds", 372, "", false, false);
    miShellLifeField->Add("7 Seconds", 434, "", false, false);
    miShellLifeField->Add("8 Seconds", 496, "", false, false);
    miShellLifeField->Add("9 Seconds", 558, "", false, false);
    miShellLifeField->Add("10 Seconds", 620, "", false, false);
    miShellLifeField->Add("15 Seconds", 930, "", false, false);
    miShellLifeField->Add("20 Seconds", 1240, "", false, false);
    miShellLifeField->Add("25 Seconds", 1550, "", false, false);
    miShellLifeField->Add("30 Seconds", 1860, "", false, false);
    miShellLifeField->SetData(&game_values.shellttl, NULL, NULL);
    miShellLifeField->SetKey(game_values.shellttl);

    miWandFreezeTimeField = new MI_SelectField(&rm->spr_selectfield, 10, 400, "Freeze", 305, 120);
    miWandFreezeTimeField->Add("1 Second", 62, "", false, false);
    miWandFreezeTimeField->Add("2 Seconds", 124, "", false, false);
    miWandFreezeTimeField->Add("3 Seconds", 186, "", false, false);
    miWandFreezeTimeField->Add("4 Seconds", 248, "", false, false);
    miWandFreezeTimeField->Add("5 Seconds", 310, "", false, false);
    miWandFreezeTimeField->Add("6 Seconds", 372, "", false, false);
    miWandFreezeTimeField->Add("7 Seconds", 434, "", false, false);
    miWandFreezeTimeField->Add("8 Seconds", 496, "", false, false);
    miWandFreezeTimeField->Add("9 Seconds", 558, "", false, false);
    miWandFreezeTimeField->Add("10 Seconds", 620, "", false, false);
    miWandFreezeTimeField->Add("12 Seconds", 744, "", false, false);
    miWandFreezeTimeField->Add("15 Seconds", 930, "", false, false);
    miWandFreezeTimeField->Add("18 Seconds", 1116, "", false, false);
    miWandFreezeTimeField->Add("20 Seconds", 1240, "", false, false);
    miWandFreezeTimeField->SetData(&game_values.wandfreezetime, NULL, NULL);
    miWandFreezeTimeField->SetKey(game_values.wandfreezetime);

    miBlueBlockLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 230, "Blue Life", 305, 120);
    miBlueBlockLifeField->Add("Unlimited", 0, "", false, false);
    miBlueBlockLifeField->Add("1 Second", 62, "", false, false);
    miBlueBlockLifeField->Add("2 Seconds", 124, "", false, false);
    miBlueBlockLifeField->Add("3 Seconds", 186, "", false, false);
    miBlueBlockLifeField->Add("4 Seconds", 248, "", false, false);
    miBlueBlockLifeField->Add("5 Seconds", 310, "", false, false);
    miBlueBlockLifeField->Add("6 Seconds", 372, "", false, false);
    miBlueBlockLifeField->Add("7 Seconds", 434, "", false, false);
    miBlueBlockLifeField->Add("8 Seconds", 496, "", false, false);
    miBlueBlockLifeField->Add("9 Seconds", 558, "", false, false);
    miBlueBlockLifeField->Add("10 Seconds", 620, "", false, false);
    miBlueBlockLifeField->Add("15 Seconds", 930, "", false, false);
    miBlueBlockLifeField->Add("20 Seconds", 1240, "", false, false);
    miBlueBlockLifeField->Add("25 Seconds", 1550, "", false, false);
    miBlueBlockLifeField->Add("30 Seconds", 1860, "", false, false);
    miBlueBlockLifeField->SetData(&game_values.blueblockttl, NULL, NULL);
    miBlueBlockLifeField->SetKey(game_values.blueblockttl);

    miGrayBlockLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 270, "Gray Life", 305, 120);
    miGrayBlockLifeField->Add("Unlimited", 0, "", false, false);
    miGrayBlockLifeField->Add("1 Second", 62, "", false, false);
    miGrayBlockLifeField->Add("2 Seconds", 124, "", false, false);
    miGrayBlockLifeField->Add("3 Seconds", 186, "", false, false);
    miGrayBlockLifeField->Add("4 Seconds", 248, "", false, false);
    miGrayBlockLifeField->Add("5 Seconds", 310, "", false, false);
    miGrayBlockLifeField->Add("6 Seconds", 372, "", false, false);
    miGrayBlockLifeField->Add("7 Seconds", 434, "", false, false);
    miGrayBlockLifeField->Add("8 Seconds", 496, "", false, false);
    miGrayBlockLifeField->Add("9 Seconds", 558, "", false, false);
    miGrayBlockLifeField->Add("10 Seconds", 620, "", false, false);
    miGrayBlockLifeField->Add("15 Seconds", 930, "", false, false);
    miGrayBlockLifeField->Add("20 Seconds", 1240, "", false, false);
    miGrayBlockLifeField->Add("25 Seconds", 1550, "", false, false);
    miGrayBlockLifeField->Add("30 Seconds", 1860, "", false, false);
    miGrayBlockLifeField->SetData(&game_values.grayblockttl, NULL, NULL);
    miGrayBlockLifeField->SetKey(game_values.grayblockttl);

    miRedBlockLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 310, "Red Life", 305, 120);
    miRedBlockLifeField->Add("Unlimited", 0, "", false, false);
    miRedBlockLifeField->Add("1 Second", 62, "", false, false);
    miRedBlockLifeField->Add("2 Seconds", 124, "", false, false);
    miRedBlockLifeField->Add("3 Seconds", 186, "", false, false);
    miRedBlockLifeField->Add("4 Seconds", 248, "", false, false);
    miRedBlockLifeField->Add("5 Seconds", 310, "", false, false);
    miRedBlockLifeField->Add("6 Seconds", 372, "", false, false);
    miRedBlockLifeField->Add("7 Seconds", 434, "", false, false);
    miRedBlockLifeField->Add("8 Seconds", 496, "", false, false);
    miRedBlockLifeField->Add("9 Seconds", 558, "", false, false);
    miRedBlockLifeField->Add("10 Seconds", 620, "", false, false);
    miRedBlockLifeField->Add("15 Seconds", 930, "", false, false);
    miRedBlockLifeField->Add("20 Seconds", 1240, "", false, false);
    miRedBlockLifeField->Add("25 Seconds", 1550, "", false, false);
    miRedBlockLifeField->Add("30 Seconds", 1860, "", false, false);
    miRedBlockLifeField->SetData(&game_values.redblockttl, NULL, NULL);
    miRedBlockLifeField->SetKey(game_values.redblockttl);

    miProjectileOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miProjectileOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miProjectileOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miProjectileOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miProjectileOptionsMenuHeaderText = new MI_Text("Projectile & Weapon Options Menu", 320, 5, 0, 2, 1);

    miFireballText = new MI_Text("Fireball", 10, 50, 0, 2, 0);
    miFeatherText = new MI_Text("Feather", 10, 120, 0, 2, 0);
    miBoomerangText = new MI_Text("Boomerang", 10, 190, 0, 2, 0);
    miHammerText = new MI_Text("Hammer", 325, 50, 0, 2, 0);
    miShellText = new MI_Text("Shell", 10, 300, 0, 2, 0);
    miWandText = new MI_Text("Wand", 10, 370, 0, 2, 0);
    miBlueBlockText = new MI_Text("Throwable Blocks", 325, 200, 0, 2, 0);

    AddControl(miFireballLifeField, miProjectileOptionsMenuBackButton, miFeatherJumpsField, NULL, miHammerLifeField);

    AddControl(miFeatherJumpsField, miFireballLifeField, miBoomerangStyleField, NULL, miHammerOneKillField);

    AddControl(miBoomerangStyleField, miFeatherJumpsField, miBoomerangLifeField, NULL, miBlueBlockLifeField);
    AddControl(miBoomerangLifeField, miBoomerangStyleField, miShellLifeField, NULL, miGrayBlockLifeField);

    AddControl(miShellLifeField, miBoomerangLifeField, miWandFreezeTimeField, NULL, miRedBlockLifeField);

    AddControl(miWandFreezeTimeField, miShellLifeField, miHammerLifeField, NULL, miProjectileOptionsMenuBackButton);

    AddControl(miHammerLifeField, miWandFreezeTimeField, miHammerDelayField, miFireballLifeField, NULL);
    AddControl(miHammerDelayField, miHammerLifeField, miHammerOneKillField, miFireballLifeField, NULL);
    AddControl(miHammerOneKillField, miHammerDelayField, miBlueBlockLifeField, miFeatherJumpsField, NULL);

    AddControl(miBlueBlockLifeField, miHammerOneKillField, miGrayBlockLifeField, miBoomerangStyleField, NULL);
    AddControl(miGrayBlockLifeField, miBlueBlockLifeField, miRedBlockLifeField, miBoomerangLifeField, NULL);
    AddControl(miRedBlockLifeField, miGrayBlockLifeField, miProjectileOptionsMenuBackButton, miShellLifeField, NULL);

    AddControl(miProjectileOptionsMenuBackButton, miRedBlockLifeField, miFireballLifeField, miWandFreezeTimeField, NULL);

    AddNonControl(miFireballText);
    AddNonControl(miFeatherText);
    AddNonControl(miBoomerangText);
    AddNonControl(miHammerText);
    AddNonControl(miShellText);
    AddNonControl(miWandText);
    AddNonControl(miBlueBlockText);

    AddNonControl(miProjectileOptionsMenuLeftHeaderBar);
    AddNonControl(miProjectileOptionsMenuRightHeaderBar);
    AddNonControl(miProjectileOptionsMenuHeaderText);

    SetHeadControl(miFireballLifeField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};

UI_ProjectileOptionsMenu::~UI_ProjectileOptionsMenu() {
}
