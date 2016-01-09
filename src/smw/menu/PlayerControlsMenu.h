#ifndef MENU_PLAYERCONTROLSMENU_H
#define MENU_PLAYERCONTROLSMENU_H

#include "uimenu.h"
#include "ui/MI_InputControlContainer.h"

/*
	This is where the selected player (from PlayerControlsSelectMenu)
	can set the preferred control device and keys.
*/

class UI_PlayerControlsMenu : public UI_Menu
{
public:
	UI_PlayerControlsMenu();
	~UI_PlayerControlsMenu();

	void SetPlayer(short playerID);

private:
	MI_InputControlContainer * miInputContainer;
};

#endif // MENU_PLAYERCONTROLSMENU_H
