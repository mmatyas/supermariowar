#ifndef UI_FRENZY_MODE_OPTIONS_H
#define UI_FRENZY_MODE_OPTIONS_H

#include "uimenu.h"
#include "uicontrol.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_PowerupSlider.h"

class MI_Button;

class MI_FrenzyModeOptions : public UI_Control
{
public:

    MI_FrenzyModeOptions(short x, short y, short width, short numlines);
    ~MI_FrenzyModeOptions() override;

    MenuCodeEnum Modify(bool modify) override;
    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;

    void Update() override;
    void Draw() override;

    void MoveNext();
    void MovePrev();

    void SetRandomGameModeSettings();

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY) override;

    void Refresh() override;

private:

    void SetupPowerupFields();
    void AdjustDisplayArrows();

    short iIndex, iOffset;
    short iTopStop, iBottomStop;
    short iNumLines;
    short iWidth;

    UI_Menu * mMenu;

    MI_SelectField<short> * miQuantityField;
    MI_SelectField<short> * miRateField;
    MI_SelectField<bool>* miStoredShellsField;
    MI_PowerupSlider * miPowerupSlider[NUMFRENZYCARDS];
    MI_Button * miBackButton;

    MI_Image * miUpArrow;
    MI_Image * miDownArrow;
};

#endif // UI_FRENZY_MODE_OPTIONS_H
