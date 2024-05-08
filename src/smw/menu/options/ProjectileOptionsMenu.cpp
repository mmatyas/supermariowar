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
    miFireballLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 10, 80, "Life", 305, 120);
    miFireballLifeField->add("1 Second", 62);
    miFireballLifeField->add("2 Seconds", 124);
    miFireballLifeField->add("3 Seconds", 186);
    miFireballLifeField->add("4 Seconds", 248);
    miFireballLifeField->add("5 Seconds", 310);
    miFireballLifeField->add("6 Seconds", 372);
    miFireballLifeField->add("7 Seconds", 434);
    miFireballLifeField->add("8 Seconds", 496);
    miFireballLifeField->add("9 Seconds", 558);
    miFireballLifeField->add("10 Seconds", 620);
    miFireballLifeField->setOutputPtr(&game_values.fireballttl);
    miFireballLifeField->setCurrentValue(game_values.fireballttl);

    miFeatherJumpsField = new MI_SelectField<short>(&rm->spr_selectfield, 10, 150, "Jumps", 305, 120);
    miFeatherJumpsField->add("1", 1);
    miFeatherJumpsField->add("2", 2);
    miFeatherJumpsField->add("3", 3);
    miFeatherJumpsField->add("4", 4);
    miFeatherJumpsField->add("5", 5);
    miFeatherJumpsField->setOutputPtr(&game_values.featherjumps);
    miFeatherJumpsField->setCurrentValue(game_values.featherjumps);

    miBoomerangStyleField = new MI_SelectField<BoomerangStyle>(&rm->spr_selectfield, 10, 220, "Style", 305, 120);
    miBoomerangStyleField->add("Flat", BoomerangStyle::Flat);
    miBoomerangStyleField->add("SMB3", BoomerangStyle::SMB3);
    miBoomerangStyleField->add("Zelda", BoomerangStyle::Zelda);
    miBoomerangStyleField->add("Random", BoomerangStyle::Random);
    miBoomerangStyleField->setOutputPtr(&game_values.boomerangstyle);
    miBoomerangStyleField->setCurrentValue(game_values.boomerangstyle);

    miBoomerangLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 10, 260, "Life", 305, 120);
    miBoomerangLifeField->add("1 Second", 62);
    miBoomerangLifeField->add("2 Seconds", 124);
    miBoomerangLifeField->add("3 Seconds", 186);
    miBoomerangLifeField->add("4 Seconds", 248);
    miBoomerangLifeField->add("5 Seconds", 310);
    miBoomerangLifeField->add("6 Seconds", 372);
    miBoomerangLifeField->add("7 Seconds", 434);
    miBoomerangLifeField->add("8 Seconds", 496);
    miBoomerangLifeField->add("9 Seconds", 558);
    miBoomerangLifeField->add("10 Seconds", 620);
    miBoomerangLifeField->setOutputPtr(&game_values.boomeranglife);
    miBoomerangLifeField->setCurrentValue(game_values.boomeranglife);

    miHammerLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 325, 80, "Life", 305, 120);
    miHammerLifeField->add("No Limit", 310);
    miHammerLifeField->add("0.5 Seconds", 31);
    miHammerLifeField->add("0.6 Seconds", 37);
    miHammerLifeField->add("0.7 Seconds", 43);
    miHammerLifeField->add("0.8 Seconds", 49);
    miHammerLifeField->add("0.9 Seconds", 55);
    miHammerLifeField->add("1.0 Seconds", 62);
    miHammerLifeField->add("1.1 Seconds", 68);
    miHammerLifeField->add("1.2 Seconds", 74);
    miHammerLifeField->setOutputPtr(&game_values.hammerttl);
    miHammerLifeField->setCurrentValue(game_values.hammerttl);

    miHammerDelayField = new MI_SelectField<short>(&rm->spr_selectfield, 325, 120, "Delay", 305, 120);
    miHammerDelayField->add("None", 0);
    miHammerDelayField->add("0.1 Seconds", 6);
    miHammerDelayField->add("0.2 Seconds", 12);
    miHammerDelayField->add("0.3 Seconds", 19);
    miHammerDelayField->add("0.4 Seconds", 25);
    miHammerDelayField->add("0.5 Seconds", 31);
    miHammerDelayField->add("0.6 Seconds", 37);
    miHammerDelayField->add("0.7 Seconds", 43);
    miHammerDelayField->add("0.8 Seconds", 49);
    miHammerDelayField->add("0.9 Seconds", 55);
    miHammerDelayField->add("1.0 Seconds", 62);
    miHammerDelayField->setOutputPtr(&game_values.hammerdelay);
    miHammerDelayField->setCurrentValue(game_values.hammerdelay);

    miHammerOneKillField = new MI_SelectField<bool>(&rm->spr_selectfield, 325, 160, "Power", 305, 120);
    miHammerOneKillField->add("One Kill", true);
    miHammerOneKillField->add("Multiple Kills", false);
    miHammerOneKillField->setOutputPtr(&game_values.hammerpower);
    miHammerOneKillField->setCurrentValue(game_values.hammerpower);
    miHammerOneKillField->setAutoAdvance(true);

    miShellLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 10, 330, "Life", 305, 120);
    miShellLifeField->add("Unlimited", 0);
    miShellLifeField->add("1 Second", 62);
    miShellLifeField->add("2 Seconds", 124);
    miShellLifeField->add("3 Seconds", 186);
    miShellLifeField->add("4 Seconds", 248);
    miShellLifeField->add("5 Seconds", 310);
    miShellLifeField->add("6 Seconds", 372);
    miShellLifeField->add("7 Seconds", 434);
    miShellLifeField->add("8 Seconds", 496);
    miShellLifeField->add("9 Seconds", 558);
    miShellLifeField->add("10 Seconds", 620);
    miShellLifeField->add("15 Seconds", 930);
    miShellLifeField->add("20 Seconds", 1240);
    miShellLifeField->add("25 Seconds", 1550);
    miShellLifeField->add("30 Seconds", 1860);
    miShellLifeField->setOutputPtr(&game_values.shellttl);
    miShellLifeField->setCurrentValue(game_values.shellttl);

    miWandFreezeTimeField = new MI_SelectField<short>(&rm->spr_selectfield, 10, 400, "Freeze", 305, 120);
    miWandFreezeTimeField->add("1 Second", 62);
    miWandFreezeTimeField->add("2 Seconds", 124);
    miWandFreezeTimeField->add("3 Seconds", 186);
    miWandFreezeTimeField->add("4 Seconds", 248);
    miWandFreezeTimeField->add("5 Seconds", 310);
    miWandFreezeTimeField->add("6 Seconds", 372);
    miWandFreezeTimeField->add("7 Seconds", 434);
    miWandFreezeTimeField->add("8 Seconds", 496);
    miWandFreezeTimeField->add("9 Seconds", 558);
    miWandFreezeTimeField->add("10 Seconds", 620);
    miWandFreezeTimeField->add("12 Seconds", 744);
    miWandFreezeTimeField->add("15 Seconds", 930);
    miWandFreezeTimeField->add("18 Seconds", 1116);
    miWandFreezeTimeField->add("20 Seconds", 1240);
    miWandFreezeTimeField->setOutputPtr(&game_values.wandfreezetime);
    miWandFreezeTimeField->setCurrentValue(game_values.wandfreezetime);

    miBlueBlockLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 325, 230, "Blue Life", 305, 120);
    miBlueBlockLifeField->add("Unlimited", 0);
    miBlueBlockLifeField->add("1 Second", 62);
    miBlueBlockLifeField->add("2 Seconds", 124);
    miBlueBlockLifeField->add("3 Seconds", 186);
    miBlueBlockLifeField->add("4 Seconds", 248);
    miBlueBlockLifeField->add("5 Seconds", 310);
    miBlueBlockLifeField->add("6 Seconds", 372);
    miBlueBlockLifeField->add("7 Seconds", 434);
    miBlueBlockLifeField->add("8 Seconds", 496);
    miBlueBlockLifeField->add("9 Seconds", 558);
    miBlueBlockLifeField->add("10 Seconds", 620);
    miBlueBlockLifeField->add("15 Seconds", 930);
    miBlueBlockLifeField->add("20 Seconds", 1240);
    miBlueBlockLifeField->add("25 Seconds", 1550);
    miBlueBlockLifeField->add("30 Seconds", 1860);
    miBlueBlockLifeField->setOutputPtr(&game_values.blueblockttl);
    miBlueBlockLifeField->setCurrentValue(game_values.blueblockttl);

    miGrayBlockLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 325, 270, "Gray Life", 305, 120);
    miGrayBlockLifeField->add("Unlimited", 0);
    miGrayBlockLifeField->add("1 Second", 62);
    miGrayBlockLifeField->add("2 Seconds", 124);
    miGrayBlockLifeField->add("3 Seconds", 186);
    miGrayBlockLifeField->add("4 Seconds", 248);
    miGrayBlockLifeField->add("5 Seconds", 310);
    miGrayBlockLifeField->add("6 Seconds", 372);
    miGrayBlockLifeField->add("7 Seconds", 434);
    miGrayBlockLifeField->add("8 Seconds", 496);
    miGrayBlockLifeField->add("9 Seconds", 558);
    miGrayBlockLifeField->add("10 Seconds", 620);
    miGrayBlockLifeField->add("15 Seconds", 930);
    miGrayBlockLifeField->add("20 Seconds", 1240);
    miGrayBlockLifeField->add("25 Seconds", 1550);
    miGrayBlockLifeField->add("30 Seconds", 1860);
    miGrayBlockLifeField->setOutputPtr(&game_values.grayblockttl);
    miGrayBlockLifeField->setCurrentValue(game_values.grayblockttl);

    miRedBlockLifeField = new MI_SelectField<short>(&rm->spr_selectfield, 325, 310, "Red Life", 305, 120);
    miRedBlockLifeField->add("Unlimited", 0);
    miRedBlockLifeField->add("1 Second", 62);
    miRedBlockLifeField->add("2 Seconds", 124);
    miRedBlockLifeField->add("3 Seconds", 186);
    miRedBlockLifeField->add("4 Seconds", 248);
    miRedBlockLifeField->add("5 Seconds", 310);
    miRedBlockLifeField->add("6 Seconds", 372);
    miRedBlockLifeField->add("7 Seconds", 434);
    miRedBlockLifeField->add("8 Seconds", 496);
    miRedBlockLifeField->add("9 Seconds", 558);
    miRedBlockLifeField->add("10 Seconds", 620);
    miRedBlockLifeField->add("15 Seconds", 930);
    miRedBlockLifeField->add("20 Seconds", 1240);
    miRedBlockLifeField->add("25 Seconds", 1550);
    miRedBlockLifeField->add("30 Seconds", 1860);
    miRedBlockLifeField->setOutputPtr(&game_values.redblockttl);
    miRedBlockLifeField->setCurrentValue(game_values.redblockttl);

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

    setInitialFocus(miFireballLifeField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};
