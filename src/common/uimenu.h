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

    UI_Control* GetHeadControl() const {
        return headControl;
    }

    void SetHeadControl(UI_Control* control);
    void ResetMenu();

    void SetCancelCode(MenuCodeEnum code) {
        cancelCode = code;
    }

    void Update();
    void Draw();

    void AddEyeCandy(CEyecandy* ec) {
        eyeCandy.add(ec);
    }
    void ClearEyeCandy() {
        eyeCandy.clean();
    }

    void ResetCurrentControl();
    MenuCodeEnum SendInput(CPlayerInput* playerInput);


    void RememberCurrent();
    void RestoreCurrent();

    UI_Control* GetCurrentControl() const {
        return current;
    }

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

    UI_Control* current = nullptr;
    UI_Control* savedCurrent = nullptr;

    MenuCodeEnum cancelCode = MENU_CODE_NONE;
    bool fModifyingItem = false;

    UI_Control* headControl = nullptr;

    CEyecandyContainer eyeCandy;

    short iControllingTeam = -1;
    bool fAllowExitButton = true;
};

#endif // UIMENU_H
