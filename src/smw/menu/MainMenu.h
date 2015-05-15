#ifndef MENU_MAIN_H
#define MENU_MAIN_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
	This is the main menu you see at the start of the game.
*/

class UI_MainMenu : public UI_Menu
{
public:
	UI_MainMenu();
	~UI_MainMenu();

private:
	MI_Image * miSMWTitle;
	MI_Image * miSMWVersion;
	//MI_Text * miSMWVersionText;

	MI_Button * miMainStartButton;
	MI_Button * miQuickGameButton;
	MI_Button * miMultiplayerButton;

	MI_PlayerSelect * miPlayerSelect;

	MI_Button * miOptionsButton;
	MI_Button * miControlsButton;

	MI_Button * miExitButton;
};

#endif // MENU_MAIN_H
