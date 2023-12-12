#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
class MI_TourStop;


class UI_TourStopMenu : public UI_Menu {
public:
    UI_TourStopMenu();

    void OpenExitDialog();
    void CloseExitDialog();

    MI_TourStop* miTourStop;

private:
    MI_Image* miTourStopExitDialogImage;
    MI_Text* miTourStopExitDialogExitTourText;
    MI_Button* miTourStopExitDialogYesButton;
    MI_Button* miTourStopExitDialogNoButton;
};
