#include "MI_FrenzyModeOptions.h"

#include "GameValues.h"
#include "ResourceManager.h"

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

    miQuantityField = new MI_SelectField(&rm->spr_selectfield, 120, 40, "Limit", 400, 180);
    miQuantityField->Add("Single Powerup", 0, "", false, false);
    miQuantityField->Add("1 Powerup", 1, "", false, false);
    miQuantityField->Add("2 Powerups", 2, "", false, false);
    miQuantityField->Add("3 Powerups", 3, "", false, false);
    miQuantityField->Add("4 Powerups", 4, "", false, false);
    miQuantityField->Add("5 Powerups", 5, "", false, false);
    miQuantityField->Add("# Players - 1", 6, "", false, false);
    miQuantityField->Add("# Players", 7, "", false, false);
    miQuantityField->Add("# Players + 1", 8, "", false, false, false);
    miQuantityField->Add("# Players + 2", 9, "", false, false, false);
    miQuantityField->Add("# Players + 3", 10, "", false, false, false);
    miQuantityField->SetData(&game_values.gamemodemenusettings.frenzy.quantity, NULL, NULL);
    miQuantityField->SetKey(game_values.gamemodemenusettings.frenzy.quantity);

    miRateField = new MI_SelectField(&rm->spr_selectfield, 120, 80, "Rate", 400, 180);
    miRateField->Add("Instant", 0, "", false, false);
    miRateField->Add("1 Second", 62, "", false, false);
    miRateField->Add("2 Seconds", 124, "", false, false);
    miRateField->Add("3 Seconds", 186, "", false, false);
    miRateField->Add("5 Seconds", 310, "", false, false);
    miRateField->Add("10 Seconds", 620, "", false, false, false);
    miRateField->Add("15 Seconds", 930, "", false, false, false);
    miRateField->Add("20 Seconds", 1240, "", false, false, false);
    miRateField->Add("25 Seconds", 1550, "", false, false, false);
    miRateField->Add("30 Seconds", 1860, "", false, false, false);
    miRateField->SetData(&game_values.gamemodemenusettings.frenzy.rate, NULL, NULL);
    miRateField->SetKey(game_values.gamemodemenusettings.frenzy.rate);

    miStoredShellsField = new MI_SelectField(&rm->spr_selectfield, 120, 120, "Store Shells", 400, 180);
    miStoredShellsField->Add("Off", 0, "", false, false);
    miStoredShellsField->Add("On", 1, "", true, false);
    miStoredShellsField->SetData(NULL, NULL, &game_values.gamemodemenusettings.frenzy.storedshells);
    miStoredShellsField->SetKey(game_values.gamemodemenusettings.frenzy.storedshells ? 1 : 0);
    miStoredShellsField->SetAutoAdvance(true);

    short iPowerupMap[] = {8, 5, 11, 17, 19, 21, 23, 24, 25, 20, 9, 16, 10, 22, 12, 13, 14, 15, 27};
    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
        miPowerupSlider[iPowerup] = new MI_PowerupSlider(&rm->spr_selectfield, &rm->menu_slider_bar, &rm->spr_storedpoweruplarge, iPowerup < 10 ? 65 : 330, 0, 245, iPowerupMap[iPowerup]);
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
        miPowerupSlider[iPowerup]->SetData(&game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup], NULL, NULL);
        miPowerupSlider[iPowerup]->SetKey(game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup]);
    }

    miBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    miUpArrow = new MI_Image(&rm->menu_verticalarrows, 310, 162, 20, 0, 20, 20, 1, 4, 8);
    miDownArrow = new MI_Image(&rm->menu_verticalarrows, 310, 402, 0, 0, 20, 20, 1, 4, 8);
    miUpArrow->Show(false);

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

    mMenu->SetHeadControl(miQuantityField);
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
            slider->Show(false);
        else {
            slider->Show(true);
            slider->SetPosition(ix + (iPosition % 2) * 295, iy + 118 + 38 * (iPosition / 2 - iOffset));
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
    UI_Control * prevControl = mMenu->GetCurrentControl();

    MenuCodeEnum ret = mMenu->SendInput(playerInput);

    UI_Control * nextControl = mMenu->GetCurrentControl();

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
    if (!fShow)
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
    game_values.gamemodesettings.frenzy.quantity = miQuantityField->GetRandomShortValue();
    game_values.gamemodesettings.frenzy.rate = miRateField->GetRandomShortValue();
    game_values.gamemodesettings.frenzy.storedshells = miStoredShellsField->GetRandomBoolValue();

    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
        game_values.gamemodesettings.frenzy.powerupweight[iPowerup] = miPowerupSlider[iPowerup]->GetRandomShortValue();
}

void MI_FrenzyModeOptions::AdjustDisplayArrows()
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

        if (slider == mMenu->GetCurrentControl()) {
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