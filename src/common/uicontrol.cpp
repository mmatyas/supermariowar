#include "uicontrol.h"

#include <cassert>


UI_Control::UI_Control(short x, short y)
    : m_pos(x, y)
{
    m_neighbors.fill(nullptr);
}


UI_Control& UI_Control::operator= (const UI_Control& other)
{
    if (this != &other) {
        m_pos = other.m_pos;

        fSelected = other.fSelected;
        fModifying = other.fModifying;
        fAutoModify = other.fAutoModify;
        fDisable = other.fDisable;
        m_visible = other.m_visible;

        m_parentMenu = nullptr;
        m_neighbors.fill(nullptr);
        iControllingTeam = other.iControllingTeam;
    }
    return *this;
}


UI_Control::UI_Control(const UI_Control& other)
{
    *this = other;
}


void UI_Control::setNeighbor(MenuNavDirection iNeighbor, UI_Control* uiControl)
{
    const auto idx = static_cast<std::size_t>(iNeighbor);
    m_neighbors[idx] = uiControl;
}
