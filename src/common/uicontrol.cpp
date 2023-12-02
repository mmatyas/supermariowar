#include "uicontrol.h"

#include "GameValues.h"
#include "ResourceManager.h"

#include <cmath>
#include <cstring>

extern CGameValues game_values;
extern CResourceManager* rm;

UI_Control::UI_Control(short x, short y)
    : ix(x)
    , iy(y)
{
    neighborControls.fill(nullptr);
}

UI_Control& UI_Control::operator= (const UI_Control& other)
{
    if (this != &other) {
        ix = other.ix;
        iy = other.iy;

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
