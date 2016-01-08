#ifndef MENU_EYECANDYOPTIONS_H
#define MENU_EYECANDYOPTIONS_H

/*
    TODO: Description.
*/

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_SelectField;
class MI_Text;

class UI_EyeCandyOptionsMenu : public UI_Menu
{
public:
    UI_EyeCandyOptionsMenu();
    ~UI_EyeCandyOptionsMenu();

private:
    MI_SelectField * miSpawnStyleField;
    MI_SelectField * miAwardStyleField;
    MI_SelectField * miScoreStyleField;
    MI_SelectField * miCrunchField;
    MI_SelectField * miWinningCrownField;
    MI_SelectField * miStartCountDownField;
    MI_SelectField * miStartModeDisplayField;
    MI_SelectField * miDeadTeamNoticeField;

    MI_Button * miEyeCandyOptionsMenuBackButton;

    MI_Image * miEyeCandyOptionsMenuLeftHeaderBar;
    MI_Image * miEyeCandyOptionsMenuRightHeaderBar;
    MI_Text * miEyeCandyOptionsMenuHeaderText;
};

#endif // MENU_EYECANDYOPTIONS_H
