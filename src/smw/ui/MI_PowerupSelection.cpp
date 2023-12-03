#include "MI_PowerupSelection.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_PowerupSlider.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"
#include "uimenu.h"

extern CResourceManager* rm;
extern CGameValues game_values;
extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];
extern short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

namespace {
//Rearrange display of powerups
//short iPowerupDisplayMap[NUM_POWERUPS] = { 4, 0, 1, 2, 3, 6, 10, 12, 11, 14, 13, 7, 16, 17, 18, 19, 15, 9, 5, 8, 20, 21, 22, 23, 24, 25};
//                                          0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
short iPowerupPositionMap[NUM_POWERUPS] = { 1, 0, 2, 6, 3, 8, 4,20,18, 7, 5,10,11,22,19, 9,23,16,21,12,17,13,24,15,25,14};

} // namespace

/*
0 == poison mushroom
1 == 1up
2 == 2up
3 == 3up
4 == 5up
5 == flower
6 == star
7 == clock
8 == bobomb
9 == pow
10 == bulletbill
11 == hammer
12 == green shell
13 == red shell
14 == spike shell
15 == buzzy shell
16 == mod
17 == feather
18 == mystery mushroom
19 == boomerang
20 == tanooki
21 == ice wand
22 == podoboo
23 == bombs
24 == leaf
25 == pwings
*/

/*
0 1UP     Poison
2 2UP     Star
4 3UP     Bob-Omb
6 5UP     Tanooki
8 Mystery Clock
10 Flower  B.Bill
12 Hammer  P'Boo
14 B'Rang  POW
16 Bomb    MOd
18 Wand    G.Shell
20 Feather R.Shell
22 Leaf    B.Shell
24 P-Wing  S.Shell
*/

MI_PowerupSelection::MI_PowerupSelection(short x, short y, short width, short numlines)
    : UI_Control(x, y)
    , iNumLines(numlines)
    , iTopStop(((iNumLines - 1) / 2) + 2)
    , iBottomStop((NUM_POWERUPS / 2) - iNumLines + iTopStop)
{
    mMenu = std::make_unique<UI_Menu>();

    miOverride = new MI_SelectField(&rm->spr_selectfield, 70, iy, "Use Settings From", 500, 250);
    miOverride->Add("Map Only", 0, "", false, false);
    miOverride->Add("Game Only", 1, "", false, false);
    miOverride->Add("Basic Average", 2, "", false, false);
    miOverride->Add("Weighted Average", 3, "", false, false);
    miOverride->SetData(&game_values.overridepowerupsettings, NULL, NULL);
    miOverride->SetKey(game_values.overridepowerupsettings);
    //miOverride->SetItemChangedCode(MENU_CODE_POWERUP_OVERRIDE_CHANGED);

    miPreset = new MI_SelectField(&rm->spr_selectfield, 70, iy + 40, "Item Set", 500, 250);
    miPreset->Add("Custom Set 1", 0, "", false, false);
    miPreset->Add("Custom Set 2", 1, "", false, false);
    miPreset->Add("Custom Set 3", 2, "", false, false);
    miPreset->Add("Custom Set 4", 3, "", false, false);
    miPreset->Add("Custom Set 5", 4, "", false, false);
    miPreset->Add("Balanced Set", 5, "", false, false);
    miPreset->Add("Weapons Only", 6, "", false, false);
    miPreset->Add("Koopa Bros Weapons", 7, "", false, false);
    miPreset->Add("Support Items", 8, "", false, false);
    miPreset->Add("Booms and Shakes", 9, "", false, false);
    miPreset->Add("Fly and Glide", 10, "", false, false);
    miPreset->Add("Shells Only", 11, "", false, false);
    miPreset->Add("Mushrooms Only", 12, "", false, false);
    miPreset->Add("Super Mario Bros 1", 13, "", false, false);
    miPreset->Add("Super Mario Bros 2", 14, "", false, false);
    miPreset->Add("Super Mario Bros 3", 15, "", false, false);
    miPreset->Add("Super Mario World", 16, "", false, false);
    miPreset->SetData(&game_values.poweruppreset, NULL, NULL);
    miPreset->SetKey(game_values.poweruppreset);
    miPreset->SetItemChangedCode(MENU_CODE_POWERUP_PRESET_CHANGED);

    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        miPowerupSlider[iPowerup] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->spr_storedpoweruplarge, 0, 0, 245, iPowerupPositionMap[iPowerup]);
        miPowerupSlider[iPowerup]->Add("", 0, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 1, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 2, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 3, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 4, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 5, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 6, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 7, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 8, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 9, "", false, false);
        miPowerupSlider[iPowerup]->Add("", 10, "", false, false);
        miPowerupSlider[iPowerup]->SetNoWrap(true);
        miPowerupSlider[iPowerup]->SetData(&game_values.powerupweights[iPowerupPositionMap[iPowerup]], NULL, NULL);
        miPowerupSlider[iPowerup]->SetKey(game_values.powerupweights[iPowerupPositionMap[iPowerup]]);
        miPowerupSlider[iPowerup]->SetItemChangedCode(MENU_CODE_POWERUP_SETTING_CHANGED);
    }

    miRestoreDefaultsButton = new MI_Button(&rm->spr_selectfield, 160, 432, "Defaults", 150, TextAlign::CENTER);
    miRestoreDefaultsButton->SetCode(MENU_CODE_RESTORE_DEFAULT_POWERUP_WEIGHTS);

    miClearButton = new MI_Button(&rm->spr_selectfield, 330, 432, "Clear", 150, TextAlign::CENTER);
    miClearButton->SetCode(MENU_CODE_CLEAR_POWERUP_WEIGHTS);

    //Are You Sure dialog box
    miDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miDialogAreYouText = new MI_HeaderText("Are You", 320, 195);
    miDialogSureText = new MI_HeaderText("Sure?", 320, 220);
    miDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, TextAlign::CENTER);
    miDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, TextAlign::CENTER);

    miDialogYesButton->SetCode(MENU_CODE_POWERUP_RESET_YES);
    miDialogNoButton->SetCode(MENU_CODE_POWERUP_RESET_NO);

    miDialogImage->Show(false);
    miDialogAreYouText->Show(false);
    miDialogSureText->Show(false);
    miDialogYesButton->Show(false);
    miDialogNoButton->Show(false);

    mMenu->AddControl(miOverride, NULL, miPreset, NULL, NULL);
    mMenu->AddControl(miPreset, miOverride, miPowerupSlider[0], NULL, NULL);

    miUpArrow = new MI_Image(&rm->menu_verticalarrows, 310, 128, 20, 0, 20, 20, 1, 4, 8);
    miDownArrow = new MI_Image(&rm->menu_verticalarrows, 310, 406, 0, 0, 20, 20, 1, 4, 8);
    miUpArrow->Show(false);

    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        UI_Control * upcontrol = NULL;
        if (iPowerup == 0)
            upcontrol = miPreset;
        else
            upcontrol = miPowerupSlider[iPowerup - 2];

        UI_Control * downcontrol = NULL;
        if (iPowerup >= NUM_POWERUPS - 2)
            downcontrol = miRestoreDefaultsButton;
        else
            downcontrol = miPowerupSlider[iPowerup + 2];

        mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, downcontrol, NULL, miPowerupSlider[iPowerup + 1]);

        if (++iPowerup < NUM_POWERUPS) {
            upcontrol = NULL;
            if (iPowerup == 1)
                upcontrol = miPreset;
            else
                upcontrol = miPowerupSlider[iPowerup - 2];

            UI_Control * downcontrol = NULL;
            if (iPowerup >= NUM_POWERUPS - 2)
                downcontrol = miClearButton;
            else
                downcontrol = miPowerupSlider[iPowerup + 2];

            mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, downcontrol, miPowerupSlider[iPowerup - 1], NULL);
        }
    }

    mMenu->AddControl(miRestoreDefaultsButton, miPowerupSlider[NUM_POWERUPS - 2], NULL, NULL, miClearButton);
    mMenu->AddControl(miClearButton, miPowerupSlider[NUM_POWERUPS - 1], NULL, miRestoreDefaultsButton, NULL);

    //Setup positions and visible powerups
    SetupPowerupFields();

    //Set enabled based on if we are overriding or not
    //EnablePowerupFields();

    mMenu->AddNonControl(miDialogImage);
    mMenu->AddNonControl(miDialogAreYouText);
    mMenu->AddNonControl(miDialogSureText);

    mMenu->AddControl(miDialogYesButton, NULL, NULL, NULL, miDialogNoButton);
    mMenu->AddControl(miDialogNoButton, NULL, NULL, miDialogYesButton, NULL);

    mMenu->AddNonControl(miUpArrow);
    mMenu->AddNonControl(miDownArrow);

    mMenu->SetHeadControl(miOverride);
    mMenu->SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
}

void MI_PowerupSelection::SetupPowerupFields()
{
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        short iPosition = iPowerupPositionMap[iPowerup];
        MI_PowerupSlider * slider = miPowerupSlider[iPosition];

        if ((iPosition >> 1) < iOffset || (iPosition >> 1) >= iOffset + iNumLines)
            slider->Show(false);
        else {
            slider->Show(true);
            slider->SetPosition(ix + (iPosition % 2) * 295, iy + 84 + 38 * (iPosition / 2 - iOffset));
        }
    }
}

void MI_PowerupSelection::EnablePowerupFields(bool fEnable)
{
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        miPowerupSlider[iPowerup]->Disable(!fEnable);
    }

    miPreset->SetNeighbor(1, fEnable ? miPowerupSlider[0] : NULL);
}

MenuCodeEnum MI_PowerupSelection::Modify(bool modify)
{
    mMenu->ResetMenu();
    iOffset = 0;
    iIndex = 0;
    SetupPowerupFields();

    //EnablePowerupFields(game_values.poweruppreset >= 1 && game_values.poweruppreset <= 3);

    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_PowerupSelection::SendInput(CPlayerInput * playerInput)
{
    MenuCodeEnum ret = mMenu->SendInput(playerInput);

    if (MENU_CODE_CANCEL_INPUT == ret) {
        fModifying = false;
        return MENU_CODE_UNSELECT_ITEM;
    } else if (MENU_CODE_POWERUP_PRESET_CHANGED == ret) {
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
            short iCurrentValue = g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]];
            miPowerupSlider[iPowerup]->SetKey(iCurrentValue);
            game_values.powerupweights[iPowerupPositionMap[iPowerup]] = iCurrentValue;
        }

        //If it is a custom preset, then allow modification
        //EnablePowerupFields(game_values.poweruppreset >= 1 && game_values.poweruppreset <= 3);
    } else if (MENU_CODE_POWERUP_SETTING_CHANGED == ret) {
        //Update the custom presets
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
            g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]] = game_values.powerupweights[iPowerupPositionMap[iPowerup]];
    } else if (MENU_CODE_RESTORE_DEFAULT_POWERUP_WEIGHTS == ret || MENU_CODE_CLEAR_POWERUP_WEIGHTS == ret) {
        miDialogImage->Show(true);
        miDialogAreYouText->Show(true);
        miDialogSureText->Show(true);
        miDialogYesButton->Show(true);
        miDialogNoButton->Show(true);

        mMenu->RememberCurrent();

        mMenu->SetHeadControl(miDialogNoButton);
        mMenu->SetCancelCode(MENU_CODE_POWERUP_RESET_NO);
        mMenu->ResetMenu();

        if (MENU_CODE_CLEAR_POWERUP_WEIGHTS == ret)
            miDialogYesButton->SetCode(MENU_CODE_POWERUP_CLEAR_YES);
        else
            miDialogYesButton->SetCode(MENU_CODE_POWERUP_RESET_YES);

    } else if (MENU_CODE_POWERUP_RESET_YES == ret || MENU_CODE_POWERUP_RESET_NO == ret || MENU_CODE_POWERUP_CLEAR_YES == ret) {
        miDialogImage->Show(false);
        miDialogAreYouText->Show(false);
        miDialogSureText->Show(false);
        miDialogYesButton->Show(false);
        miDialogNoButton->Show(false);

        mMenu->SetHeadControl(miOverride);
        mMenu->SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

        mMenu->RestoreCurrent();

        if (MENU_CODE_POWERUP_RESET_YES == ret) {
            //restore default powerup weights for powerup selection menu
            for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
                short iDefaultValue = g_iDefaultPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]];
                miPowerupSlider[iPowerup]->SetKey(iDefaultValue);
                game_values.powerupweights[iPowerupPositionMap[iPowerup]] = iDefaultValue;

                g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]] = iDefaultValue;
            }
        } else if (MENU_CODE_POWERUP_CLEAR_YES == ret) {
            //restore default powerup weights for powerup selection menu
            for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
                miPowerupSlider[iPowerup]->SetKey(0);
                game_values.powerupweights[iPowerup] = 0;

                g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerup] = 0;
            }
        }
    } else if (MENU_CODE_NEIGHBOR_UP == ret) {
        MovePrev();
    } else if (MENU_CODE_NEIGHBOR_DOWN == ret) {
        MoveNext();
    }

    return ret;
}

void MI_PowerupSelection::Update()
{
    mMenu->Update();
}

void MI_PowerupSelection::Draw()
{
    if (!fShow)
        return;

    mMenu->Draw();
}

void MI_PowerupSelection::MoveNext()
{
    iIndex++;

    if (iIndex > iTopStop && iIndex <= iBottomStop) {
        iOffset++;
        SetupPowerupFields();
    }

    AdjustDisplayArrows();
}

void MI_PowerupSelection::MovePrev()
{
    iIndex--;

    if (iIndex >= iTopStop && iIndex < iBottomStop) {
        iOffset--;
        SetupPowerupFields();
    }

    AdjustDisplayArrows();
}

void MI_PowerupSelection::AdjustDisplayArrows()
{
    if (iIndex > iTopStop)
        miUpArrow->Show(true);
    else
        miUpArrow->Show(false);

    if (iIndex < iBottomStop)
        miDownArrow->Show(true);
    else
        miDownArrow->Show(false);
}
