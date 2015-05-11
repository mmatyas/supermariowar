#ifndef MENU_OPTIONSMENU_H
#define MENU_OPTIONSMENU_H

#include "uimenu.h"
#include "uicontrol.h"

/*
	This is the main options menu, where you can select the
	settings category you want to modify, or regenerate the
	map cache.
*/

class UI_OptionsMenu : public UI_Menu
{
public:
	UI_OptionsMenu();
	~UI_OptionsMenu();

	void showThumbnailsPopup();
	void hideThumbnailsPopup();

private:
	MI_Button * miGameplayOptionsMenuButton;
	MI_Button * miTeamOptionsMenuButton;
	MI_Button * miPowerupOptionsMenuButton;
	MI_Button * miProjectilesOptionsMenuButton;
	MI_Button * miProjectilesLimitsMenuButton;
	MI_Button * miPowerupSettingsMenuButton;
	MI_Button * miGraphicsOptionsMenuButton;
	MI_Button * miEyeCandyOptionsMenuButton;
	MI_Button * miSoundOptionsMenuButton;
	MI_Button * miGenerateMapThumbsButton;

	MI_Button * miOptionsMenuBackButton;

	MI_Image * miOptionsMenuLeftHeaderBar;
	MI_Image * miOptionsMenuRightHeaderBar;
	MI_Text * miOptionsMenuHeaderText;

	MI_Image * miGenerateThumbsDialogImage;
	MI_Text * miGenerateThumbsDialogAreYouText;
	MI_Text * miGenerateThumbsDialogSureText;
	MI_Button * miGenerateThumbsDialogYesButton;
	MI_Button * miGenerateThumbsDialogNoButton;
};

#endif // MENU_OPTIONSMENU_H
