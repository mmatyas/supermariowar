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

    /// Called when user selects this control to change it's value
    virtual MenuCodeEnum Modify(bool modify) {
        if (fDisable)
            return MENU_CODE_UNSELECT_ITEM;

        fModifying = modify;
        return MENU_CODE_MODIFY_ACCEPTED;
    }

    virtual MenuCodeEnum MouseClick(short iMouseX, short iMouseY) {
        return MENU_CODE_NONE;
    }

    virtual void Refresh() {}

    virtual void Disable(bool disable) {
        fDisable = disable;
    }

    bool Select(bool select) {
        fSelected = select;

        if (fSelected && fAutoModify)
            Modify(true);

        return fModifying;
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

    void setNeighbor(MenuNavDirection iNeighbor, UI_Control* uiControl);

    UI_Control* neighbor(MenuNavDirection iNeighbor) const {
        const auto idx = static_cast<std::size_t>(iNeighbor);
        return m_neighbors[idx];
    }

    void setVisible(bool show) {
        m_visible = show;
    }
    bool IsVisible() const {
        return m_visible;
    }

    void setParent(UI_Menu* menu) {
        m_parentMenu = menu;
    }

    bool IsModifying() const {
        return fModifying;
    }

    void SetControllingTeam(short teamid) {
        iControllingTeam = teamid;
    }

protected:
    Vec2s m_pos = Vec2s::zero();

    bool fSelected = false;
    bool fModifying = false;
    bool fAutoModify = false;
    bool fDisable = false;
    bool m_visible = true;

    std::array<UI_Control*, 4> m_neighbors;

    UI_Menu* m_parentMenu = nullptr;
    short iControllingTeam = -1;
};
