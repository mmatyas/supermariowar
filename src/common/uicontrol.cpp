#include "uicontrol.h"

#include <cassert>


UI_Control::UI_Control(short x, short y)
    : m_pos(x, y)
{
    neighborControls.fill(nullptr);
}


UI_Control& UI_Control::operator= (const UI_Control& other)
{
    if (this != &other) {
        m_pos = other.m_pos;

        fSelected = other.fSelected;
        fModifying = other.fModifying;
        fAutoModify = other.fAutoModify;
        fDisable = other.fDisable;
        fShow = other.fShow;

        uiMenu = nullptr;
        neighborControls.fill(nullptr);
        iControllingTeam = other.iControllingTeam;
    }
    return *this;
}


UI_Control::UI_Control(const UI_Control& other)
{
    *this = other;
}


void UI_Control::SetNeighbor(unsigned short iNeighbor, UI_Control* uiControl)
{
    assert(iNeighbor < neighborControls.size());
    neighborControls[iNeighbor] = uiControl;
}
