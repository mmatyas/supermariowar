#ifndef MENU_MATCHSELECTION_H
#define MENU_MATCHSELECTION_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_MatchSelectionMenu : public UI_Menu
{
public:
    UI_MatchSelectionMenu();
    ~UI_MatchSelectionMenu();

    void SelectionChanged();
    void WorldMapChanged();
    void ActivateMinigameField();
    short GetMinigameID();
    short GetSelectedMatchType();

private:
    MI_Image * miMatchSelectionDisplayImage;

    MI_SelectField * miMatchSelectionField;
    MI_SelectField * miTournamentField;
    MI_SelectField * miTourField;
    MI_SelectField * miWorldField;
    MI_SelectField * miMinigameField;
    MI_WorldPreviewDisplay * miWorldPreviewDisplay;

    MI_Button * miMatchSelectionStartButton;

    MI_Image * miMatchSelectionMenuLeftHeaderBar;
    MI_Image * miMatchSelectionMenuRightHeaderBar;
    MI_Text * miMatchSelectionMenuHeaderText;
};

#endif // MENU_MATCHSELECTION_H
