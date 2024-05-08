#include "uimenu.h"

#include "GameValues.h" // UI_Menu::SendInput
#include "uicontrol.h"

extern short LookupTeamID(short id);

extern CGameValues game_values;


void UI_Menu::AddControl(UI_Control* control, UI_Control* up, UI_Control* down, UI_Control* left, UI_Control* right)
{
    control->SetMenuParent(this);
    control->SetNeighbor(MENU_ITEM_NEIGHBOR_UP, up);
    control->SetNeighbor(MENU_ITEM_NEIGHBOR_DOWN, down);
    control->SetNeighbor(MENU_ITEM_NEIGHBOR_LEFT, left);
    control->SetNeighbor(MENU_ITEM_NEIGHBOR_RIGHT, right);
    controls.emplace_back(control);
}

void UI_Menu::AddNonControl(UI_Control* control)
{
    control->SetMenuParent(this);
    controls.emplace_back(control);
}

void UI_Menu::setInitialFocus(UI_Control* control)
{
    m_initialFocus = control;
    ResetMenu();
}

void UI_Menu::ResetMenu()
{
    if (m_currentFocus) {
        m_currentFocus->Modify(false);
        m_currentFocus->Select(false);
    }

    m_currentFocus = m_initialFocus;

    if (m_currentFocus)
        fModifyingItem = m_currentFocus->Select(true);

    eyeCandy.clean();
}

void UI_Menu::Update()
{
    for (const std::unique_ptr<UI_Control>& control : controls) {
        control->Update();
    }

    eyeCandy.cleanDeadObjects();
    eyeCandy.update();
}

void UI_Menu::Draw()
{
    for (const std::unique_ptr<UI_Control>& control : controls) {
        control->Draw();
    }

    eyeCandy.draw();
}

MenuCodeEnum UI_Menu::SendInput(CPlayerInput* playerInput)
{
    if (fModifyingItem) {
        MenuCodeEnum ret = m_currentFocus->SendInput(playerInput);

        if (MENU_CODE_UNSELECT_ITEM == ret) {
            fModifyingItem = false;

            /*if (current->IsAutoModify())
                    return cancelCode;*/

            return MENU_CODE_NONE;
        }

        return ret;
    }

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        // Only allow the controlling team to control the menu (if there is one)
        if (iControllingTeam != -1) {
            // Pay attention to other player's exit button pushes so we can exit when AI is controlling
            if (game_values.playercontrol[iPlayer] != 1 || (iControllingTeam != LookupTeamID(iPlayer) && (!fAllowExitButton || !playerInput->outputControls[iPlayer].menu_cancel.fPressed)))
                continue;
        }
        // Only let player 1 on the keyboard control the menu unless there is another controlling team
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

            if (m_currentFocus) {
                ret = m_currentFocus->Modify(true);

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
    if (!m_currentFocus)
        return MENU_CODE_NONE;

    UI_Control* neighbor = m_currentFocus->GetNeighbor(iDirection);

    while (neighbor && !neighbor->IsVisible()) {
        neighbor = neighbor->GetNeighbor(iDirection);
    }

    if (neighbor) {
        m_currentFocus->Select(false);
        m_currentFocus = neighbor;
        fModifyingItem = m_currentFocus->Select(true);
        return iDirection;
    }

    return MENU_CODE_NONE;
}

void UI_Menu::RememberCurrent()
{
    m_savedCurrent = m_currentFocus;
}

void UI_Menu::RestoreCurrent()
{
    if (m_currentFocus) {
        m_currentFocus->Modify(false);
        m_currentFocus->Select(false);
    }

    m_currentFocus = m_savedCurrent;

    if (m_currentFocus)
        fModifyingItem = m_currentFocus->Select(true);

    eyeCandy.clean();
}

MenuCodeEnum UI_Menu::MouseClick(short iMouseX, short iMouseY)
{
    // Loop through all controls to see if one was clicked on
    UI_Control* pFound = nullptr;
    MenuCodeEnum code = MENU_CODE_NONE;
    for (const std::unique_ptr<UI_Control>& control : controls) {
        if (control->IsVisible()) {
            code = control->MouseClick(iMouseX, iMouseY);
            if (code != MENU_CODE_NONE) {
                pFound = control.get();
                break;
            }
        }
    }

    if (pFound) {
        // If we clicked the same control we have selected
        if (pFound != m_currentFocus) {
            if (fModifyingItem) {
                m_currentFocus->Modify(false);
                fModifyingItem = false;
            }

            m_currentFocus->Select(false);
            m_currentFocus = pFound;
            fModifyingItem = m_currentFocus->Select(true);

            if (!fModifyingItem) {
                fModifyingItem = m_currentFocus->Modify(true) == MENU_CODE_MODIFY_ACCEPTED;
            }
        } else {
            if (!fModifyingItem) {
                fModifyingItem = m_currentFocus->Modify(true) == MENU_CODE_MODIFY_ACCEPTED;
            }
        }
    } else {
        // If nothing was clicked, then stop modifying the current control
        if (fModifyingItem) {
            m_currentFocus->Modify(false);
            fModifyingItem = false;
        }
    }

    return code;
}

void UI_Menu::Refresh()
{
    for (const std::unique_ptr<UI_Control>& control : controls) {
        control->Refresh();
    }
}
