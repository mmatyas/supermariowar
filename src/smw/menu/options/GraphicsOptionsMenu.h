#ifndef MENU_GRAPHICSOPTIONS_H
#define MENU_GRAPHICSOPTIONS_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    You can switch between windowed and fullscreen mode,
    as well as change the graphic packs here.
*/

class UI_GraphicsOptionsMenu : public UI_Menu
{
public:
    UI_GraphicsOptionsMenu();
    ~UI_GraphicsOptionsMenu();

private:
    MI_SelectField * miTopLayerField;
    MI_SelectField * miFrameLimiterField;

#ifdef _XBOX
    MI_Button * miScreenSettingsButton;
#else
    MI_SelectField * miFullscreenField;
#endif //_XBOX

    MI_PacksField * miMenuGraphicsPackField;
    MI_PacksField * miWorldGraphicsPackField;
    MI_PacksField * miGameGraphicsPackField;
    MI_Button * miGraphicsOptionsMenuBackButton;

    MI_Image * miGraphicsOptionsMenuLeftHeaderBar;
    MI_Image * miGraphicsOptionsMenuRightHeaderBar;
    MI_Text * miGraphicsOptionsMenuHeaderText;
};

#endif // MENU_GRAPHICSOPTIONS_H
