#ifndef MENU_NETNEWROOM_H
#define MENU_NETNEWROOM_H

/*
    TODO: Description.
*/

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_SelectField;
class MI_Text;
class MI_TextField;

class UI_NetNewRoomMenu : public UI_Menu
{
public:
    UI_NetNewRoomMenu();
    ~UI_NetNewRoomMenu();

    void CreateInProgress();
    void AbortCreate();
    void Restore();

private:
    MI_TextField * miNetNewRoomNameField;
    MI_TextField * miNetNewRoomPasswordField;
    MI_SelectField * miNetNewRoomPrivateToggle;
    MI_Button * miNetNewRoomCreateButton;
    MI_Button * miNetNewRoomBackButton;

    MI_Image * miNetNewRoomCreatingDialogImage;
    MI_Text * miNetNewRoomCreatingDialogText;

    MI_Image * miNetNewRoomLeftHeaderBar;
    MI_Image * miNetNewRoomRightHeaderBar;
    MI_Text * miNetNewRoomHeaderText;
};

#endif // MENU_NETNEWROOM_H
