#ifndef MENU_NET_EDIT_SERVER_LIST_H
#define MENU_NET_EDIT_SERVER_LIST_H

#include "uimenu.h"
#include "uicontrol.h"

#include "ui/MI_StringScroll.h"

/*
    TODO: Description.
*/

class UI_NetEditServersMenu : public UI_Menu
{
public:
    UI_NetEditServersMenu();
    ~UI_NetEditServersMenu();

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

    MI_Button * miAddButton;
    MI_Button * miEditButton;
    MI_Button * miRemoveButton;
    MI_Button * miBackButton;

    MI_StringScroll* miServerScroll;

    MI_Text * miInstructionsText1;
    MI_Text * miInstructionsText2;

    MI_Text * miDialogTitle;
    MI_TextField * miDialogTextField;
    MI_Button * miDialogOK;
    MI_Button * miDialogCancel;

    char dialogTextData[128];

    MI_Image * miLeftHeaderBar;
    MI_Image * miRightHeaderBar;
    MI_Text * miHeaderText;
};

#endif // MENU_NET_EDIT_SERVER_LIST_H
