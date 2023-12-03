#pragma once

#include "uicontrol.h"
#include "GlobalConstants.h"

#include <array>
#include <memory>

class MI_Button;
class MI_Image;
class MI_PowerupSlider;
class MI_SelectField;
class MI_Text;
class UI_Menu;


class MI_PowerupSelection : public UI_Control {
public:
    MI_PowerupSelection(short x, short y, short width, short numlines);

    MenuCodeEnum Modify(bool modify) override;
    MenuCodeEnum SendInput(CPlayerInput* playerInput) override;

    void Update() override;
    void Draw() override;

    void MoveNext();
    void MovePrev();

private:
    void AdjustDisplayArrows();

    void SetupPowerupFields();
    void EnablePowerupFields(bool fEnable);

    short iIndex = 0;
    short iOffset = 0;
    short iNumLines = 0;
    short iTopStop = 0;
    short iBottomStop = 0;

    // TODO: Ownership of most members gets passed to this menu
    std::unique_ptr<UI_Menu> mMenu;

    MI_SelectField* miOverride = nullptr;
    MI_SelectField* miPreset = nullptr;

    std::array<MI_PowerupSlider*, NUM_POWERUPS> miPowerupSlider;

    MI_Button* miRestoreDefaultsButton = nullptr;
    MI_Button* miClearButton = nullptr;

    MI_Image* miDialogImage = nullptr;
    MI_Text* miDialogAreYouText = nullptr;
    MI_Text* miDialogSureText = nullptr;
    MI_Button* miDialogYesButton = nullptr;
    MI_Button* miDialogNoButton = nullptr;

    MI_Image* miUpArrow = nullptr;
    MI_Image* miDownArrow = nullptr;
};
