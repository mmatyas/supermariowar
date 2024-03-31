#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectField;


class UI_EyeCandyOptionsMenu : public UI_Menu {
public:
    UI_EyeCandyOptionsMenu();

private:
    MI_SelectField<short>* miSpawnStyleField;
    MI_SelectField<short>* miAwardStyleField;
    MI_SelectField<short>* miScoreStyleField;
    MI_SelectField<bool>* miCrunchField;
    MI_SelectField<bool>* miWinningCrownField;
    MI_SelectField<bool>* miStartCountDownField;
    MI_SelectField<bool>* miStartModeDisplayField;
    MI_SelectField<bool>* miDeadTeamNoticeField;

    MI_Button* miEyeCandyOptionsMenuBackButton;

    MI_Image* miEyeCandyOptionsMenuLeftHeaderBar;
    MI_Image* miEyeCandyOptionsMenuRightHeaderBar;
    MI_Text* miEyeCandyOptionsMenuHeaderText;
};
