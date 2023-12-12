#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_StringScroll;
class MI_Text;
class MI_TextField;


class UI_NetEditServersMenu : public UI_Menu {
public:
    UI_NetEditServersMenu();

    void Restore();
    void onPressAdd();
    void onPressEdit();
    void onPressDelete();
    void onEntrySelect();
    void onDialogOk();

private:
    void ReloadScroll();
    void ShowDialog();
    void HideDialog();

    enum MenuState {
        DEFAULT,
        ADD,
        EDIT,
        DELETE
    };
    MenuState currentState;

    MI_Button* miAddButton;
    MI_Button* miEditButton;
    MI_Button* miRemoveButton;
    MI_Button* miBackButton;

    MI_StringScroll* miServerScroll;

    MI_Text* miInstructionsText1;
    MI_Text* miInstructionsText2;

    MI_Text* miDialogTitle;
    MI_TextField* miDialogTextField;
    MI_Button* miDialogOK;
    MI_Button* miDialogCancel;

    char dialogTextData[128];

    MI_Image* miLeftHeaderBar;
    MI_Image* miRightHeaderBar;
    MI_Text* miHeaderText;
};
