#include "ProjectileOptionsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;
extern CGameValues game_values;


UI_ProjectileOptionsMenu::UI_ProjectileOptionsMenu()
    : UI_Menu()
{
    miFireballLifeField = new MI_SelectField(&rm->spr_selectfield, 10, 80, "Life", 305, 120);
    miFireballLifeField->Add("1 Second", 62);
    miFireballLifeField->Add("2 Seconds", 124);
    miFireballLifeField->Add("3 Seconds", 186);
    miFireballLifeField->Add("4 Seconds", 248);
    miFireballLifeField->Add("5 Seconds", 310);
    miFireballLifeField->Add("6 Seconds", 372);
    miFireballLifeField->Add("7 Seconds", 434);
    miFireballLifeField->Add("8 Seconds", 496);
    miFireballLifeField->Add("9 Seconds", 558);
    miFireballLifeField->Add("10 Seconds", 620);
    miFireballLifeField->SetData(&game_values.fireballttl, NULL, NULL);
    miFireballLifeField->SetKey(game_values.fireballttl);

    miFeatherJumpsField = new MI_SelectField(&rm->spr_selectfield, 10, 150, "Jumps", 305, 120);
    miFeatherJumpsField->Add("1", 1);
    miFeatherJumpsField->Add("2", 2);
    miFeatherJumpsField->Add("3", 3);
    miFeatherJumpsField->Add("4", 4);
    miFeatherJumpsField->Add("5", 5);
    miFeatherJumpsField->SetData(&game_values.featherjumps, NULL, NULL);
    miFeatherJumpsField->SetKey(game_values.featherjumps);

    miBoomerangStyleField = new MI_SelectField(&rm->spr_selectfield, 10, 220, "Style", 305, 120);
    miBoomerangStyleField->Add("Flat", 0);
    miBoomerangStyleField->Add("SMB3", 1);
    miBoomerangStyleField->Add("Zelda", 2);
    miBoomerangStyleField->Add("Random", 3);
    miBoomerangStyleField->SetData(&game_values.boomerangstyle, NULL, NULL);
    miBoomerangStyleField->SetKey(game_values.boomerangstyle);

    miBoomerangLifeField = new MI_SelectField(&rm->spr_selectfield, 10, 260, "Life", 305, 120);
    miBoomerangLifeField->Add("1 Second", 62);
    miBoomerangLifeField->Add("2 Seconds", 124);
    miBoomerangLifeField->Add("3 Seconds", 186);
    miBoomerangLifeField->Add("4 Seconds", 248);
    miBoomerangLifeField->Add("5 Seconds", 310);
    miBoomerangLifeField->Add("6 Seconds", 372);
    miBoomerangLifeField->Add("7 Seconds", 434);
    miBoomerangLifeField->Add("8 Seconds", 496);
    miBoomerangLifeField->Add("9 Seconds", 558);
    miBoomerangLifeField->Add("10 Seconds", 620);
    miBoomerangLifeField->SetData(&game_values.boomeranglife, NULL, NULL);
    miBoomerangLifeField->SetKey(game_values.boomeranglife);

    miHammerLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 80, "Life", 305, 120);
    miHammerLifeField->Add("No Limit", 310);
    miHammerLifeField->Add("0.5 Seconds", 31);
    miHammerLifeField->Add("0.6 Seconds", 37);
    miHammerLifeField->Add("0.7 Seconds", 43);
    miHammerLifeField->Add("0.8 Seconds", 49);
    miHammerLifeField->Add("0.9 Seconds", 55);
    miHammerLifeField->Add("1.0 Seconds", 62);
    miHammerLifeField->Add("1.1 Seconds", 68);
    miHammerLifeField->Add("1.2 Seconds", 74);
    miHammerLifeField->SetData(&game_values.hammerttl, NULL, NULL);
    miHammerLifeField->SetKey(game_values.hammerttl);

    miHammerDelayField = new MI_SelectField(&rm->spr_selectfield, 325, 120, "Delay", 305, 120);
    miHammerDelayField->Add("None", 0);
    miHammerDelayField->Add("0.1 Seconds", 6);
    miHammerDelayField->Add("0.2 Seconds", 12);
    miHammerDelayField->Add("0.3 Seconds", 19);
    miHammerDelayField->Add("0.4 Seconds", 25);
    miHammerDelayField->Add("0.5 Seconds", 31);
    miHammerDelayField->Add("0.6 Seconds", 37);
    miHammerDelayField->Add("0.7 Seconds", 43);
    miHammerDelayField->Add("0.8 Seconds", 49);
    miHammerDelayField->Add("0.9 Seconds", 55);
    miHammerDelayField->Add("1.0 Seconds", 62);
    miHammerDelayField->SetData(&game_values.hammerdelay, NULL, NULL);
    miHammerDelayField->SetKey(game_values.hammerdelay);

    miHammerOneKillField = new MI_SelectField(&rm->spr_selectfield, 325, 160, "Power", 305, 120);
    miHammerOneKillField->Add("One Kill", 0, "", true, false);
    miHammerOneKillField->Add("Multiple Kills", 1);
    miHammerOneKillField->SetData(NULL, NULL, &game_values.hammerpower);
    miHammerOneKillField->SetKey(game_values.hammerpower ? 0 : 1);
    miHammerOneKillField->SetAutoAdvance(true);

    miShellLifeField = new MI_SelectField(&rm->spr_selectfield, 10, 330, "Life", 305, 120);
    miShellLifeField->Add("Unlimited", 0);
    miShellLifeField->Add("1 Second", 62);
    miShellLifeField->Add("2 Seconds", 124);
    miShellLifeField->Add("3 Seconds", 186);
    miShellLifeField->Add("4 Seconds", 248);
    miShellLifeField->Add("5 Seconds", 310);
    miShellLifeField->Add("6 Seconds", 372);
    miShellLifeField->Add("7 Seconds", 434);
    miShellLifeField->Add("8 Seconds", 496);
    miShellLifeField->Add("9 Seconds", 558);
    miShellLifeField->Add("10 Seconds", 620);
    miShellLifeField->Add("15 Seconds", 930);
    miShellLifeField->Add("20 Seconds", 1240);
    miShellLifeField->Add("25 Seconds", 1550);
    miShellLifeField->Add("30 Seconds", 1860);
    miShellLifeField->SetData(&game_values.shellttl, NULL, NULL);
    miShellLifeField->SetKey(game_values.shellttl);

    miWandFreezeTimeField = new MI_SelectField(&rm->spr_selectfield, 10, 400, "Freeze", 305, 120);
    miWandFreezeTimeField->Add("1 Second", 62);
    miWandFreezeTimeField->Add("2 Seconds", 124);
    miWandFreezeTimeField->Add("3 Seconds", 186);
    miWandFreezeTimeField->Add("4 Seconds", 248);
    miWandFreezeTimeField->Add("5 Seconds", 310);
    miWandFreezeTimeField->Add("6 Seconds", 372);
    miWandFreezeTimeField->Add("7 Seconds", 434);
    miWandFreezeTimeField->Add("8 Seconds", 496);
    miWandFreezeTimeField->Add("9 Seconds", 558);
    miWandFreezeTimeField->Add("10 Seconds", 620);
    miWandFreezeTimeField->Add("12 Seconds", 744);
    miWandFreezeTimeField->Add("15 Seconds", 930);
    miWandFreezeTimeField->Add("18 Seconds", 1116);
    miWandFreezeTimeField->Add("20 Seconds", 1240);
    miWandFreezeTimeField->SetData(&game_values.wandfreezetime, NULL, NULL);
    miWandFreezeTimeField->SetKey(game_values.wandfreezetime);

    miBlueBlockLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 230, "Blue Life", 305, 120);
    miBlueBlockLifeField->Add("Unlimited", 0);
    miBlueBlockLifeField->Add("1 Second", 62);
    miBlueBlockLifeField->Add("2 Seconds", 124);
    miBlueBlockLifeField->Add("3 Seconds", 186);
    miBlueBlockLifeField->Add("4 Seconds", 248);
    miBlueBlockLifeField->Add("5 Seconds", 310);
    miBlueBlockLifeField->Add("6 Seconds", 372);
    miBlueBlockLifeField->Add("7 Seconds", 434);
    miBlueBlockLifeField->Add("8 Seconds", 496);
    miBlueBlockLifeField->Add("9 Seconds", 558);
    miBlueBlockLifeField->Add("10 Seconds", 620);
    miBlueBlockLifeField->Add("15 Seconds", 930);
    miBlueBlockLifeField->Add("20 Seconds", 1240);
    miBlueBlockLifeField->Add("25 Seconds", 1550);
    miBlueBlockLifeField->Add("30 Seconds", 1860);
    miBlueBlockLifeField->SetData(&game_values.blueblockttl, NULL, NULL);
    miBlueBlockLifeField->SetKey(game_values.blueblockttl);

    miGrayBlockLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 270, "Gray Life", 305, 120);
    miGrayBlockLifeField->Add("Unlimited", 0);
    miGrayBlockLifeField->Add("1 Second", 62);
    miGrayBlockLifeField->Add("2 Seconds", 124);
    miGrayBlockLifeField->Add("3 Seconds", 186);
    miGrayBlockLifeField->Add("4 Seconds", 248);
    miGrayBlockLifeField->Add("5 Seconds", 310);
    miGrayBlockLifeField->Add("6 Seconds", 372);
    miGrayBlockLifeField->Add("7 Seconds", 434);
    miGrayBlockLifeField->Add("8 Seconds", 496);
    miGrayBlockLifeField->Add("9 Seconds", 558);
    miGrayBlockLifeField->Add("10 Seconds", 620);
    miGrayBlockLifeField->Add("15 Seconds", 930);
    miGrayBlockLifeField->Add("20 Seconds", 1240);
    miGrayBlockLifeField->Add("25 Seconds", 1550);
    miGrayBlockLifeField->Add("30 Seconds", 1860);
    miGrayBlockLifeField->SetData(&game_values.grayblockttl, NULL, NULL);
    miGrayBlockLifeField->SetKey(game_values.grayblockttl);

    miRedBlockLifeField = new MI_SelectField(&rm->spr_selectfield, 325, 310, "Red Life", 305, 120);
    miRedBlockLifeField->Add("Unlimited", 0);
    miRedBlockLifeField->Add("1 Second", 62);
    miRedBlockLifeField->Add("2 Seconds", 124);
    miRedBlockLifeField->Add("3 Seconds", 186);
    miRedBlockLifeField->Add("4 Seconds", 248);
    miRedBlockLifeField->Add("5 Seconds", 310);
    miRedBlockLifeField->Add("6 Seconds", 372);
    miRedBlockLifeField->Add("7 Seconds", 434);
    miRedBlockLifeField->Add("8 Seconds", 496);
    miRedBlockLifeField->Add("9 Seconds", 558);
    miRedBlockLifeField->Add("10 Seconds", 620);
    miRedBlockLifeField->Add("15 Seconds", 930);
    miRedBlockLifeField->Add("20 Seconds", 1240);
    miRedBlockLifeField->Add("25 Seconds", 1550);
    miRedBlockLifeField->Add("30 Seconds", 1860);
    miRedBlockLifeField->SetData(&game_values.redblockttl, NULL, NULL);
    miRedBlockLifeField->SetKey(game_values.redblockttl);

    miProjectileOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miProjectileOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miProjectileOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miProjectileOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miProjectileOptionsMenuHeaderText = new MI_HeaderText("Projectile & Weapon Options Menu", 320, 5);

    miFireballText = new MI_Text("Fireball", 10, 50, 0, true, TextAlign::LEFT);
    miFeatherText = new MI_Text("Feather", 10, 120, 0, true, TextAlign::LEFT);
    miBoomerangText = new MI_Text("Boomerang", 10, 190, 0, true, TextAlign::LEFT);
    miHammerText = new MI_Text("Hammer", 325, 50, 0, true, TextAlign::LEFT);
    miShellText = new MI_Text("Shell", 10, 300, 0, true, TextAlign::LEFT);
    miWandText = new MI_Text("Wand", 10, 370, 0, true, TextAlign::LEFT);
    miBlueBlockText = new MI_Text("Throwable Blocks", 325, 200, 0, true, TextAlign::LEFT);

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
