#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_SelectField;
class MI_Text;
class MI_WorldPreviewDisplay;


class UI_MatchSelectionMenu : public UI_Menu {
public:
    UI_MatchSelectionMenu();

    void SelectionChanged();
    void WorldMapChanged();
    void ActivateMinigameField();
    short GetMinigameID();
    short GetSelectedMatchType();

private:
    MI_Image* miMatchSelectionDisplayImage;

    MI_SelectField* miMatchSelectionField;
    MI_SelectField* miTournamentField;
    MI_SelectField* miTourField;
    MI_SelectField* miWorldField;
    MI_SelectField* miMinigameField;
    MI_WorldPreviewDisplay* miWorldPreviewDisplay;

    MI_Button* miMatchSelectionStartButton;

    MI_Image* miMatchSelectionMenuLeftHeaderBar;
    MI_Image* miMatchSelectionMenuRightHeaderBar;
    MI_Text* miMatchSelectionMenuHeaderText;
};
