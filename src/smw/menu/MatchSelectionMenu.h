#pragma once

#include "uimenu.h"
#include "MatchTypes.h"

class MI_Button;
class MI_Image;
class MI_Text;
class MI_WorldPreviewDisplay;
template<typename T> class MI_SelectField;


class UI_MatchSelectionMenu : public UI_Menu {
public:
    UI_MatchSelectionMenu();

    void SelectionChanged();
    void WorldMapChanged();
    void ActivateMinigameField();
    short GetMinigameID();
    MatchType GetSelectedMatchType();

private:
    MI_Image* miMatchSelectionDisplayImage;

    MI_SelectField<MatchType>* miMatchSelectionField;
    MI_SelectField<short>* miTournamentField;
    MI_SelectField<short>* miTourField;
    MI_SelectField<short>* miWorldField;
    MI_SelectField<short>* miMinigameField;
    MI_WorldPreviewDisplay* miWorldPreviewDisplay;

    MI_Button* miMatchSelectionStartButton;

    MI_Image* miMatchSelectionMenuLeftHeaderBar;
    MI_Image* miMatchSelectionMenuRightHeaderBar;
    MI_Text* miMatchSelectionMenuHeaderText;
};
