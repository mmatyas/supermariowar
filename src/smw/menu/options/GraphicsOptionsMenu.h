#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_PacksField;
class MI_SelectField;
class MI_Text;


/*
    You can switch between windowed and fullscreen mode,
    as well as change the graphic packs here.
*/
class UI_GraphicsOptionsMenu : public UI_Menu {
public:
    UI_GraphicsOptionsMenu();

private:
    MI_SelectField* miTopLayerField;
    MI_SelectField* miFrameLimiterField;

#ifdef _XBOX
    MI_Button* miScreenSettingsButton;
#else
    MI_SelectField* miFullscreenField;
#endif //_XBOX

    MI_PacksField* miMenuGraphicsPackField;
    MI_PacksField* miWorldGraphicsPackField;
    MI_PacksField* miGameGraphicsPackField;
    MI_Button* miGraphicsOptionsMenuBackButton;

    MI_Image* miGraphicsOptionsMenuLeftHeaderBar;
    MI_Image* miGraphicsOptionsMenuRightHeaderBar;
    MI_Text* miGraphicsOptionsMenuHeaderText;
};
