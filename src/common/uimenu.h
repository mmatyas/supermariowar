#ifndef UIMENU_H
#define UIMENU_H

#include "eyecandy.h"
#include "ui/MenuCode.h"

#include <memory>
#include <vector>

class UI_Control;
class CPlayerInput;


class UI_Menu {
public:
    virtual ~UI_Menu() = default;

    void AddControl(UI_Control* control, UI_Control* up, UI_Control* down, UI_Control* left, UI_Control* right);
    void AddNonControl(UI_Control* control);

    /// Sets the initially focused element on opening/resetting the menu.
    void setInitialFocus(UI_Control* control);
    /// The initially focused element on opening/resetting the menu.
    UI_Control* initialFocus() const { return m_initialFocus; }
    /// The currently focused element of the menu.
    UI_Control* currentFocus() const { return m_currentFocus; }

    void SetCancelCode(MenuCodeEnum code) {
        cancelCode = code;
    }

    void ResetMenu();

    MenuCodeEnum SendInput(CPlayerInput* playerInput);
    void Update();
    void Draw();

    void AddEyeCandy(CEyecandy* ec) {
        eyeCandy.add(ec);
    }
    void ClearEyeCandy() {
        eyeCandy.clean();
    }

    void RememberCurrent();
    void RestoreCurrent();

    void SetControllingTeam(short teamid) {
        iControllingTeam = teamid;
    }
    void SetAllowExit(bool allowExit) {
        fAllowExitButton = allowExit;
    }

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    bool IsModifying() const {
        return fModifyingItem;
    }

    void Refresh();

protected:
    MenuCodeEnum MoveNextControl(MenuCodeEnum iDirection);

    std::vector<std::unique_ptr<UI_Control>> controls;

    UI_Control* m_initialFocus = nullptr;
    UI_Control* m_currentFocus = nullptr;
    UI_Control* m_savedCurrent = nullptr;

    MenuCodeEnum cancelCode = MENU_CODE_NONE;
    bool fModifyingItem = false;

    CEyecandyContainer eyeCandy;

    short iControllingTeam = -1;
    bool fAllowExitButton = true;
};

#endif // UIMENU_H
