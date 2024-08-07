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

    miOverride = new MI_SelectField<short>(&rm->spr_selectfield, 70, m_pos.y, "Use Settings From", 500, 250);
    miOverride->add("Map Only", 0);
    miOverride->add("Game Only", 1);
    miOverride->add("Basic Average", 2);
    miOverride->add("Weighted Average", 3);
    miOverride->setOutputPtr(&game_values.overridepowerupsettings);
    miOverride->setCurrentValue(game_values.overridepowerupsettings);
    //miOverride->SetItemChangedCode(MENU_CODE_POWERUP_OVERRIDE_CHANGED);

    miPreset = new MI_SelectField<short>(&rm->spr_selectfield, 70, m_pos.y + 40, "Item Set", 500, 250);
    miPreset->add("Custom Set 1", 0);
    miPreset->add("Custom Set 2", 1);
    miPreset->add("Custom Set 3", 2);
    miPreset->add("Custom Set 4", 3);
    miPreset->add("Custom Set 5", 4);
    miPreset->add("Balanced Set", 5);
    miPreset->add("Weapons Only", 6);
    miPreset->add("Koopa Bros Weapons", 7);
    miPreset->add("Support Items", 8);
    miPreset->add("Booms and Shakes", 9);
    miPreset->add("Fly and Glide", 10);
    miPreset->add("Shells Only", 11);
    miPreset->add("Mushrooms Only", 12);
    miPreset->add("Super Mario Bros 1", 13);
    miPreset->add("Super Mario Bros 2", 14);
    miPreset->add("Super Mario Bros 3", 15);
    miPreset->add("Super Mario World", 16);
    miPreset->setOutputPtr(&game_values.poweruppreset);
    miPreset->setCurrentValue(game_values.poweruppreset);
    miPreset->setItemChangedCode(MENU_CODE_POWERUP_PRESET_CHANGED);

    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        miPowerupSlider[iPowerup] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->spr_storedpoweruplarge, 0, 0, 245, iPowerupPositionMap[iPowerup]);
        miPowerupSlider[iPowerup]->add("", 0);
        miPowerupSlider[iPowerup]->add("", 1);
        miPowerupSlider[iPowerup]->add("", 2);
        miPowerupSlider[iPowerup]->add("", 3);
        miPowerupSlider[iPowerup]->add("", 4);
        miPowerupSlider[iPowerup]->add("", 5);
        miPowerupSlider[iPowerup]->add("", 6);
        miPowerupSlider[iPowerup]->add("", 7);
        miPowerupSlider[iPowerup]->add("", 8);
        miPowerupSlider[iPowerup]->add("", 9);
        miPowerupSlider[iPowerup]->add("", 10);
        miPowerupSlider[iPowerup]->allowWrap(false);
        miPowerupSlider[iPowerup]->setOutputPtr(&game_values.powerupweights[iPowerupPositionMap[iPowerup]]);
        miPowerupSlider[iPowerup]->setCurrentValue(game_values.powerupweights[iPowerupPositionMap[iPowerup]]);
        miPowerupSlider[iPowerup]->setItemChangedCode(MENU_CODE_POWERUP_SETTING_CHANGED);
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

    miDialogImage->setVisible(false);
    miDialogAreYouText->setVisible(false);
    miDialogSureText->setVisible(false);
    miDialogYesButton->setVisible(false);
    miDialogNoButton->setVisible(false);

    mMenu->AddControl(miOverride, NULL, miPreset, NULL, NULL);
    mMenu->AddControl(miPreset, miOverride, miPowerupSlider[0], NULL, NULL);

    miUpArrow = new MI_Image(&rm->menu_verticalarrows, 310, 128, 20, 0, 20, 20, 1, 4, 8);
    miDownArrow = new MI_Image(&rm->menu_verticalarrows, 310, 406, 0, 0, 20, 20, 1, 4, 8);
    miUpArrow->setVisible(false);

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

    mMenu->setInitialFocus(miOverride);
    mMenu->SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
}

void MI_PowerupSelection::SetupPowerupFields()
{
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        short iPosition = iPowerupPositionMap[iPowerup];
        MI_PowerupSlider * slider = miPowerupSlider[iPosition];

        if ((iPosition >> 1) < iOffset || (iPosition >> 1) >= iOffset + iNumLines)
            slider->setVisible(false);
        else {
            slider->setVisible(true);
            slider->SetPosition(m_pos.x + (iPosition % 2) * 295, m_pos.y + 84 + 38 * (iPosition / 2 - iOffset));
        }
    }
}

void MI_PowerupSelection::EnablePowerupFields(bool fEnable)
{
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        miPowerupSlider[iPowerup]->Disable(!fEnable);
    }

    miPreset->setNeighbor(MenuNavDirection::Down, fEnable ? miPowerupSlider[0] : NULL);
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

MenuCodeEnum MI_PowerupSelection::SendInput(CPlayerInput* playerInput)
{
    MenuCodeEnum ret = mMenu->SendInput(playerInput);

    if (MENU_CODE_CANCEL_INPUT == ret) {
        fModifying = false;
        return MENU_CODE_UNSELECT_ITEM;
    } else if (MENU_CODE_POWERUP_PRESET_CHANGED == ret) {
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
            short iCurrentValue = game_values.allPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]];
            miPowerupSlider[iPowerup]->setCurrentValue(iCurrentValue);
            game_values.powerupweights[iPowerupPositionMap[iPowerup]] = iCurrentValue;
        }

        //If it is a custom preset, then allow modification
        //EnablePowerupFields(game_values.poweruppreset >= 1 && game_values.poweruppreset <= 3);
    } else if (MENU_CODE_POWERUP_SETTING_CHANGED == ret) {
        //Update the custom presets
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
            game_values.allPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]] = game_values.powerupweights[iPowerupPositionMap[iPowerup]];
        }
    } else if (MENU_CODE_RESTORE_DEFAULT_POWERUP_WEIGHTS == ret || MENU_CODE_CLEAR_POWERUP_WEIGHTS == ret) {
        miDialogImage->setVisible(true);
        miDialogAreYouText->setVisible(true);
        miDialogSureText->setVisible(true);
        miDialogYesButton->setVisible(true);
        miDialogNoButton->setVisible(true);

        mMenu->RememberCurrent();

        mMenu->setInitialFocus(miDialogNoButton);
        mMenu->SetCancelCode(MENU_CODE_POWERUP_RESET_NO);
        mMenu->ResetMenu();

        if (MENU_CODE_CLEAR_POWERUP_WEIGHTS == ret)
            miDialogYesButton->SetCode(MENU_CODE_POWERUP_CLEAR_YES);
        else
            miDialogYesButton->SetCode(MENU_CODE_POWERUP_RESET_YES);

    } else if (MENU_CODE_POWERUP_RESET_YES == ret || MENU_CODE_POWERUP_RESET_NO == ret || MENU_CODE_POWERUP_CLEAR_YES == ret) {
        miDialogImage->setVisible(false);
        miDialogAreYouText->setVisible(false);
        miDialogSureText->setVisible(false);
        miDialogYesButton->setVisible(false);
        miDialogNoButton->setVisible(false);

        mMenu->setInitialFocus(miOverride);
        mMenu->SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

        mMenu->RestoreCurrent();

        if (MENU_CODE_POWERUP_RESET_YES == ret) {
            //restore default powerup weights for powerup selection menu
            for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
                short iDefaultValue = defaultPowerupSetting(game_values.poweruppreset, iPowerupPositionMap[iPowerup]);
                miPowerupSlider[iPowerup]->setCurrentValue(iDefaultValue);
                game_values.powerupweights[iPowerupPositionMap[iPowerup]] = iDefaultValue;

                game_values.allPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]] = iDefaultValue;
            }
        } else if (MENU_CODE_POWERUP_CLEAR_YES == ret) {
            //restore default powerup weights for powerup selection menu
            for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
                miPowerupSlider[iPowerup]->setCurrentValue(0);
                game_values.powerupweights[iPowerup] = 0;

                game_values.allPowerupPresets[game_values.poweruppreset][iPowerup] = 0;
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
    if (!m_visible)
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
        miUpArrow->setVisible(true);
    else
        miUpArrow->setVisible(false);

    if (iIndex < iBottomStop)
        miDownArrow->setVisible(true);
    else
        miDownArrow->setVisible(false);
}
