#include "ScreenSettingsMenu.h"

#include "ResourceManager.h"

extern CResourceManager* rm;

UI_ScreenSettingsMenu::UI_ScreenSettingsMenu() : UI_Menu()
{
	MI_Button * miScreenResizeButton;
    miScreenResizeButton = new MI_Button(&rm->spr_selectfield, 70, 160, "Resize Screen", 500, 0);
    miScreenResizeButton->SetCode(MENU_CODE_TO_SCREEN_RESIZE);

    miScreenHardwareFilterField = new MI_SelectField(&rm->spr_selectfield, 70, 200, "Screen Filter", 500, 220);
    //miScreenHardwareFilterField->Add("None", 0);
    miScreenHardwareFilterField->Add("Point", 1);
    miScreenHardwareFilterField->Add("Bilinear", 2);
    miScreenHardwareFilterField->Add("Trilinear", 3);
    miScreenHardwareFilterField->Add("Anisotrpoic", 4);
    miScreenHardwareFilterField->Add("Quincunx", 5);
    miScreenHardwareFilterField->Add("Gaussian Cubic", 6);
    miScreenHardwareFilterField->SetData(&game_values.hardwarefilter, NULL, NULL);
    miScreenHardwareFilterField->SetKey(game_values.hardwarefilter);
    miScreenHardwareFilterField->SetItemChangedCode(MENU_CODE_SCREEN_FILTER_CHANGED);

    miScreenFlickerFilterField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 70, 240, "Flicker Filter", 500, 220, 380);
    miScreenFlickerFilterField->Add("0", 0);
    miScreenFlickerFilterField->Add("1", 1);
    miScreenFlickerFilterField->Add("2", 2);
    miScreenFlickerFilterField->Add("3", 3);
    miScreenFlickerFilterField->Add("4", 4);
    miScreenFlickerFilterField->Add("5", 5);
    miScreenFlickerFilterField->SetData(&game_values.flickerfilter, NULL, NULL);
    miScreenFlickerFilterField->SetKey(game_values.flickerfilter);
    miScreenFlickerFilterField->SetNoWrap(true);
    miScreenFlickerFilterField->SetItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);

    miScreenSoftFilterField = new MI_SelectField(&rm->spr_selectfield, 70, 280, "Soften Filter", 500, 220);
    miScreenSoftFilterField->Add("Off", 0);
    miScreenSoftFilterField->Add("On", 1, "", true, false);
    miScreenSoftFilterField->SetData(&game_values.softfilter, NULL, NULL);
    miScreenSoftFilterField->SetKey(game_values.softfilter);
    miScreenSoftFilterField->SetAutoAdvance(true);
    miScreenSoftFilterField->SetItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);

    /*
    miScreenAspectRatioField = new MI_SelectField(&rm->spr_selectfield, 70, 300, "10x11 Aspect", 500, 220);
    miScreenAspectRatioField->Add("Off", 0);
    miScreenAspectRatioField->Add("On", 1, "", true, false);
    miScreenAspectRatioField->SetData(NULL, NULL, &game_values.aspectratio10x11);
    miScreenAspectRatioField->SetKey(game_values.aspectratio10x11 ? 1 : 0);
    miScreenAspectRatioField->SetAutoAdvance(true);
    miScreenAspectRatioField->SetItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);
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
