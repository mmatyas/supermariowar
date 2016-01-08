#ifndef UI_FRENZY_MODE_OPTIONS_H
#define UI_FRENZY_MODE_OPTIONS_H

#include "uimenu.h"
#include "uicontrol.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_PowerupSlider.h"

class MI_FrenzyModeOptions : public UI_Control
{
public:

    MI_FrenzyModeOptions(short x, short y, short width, short numlines);
    virtual ~MI_FrenzyModeOptions();

    MenuCodeEnum Modify(bool modify);
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

    void Update();
    void Draw();

    void MoveNext();
    void MovePrev();

    void SetRandomGameModeSettings();

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    void Refresh();

private:

    void SetupPowerupFields();
    void AdjustDisplayArrows();

    short iIndex, iOffset;
    short iTopStop, iBottomStop;
    short iNumLines;
    short iWidth;

    UI_Menu * mMenu;

    MI_SelectField * miQuantityField;
    MI_SelectField * miRateField;
    MI_SelectField * miStoredShellsField;
    MI_PowerupSlider * miPowerupSlider[NUMFRENZYCARDS];
    MI_Button * miBackButton;

    MI_Image * miUpArrow;
    MI_Image * miDownArrow;
};

#endif // UI_FRENZY_MODE_OPTIONS_H
