#include "ScreenSettingsMenu.h"

#include "ResourceManager.h"

extern CResourceManager* rm;

UI_ScreenSettingsMenu::UI_ScreenSettingsMenu() : UI_Menu()
{
	MI_Button * miScreenResizeButton;
    miScreenResizeButton = new MI_Button(&rm->spr_selectfield, 70, 160, "Resize Screen", 500, 0);
    miScreenResizeButton->SetCode(MENU_CODE_TO_SCREEN_RESIZE);

    miScreenHardwareFilterField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, 70, 200, "Screen Filter", 500, 220);
    //miScreenHardwareFilterField->add("None", 0);
    miScreenHardwareFilterField->add("Point", 1);
    miScreenHardwareFilterField->add("Bilinear", 2);
    miScreenHardwareFilterField->add("Trilinear", 3);
    miScreenHardwareFilterField->add("Anisotrpoic", 4);
    miScreenHardwareFilterField->add("Quincunx", 5);
    miScreenHardwareFilterField->add("Gaussian Cubic", 6);
    miScreenHardwareFilterField->setOutputPtr(&game_values.hardwarefilter);
    miScreenHardwareFilterField->setCurrenzValue(game_values.hardwarefilter);
    miScreenHardwareFilterField->setItemChangedCode(MENU_CODE_SCREEN_FILTER_CHANGED);

    miScreenFlickerFilterField = new MI_SelectFieldDyn<short>(&rm->spr_selectfield, &rm->menu_slider_bar, 70, 240, "Flicker Filter", 500, 220, 380);
    miScreenFlickerFilterField->add("0", 0);
    miScreenFlickerFilterField->add("1", 1);
    miScreenFlickerFilterField->add("2", 2);
    miScreenFlickerFilterField->add("3", 3);
    miScreenFlickerFilterField->add("4", 4);
    miScreenFlickerFilterField->add("5", 5);
    miScreenFlickerFilterField->setOutputPtr(&game_values.flickerfilter);
    miScreenFlickerFilterField->setCurrenzValue(game_values.flickerfilter);
    miScreenFlickerFilterField->allowWrap(false);
    miScreenFlickerFilterField->setItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);

    miScreenSoftFilterField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 70, 280, "Soften Filter", 500, 220);
    miScreenSoftFilterField->add("Off", false);
    miScreenSoftFilterField->add("On", true);
    miScreenSoftFilterField->setOutputPtr(&game_values.softfilter);
    miScreenSoftFilterField->setCurrenzValue(game_values.softfilter);
    miScreenSoftFilterField->setAutoAdvance(true);
    miScreenSoftFilterField->setItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);

    /*
    miScreenAspectRatioField = new MI_SelectFieldDyn<bool>(&rm->spr_selectfield, 70, 300, "10x11 Aspect", 500, 220);
    miScreenAspectRatioField->add("Off", false);
    miScreenAspectRatioField->add("On", true);
    miScreenAspectRatioField->setOutputPtr(&game_values.aspectratio10x11);
    miScreenAspectRatioField->setCurrenzValue(game_values.aspectratio10x11);
    miScreenAspectRatioField->setAutoAdvance(true);
    miScreenAspectRatioField->setItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);
    */

    miScreenSettingsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miScreenSettingsMenuBackButton->SetCode(MENU_CODE_BACK_TO_GRAPHIC_OPTIONS_MENU);

    miScreenSettingsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miScreenSettingsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miScreenSettingsMenuHeaderText = new MI_Text("Screen Settings Menu", 320, 5, 0, 2, 1);

    AddNonControl(miScreenSettingsMenuLeftHeaderBar);
    AddNonControl(miScreenSettingsMenuRightHeaderBar);
    AddNonControl(miScreenSettingsMenuHeaderText);

    AddControl(miScreenResizeButton, miScreenSettingsMenuBackButton, miScreenHardwareFilterField, NULL, miScreenSettingsMenuBackButton);
    AddControl(miScreenHardwareFilterField, miScreenResizeButton, miScreenFlickerFilterField, NULL, miScreenSettingsMenuBackButton);
    AddControl(miScreenFlickerFilterField, miScreenHardwareFilterField, miScreenSoftFilterField, NULL, miScreenSettingsMenuBackButton);
    AddControl(miScreenSoftFilterField, miScreenFlickerFilterField, miScreenSettingsMenuBackButton, NULL, miScreenSettingsMenuBackButton);
    //AddControl(miScreenAspectRatioField, miScreenSoftFilterField, miScreenSettingsMenuBackButton, NULL, miScreenSettingsMenuBackButton);
    AddControl(miScreenSettingsMenuBackButton, miScreenSoftFilterField, miScreenResizeButton, miScreenSoftFilterField, NULL);

    SetHeadControl(miScreenResizeButton);
    SetCancelCode(MENU_CODE_BACK_TO_GRAPHIC_OPTIONS_MENU);
};

UI_ScreenSettingsMenu::~UI_ScreenSettingsMenu() {
}
