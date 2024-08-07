#include "MI_FrenzyModeOptions.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"

extern CGameValues game_values;
extern CResourceManager* rm;

/**************************************
 * MI_FrenzyModeOptions Class
 **************************************/

//Rearrange display of powerups
short iFrenzyCardPositionMap[NUMFRENZYCARDS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

MI_FrenzyModeOptions::MI_FrenzyModeOptions(short x, short y, short width, short numlines) :
    UI_Control(x, y)
{
    iWidth = width;
    iNumLines = numlines;

    iIndex = 0;
    iOffset = 0;

    iTopStop = ((iNumLines - 1) >> 1) + 3;  // Plus 3 for the 3 fields at the top
    iBottomStop = ((NUMFRENZYCARDS + 1) >> 1) - iNumLines + iTopStop;

    mMenu = new UI_Menu();

    miQuantityField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 40, "Limit", 400, 180);
    miQuantityField->add("Single Powerup", 0);
    miQuantityField->add("1 Powerup", 1);
    miQuantityField->add("2 Powerups", 2);
    miQuantityField->add("3 Powerups", 3);
    miQuantityField->add("4 Powerups", 4);
    miQuantityField->add("5 Powerups", 5);
    miQuantityField->add("# Players - 1", 6);
    miQuantityField->add("# Players", 7);
    miQuantityField->add("# Players + 1", 8, false);
    miQuantityField->add("# Players + 2", 9, false);
    miQuantityField->add("# Players + 3", 10, false);
    miQuantityField->setOutputPtr(&game_values.gamemodemenusettings.frenzy.quantity);
    miQuantityField->setCurrentValue(game_values.gamemodemenusettings.frenzy.quantity);

    miRateField = new MI_SelectField<short>(&rm->spr_selectfield, 120, 80, "Rate", 400, 180);
    miRateField->add("Instant", 0);
    miRateField->add("1 Second", 62);
    miRateField->add("2 Seconds", 124);
    miRateField->add("3 Seconds", 186);
    miRateField->add("5 Seconds", 310);
    miRateField->add("10 Seconds", 620, false);
    miRateField->add("15 Seconds", 930, false);
    miRateField->add("20 Seconds", 1240, false);
    miRateField->add("25 Seconds", 1550, false);
    miRateField->add("30 Seconds", 1860, false);
    miRateField->setOutputPtr(&game_values.gamemodemenusettings.frenzy.rate);
    miRateField->setCurrentValue(game_values.gamemodemenusettings.frenzy.rate);

    miStoredShellsField = new MI_SelectField<bool>(&rm->spr_selectfield, 120, 120, "Store Shells", 400, 180);
    miStoredShellsField->add("Off", false);
    miStoredShellsField->add("On", true);
    miStoredShellsField->setOutputPtr(&game_values.gamemodemenusettings.frenzy.storedshells);
    miStoredShellsField->setCurrentValue(game_values.gamemodemenusettings.frenzy.storedshells ? 1 : 0);
    miStoredShellsField->setAutoAdvance(true);

    short iPowerupMap[] = {8, 5, 11, 17, 19, 21, 23, 24, 25, 20, 9, 16, 10, 22, 12, 13, 14, 15, 27};
    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        miPowerupSlider[iPowerup] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->spr_storedpoweruplarge, iPowerup < 10 ? 65 : 330, 0, 245, iPowerupMap[iPowerup]);
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
        miPowerupSlider[iPowerup]->setOutputPtr(&game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup]);
        miPowerupSlider[iPowerup]->setCurrentValue(game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup]);
    }

    miBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miUpArrow = new MI_Image(&rm->menu_verticalarrows, 310, 162, 20, 0, 20, 20, 1, 4, 8);
    miDownArrow = new MI_Image(&rm->menu_verticalarrows, 310, 402, 0, 0, 20, 20, 1, 4, 8);
    miUpArrow->setVisible(false);

    mMenu->AddControl(miQuantityField, NULL, miRateField, NULL, NULL);
    mMenu->AddControl(miRateField, miQuantityField, miStoredShellsField, NULL, NULL);
    mMenu->AddControl(miStoredShellsField, miRateField, miPowerupSlider[0], NULL, NULL);

    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        UI_Control * upcontrol = NULL;
        if (iPowerup == 0)
            upcontrol = miStoredShellsField;
        else
            upcontrol = miPowerupSlider[iPowerup - 2];

        UI_Control * lDownControl1 = NULL;
        if (iPowerup >= NUMFRENZYCARDS - 2)
            lDownControl1 = miBackButton;
        else
            lDownControl1 = miPowerupSlider[iPowerup + 2];

        UI_Control * rightcontrol = NULL;
        if (iPowerup + 1 < NUMFRENZYCARDS)
            rightcontrol = miPowerupSlider[iPowerup + 1];
        else
            rightcontrol = miBackButton;

        mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, lDownControl1, NULL, rightcontrol);

        if (++iPowerup < NUMFRENZYCARDS) {
            upcontrol = NULL;
            if (iPowerup == 1)
                upcontrol = miStoredShellsField;
            else
                upcontrol = miPowerupSlider[iPowerup - 2];

            UI_Control * lDownControl2 = NULL;
            if (iPowerup >= NUMFRENZYCARDS - 2)
                lDownControl2 = miBackButton;
            else
                lDownControl2 = miPowerupSlider[iPowerup + 2];

            mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, lDownControl2, miPowerupSlider[iPowerup - 1], NULL);
        }
    }

    //Setup positions and visible powerups
    SetupPowerupFields();

    mMenu->AddNonControl(miUpArrow);
    mMenu->AddNonControl(miDownArrow);

    mMenu->AddControl(miBackButton, miPowerupSlider[NUMFRENZYCARDS - 1], NULL, miPowerupSlider[NUMFRENZYCARDS - 1], NULL);

    mMenu->setInitialFocus(miQuantityField);
    mMenu->SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);
}

MI_FrenzyModeOptions::~MI_FrenzyModeOptions()
{
    delete mMenu;
}

void MI_FrenzyModeOptions::SetupPowerupFields()
{
    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        short iPosition = iFrenzyCardPositionMap[iPowerup];
        MI_PowerupSlider * slider = miPowerupSlider[iPosition];

        if ((iPosition >> 1) < iOffset || (iPosition >> 1) >= iOffset + iNumLines)
            slider->setVisible(false);
        else {
            slider->setVisible(true);
            slider->SetPosition(m_pos.x + (iPosition % 2) * 295, m_pos.y + 118 + 38 * (iPosition / 2 - iOffset));
        }
    }
}

MenuCodeEnum MI_FrenzyModeOptions::Modify(bool modify)
{
    mMenu->ResetMenu();
    iOffset = 0;
    iIndex = 0;
    SetupPowerupFields();
    AdjustDisplayArrows();

    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_FrenzyModeOptions::SendInput(CPlayerInput * playerInput)
{
    UI_Control * prevControl = mMenu->currentFocus();

    MenuCodeEnum ret = mMenu->SendInput(playerInput);

    UI_Control * nextControl = mMenu->currentFocus();

    if (MENU_CODE_CANCEL_INPUT == ret) {
        fModifying = false;
        return MENU_CODE_UNSELECT_ITEM;
    } else if (MENU_CODE_NEIGHBOR_UP == ret) {
        if (prevControl != miBackButton)
            MovePrev();
    } else if (MENU_CODE_NEIGHBOR_DOWN == ret) {
        if (nextControl != miBackButton || prevControl == miPowerupSlider[NUMFRENZYCARDS - 2])
            MoveNext();
    }

    return ret;
}

void MI_FrenzyModeOptions::Update()
{
    mMenu->Update();
}

void MI_FrenzyModeOptions::Draw()
{
    if (!m_visible)
        return;

    mMenu->Draw();
}

void MI_FrenzyModeOptions::MoveNext()
{
    iIndex++;

    if (iIndex > iTopStop && iIndex <= iBottomStop) {
        iOffset++;
        SetupPowerupFields();
    }

    AdjustDisplayArrows();
}

void MI_FrenzyModeOptions::MovePrev()
{
    iIndex--;

    if (iIndex >= iTopStop && iIndex < iBottomStop) {
        iOffset--;
        SetupPowerupFields();
    }

    AdjustDisplayArrows();
}

void MI_FrenzyModeOptions::SetRandomGameModeSettings()
{
    game_values.gamemodesettings.frenzy.quantity = miQuantityField->randomValue();
    game_values.gamemodesettings.frenzy.rate = miRateField->randomValue();
    game_values.gamemodesettings.frenzy.storedshells = miStoredShellsField->randomValue();

    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
        game_values.gamemodesettings.frenzy.powerupweight[iPowerup] = miPowerupSlider[iPowerup]->randomValue();
}

void MI_FrenzyModeOptions::AdjustDisplayArrows()
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

MenuCodeEnum MI_FrenzyModeOptions::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    //Loop through all controls to see if one was clicked on
    MenuCodeEnum ret = mMenu->MouseClick(iMouseX, iMouseY);

    if (ret == MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS)
        return ret;

    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        MI_PowerupSlider * slider = miPowerupSlider[iPowerup];

        if (slider == mMenu->currentFocus()) {
            iIndex = (iPowerup >> 1) + 3;

            if (iIndex <= iTopStop)
                iOffset = 0;
            else if (iIndex >= iBottomStop)
                iOffset = iBottomStop - iTopStop;
            else
                iOffset = iIndex - iTopStop;

            SetupPowerupFields();
            AdjustDisplayArrows();

            return ret;
        }
    }

    iOffset = 0;
    iIndex = 0;
    SetupPowerupFields();
    AdjustDisplayArrows();

    return ret;
}

void MI_FrenzyModeOptions::Refresh()
{
    mMenu->Refresh();
}
