#include "uimenu.h"

#include "GameValues.h" // UI_Menu::SendInput
#include "uicontrol.h"

extern short LookupTeamID(short id);

extern CGameValues game_values;

UI_Menu::UI_Menu()
{
	cancelCode = MENU_CODE_NONE;
	fModifyingItem = false;
	headControl = NULL;
	current = NULL;
	savedCurrent = NULL;

	iControllingTeam = -1;
}

UI_Menu::~UI_Menu()
{
	std::list<UI_Control*>::iterator iterateAll = controls.begin();

    while (iterateAll != controls.end()) {
        delete (*iterateAll); // Can CRASH!
		iterateAll++;
	}

	controls.clear();
}


void UI_Menu::AddControl(UI_Control * control, UI_Control * up, UI_Control * down, UI_Control * left, UI_Control * right)
{
	control->SetMenuParent(this);
	controls.push_back(control);
	control->SetNeighbor(MENU_ITEM_NEIGHBOR_UP, up);
	control->SetNeighbor(MENU_ITEM_NEIGHBOR_DOWN, down);
	control->SetNeighbor(MENU_ITEM_NEIGHBOR_LEFT, left);
	control->SetNeighbor(MENU_ITEM_NEIGHBOR_RIGHT, right);
}

void UI_Menu::AddNonControl(UI_Control * control)
{
	control->SetMenuParent(this);
	controls.push_back(control);
}

void UI_Menu::SetHeadControl(UI_Control * control)
{
	headControl = control;
	ResetMenu();
}

void UI_Menu::ResetMenu()
{
    if (current) {
		current->Modify(false);
		current->Select(false);
	}

	current = headControl;

	if (current)
		fModifyingItem = current->Select(true);

	eyeCandy.clean();
}

void UI_Menu::Update()
{
	std::list<UI_Control*>::iterator iterateAll = controls.begin();

    while (iterateAll != controls.end()) {
		(*iterateAll)->Update();
		iterateAll++;
	}

	eyeCandy.cleandeadobjects();
	eyeCandy.update();
}

void UI_Menu::Draw()
{
	std::list<UI_Control*>::iterator iterateAll = controls.begin();

    while (iterateAll != controls.end()) {
		(*iterateAll)->Draw();
		iterateAll++;
	}

	eyeCandy.draw();
}

MenuCodeEnum UI_Menu::SendInput(CPlayerInput * playerInput)
{
    if (fModifyingItem) {
		MenuCodeEnum ret = current->SendInput(playerInput);

        if (MENU_CODE_UNSELECT_ITEM == ret) {
			fModifyingItem = false;

			/*if (current->IsAutoModify())
				return cancelCode;*/

			return MENU_CODE_NONE;
		}

		return ret;
	}

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
		//Only allow the controlling team to control the menu (if there is one)
        if (iControllingTeam != -1) {
			//Pay attention to other player's exit button pushes so we can exit when AI is controlling
			if (game_values.playercontrol[iPlayer] != 1 || (iControllingTeam != LookupTeamID(iPlayer) && (!fAllowExitButton || !playerInput->outputControls[iPlayer].menu_cancel.fPressed)))
				continue;
		}
		//Only let player 1 on the keyboard control the menu unless there is another controlling team
        else if (iPlayer != 0 && game_values.playerInput.inputControls[iPlayer] && game_values.playerInput.inputControls[iPlayer]->iDevice == DEVICE_KEYBOARD) {
			continue;
		}

        if (playerInput->outputControls[iPlayer].menu_up.fPressed) {
			return MoveNextControl(MENU_CODE_NEIGHBOR_UP);
		}

        if (playerInput->outputControls[iPlayer].menu_down.fPressed) {
			return MoveNextControl(MENU_CODE_NEIGHBOR_DOWN);
		}

        if (playerInput->outputControls[iPlayer].menu_left.fPressed) {
			return MoveNextControl(MENU_CODE_NEIGHBOR_LEFT);
		}

        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
			return MoveNextControl(MENU_CODE_NEIGHBOR_RIGHT);
		}

        if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
			MenuCodeEnum ret = MENU_CODE_NONE;

            if (current) {
				ret = current->Modify(true);

                if (MENU_CODE_MODIFY_ACCEPTED == ret) {
					fModifyingItem = true;
					return MENU_CODE_NONE;
				}

                if (MENU_CODE_UNSELECT_ITEM == ret) {
					fModifyingItem = false;
					return MENU_CODE_NONE;
				}
			}

			return ret;
		}

        if (playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
			return cancelCode;
		}
	}

	return MENU_CODE_NONE;
}

MenuCodeEnum UI_Menu::MoveNextControl(MenuCodeEnum iDirection)
{
	if (!current)
		return MENU_CODE_NONE;

	UI_Control * neighbor = current->GetNeighbor(iDirection);

    while (neighbor && !neighbor->IsVisible()) {
		neighbor = neighbor->GetNeighbor(iDirection);
	}

    if (neighbor) {
		current->Select(false);
		current = neighbor;
		fModifyingItem = current->Select(true);
		return iDirection;
	}

	return MENU_CODE_NONE;
}

void UI_Menu::RememberCurrent()
{
	savedCurrent = current;
}

void UI_Menu::RestoreCurrent()
{
    if (current) {
		current->Modify(false);
		current->Select(false);
	}

	current = savedCurrent;

	if (current)
		fModifyingItem = current->Select(true);

	eyeCandy.clean();
}

MenuCodeEnum UI_Menu::MouseClick(short iMouseX, short iMouseY)
{
	//Loop through all controls to see if one was clicked on
	std::list<UI_Control*>::iterator itr = controls.begin(), lim = controls.end();
	UI_Control * pFound = NULL;
	MenuCodeEnum code = MENU_CODE_NONE;
    while (itr != lim) {
        if ((*itr)->IsVisible()) {
			code = (*itr)->MouseClick(iMouseX, iMouseY);

            if (code != MENU_CODE_NONE) {
				pFound = *itr;
				break;
			}
		}

		itr++;
	}

    if (pFound) {
		//If we clicked the same control we have selected
        if (pFound != current) {
            if (fModifyingItem) {
				current->Modify(false);
				fModifyingItem = false;
			}

			current->Select(false);
			current = pFound;
			fModifyingItem = current->Select(true);

            if (!fModifyingItem) {
				fModifyingItem = current->Modify(true) == MENU_CODE_MODIFY_ACCEPTED;
			}
        } else {
            if (!fModifyingItem) {
				fModifyingItem = current->Modify(true) == MENU_CODE_MODIFY_ACCEPTED;
			}
		}
    } else {
		//If nothing was clicked, then stop modifying the current control
        if (fModifyingItem) {
			current->Modify(false);
			fModifyingItem = false;
		}
	}

	return code;
}

void UI_Menu::Refresh()
{
	std::list<UI_Control*>::iterator itr = controls.begin(), lim = controls.end();
    while (itr != lim) {
		(*itr)->Refresh();
		itr++;
	}
}
