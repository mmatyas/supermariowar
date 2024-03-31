#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;


class UI_EyeCandyOptionsMenu : public UI_Menu {
public:
    UI_EyeCandyOptionsMenu();

private:
    MI_SelectFieldDyn<short>* miSpawnStyleField;
    MI_SelectFieldDyn<short>* miAwardStyleField;
    MI_SelectFieldDyn<short>* miScoreStyleField;
    MI_SelectFieldDyn<bool>* miCrunchField;
    MI_SelectFieldDyn<bool>* miWinningCrownField;
    MI_SelectFieldDyn<bool>* miStartCountDownField;
    MI_SelectFieldDyn<bool>* miStartModeDisplayField;
    MI_SelectFieldDyn<bool>* miDeadTeamNoticeField;

    MI_Button* miEyeCandyOptionsMenuBackButton;

    MI_Image* miEyeCandyOptionsMenuLeftHeaderBar;
    MI_Image* miEyeCandyOptionsMenuRightHeaderBar;
    MI_Text* miEyeCandyOptionsMenuHeaderText;
};
