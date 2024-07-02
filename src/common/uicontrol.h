#pragma once

#include "ui/MenuCode.h"
#include "math/Vec2.h"

#include <array>

class CPlayerInput;
class UI_Menu;


enum class TextAlign: unsigned char {
    LEFT,
    CENTER,
    RIGHT,
};


class UI_Control
{
public:
    UI_Control(short x, short y);
    UI_Control(const UI_Control&);
    UI_Control& operator=(const UI_Control&);
    virtual ~UI_Control() = default;

    /// Updates animations or other events every frame
    virtual void Update() {}

    /// Draws every frame
    virtual void Draw() {}

    /// Sends player input to control on every frame
    virtual MenuCodeEnum SendInput(CPlayerInput*) {
        return MENU_CODE_NONE;
    }

    bool Select(bool select) {
        fSelected = select;

        if (fSelected && fAutoModify)
            Modify(true);

        return fModifying;
    }

    virtual MenuCodeEnum Modify(bool modify) {
        if (fDisable)
            return MENU_CODE_UNSELECT_ITEM;

        fModifying = modify;
        return MENU_CODE_MODIFY_ACCEPTED;
    }

    void SetAutoModify(bool autoModify) {
        fAutoModify = autoModify;
    }
    bool IsAutoModify() const {
        return fAutoModify;
    }

    void SetPosition(short x, short y) {
        m_pos = {x, y};
    }

    void SetNeighbor(unsigned short iNeighbor, UI_Control* uiControl);

    UI_Control* GetNeighbor(short iNeighbor) const {
        return neighborControls[iNeighbor];
    }

    void Show(bool show) {
        fShow = show;
    }
    bool IsVisible() const {
        return fShow;
    }

    void SetMenuParent(UI_Menu* menu) {
        uiMenu = menu;
    }

    bool IsModifying() const {
        return fModifying;
    }

    void SetControllingTeam(short teamid) {
        iControllingTeam = teamid;
    }

    virtual MenuCodeEnum MouseClick(short iMouseX, short iMouseY) {
        return MENU_CODE_NONE;
    }

    virtual void Refresh() {}

    virtual void Disable(bool disable) {
        fDisable = disable;
    }

protected:
    Vec2s m_pos = Vec2s::zero();

    bool fSelected = false;
    bool fModifying = false;
    bool fAutoModify = false;
    bool fDisable = false;
    bool fShow = true;

    std::array<UI_Control*, 4> neighborControls;

    UI_Menu* uiMenu = nullptr;
    short iControllingTeam = -1;
};
