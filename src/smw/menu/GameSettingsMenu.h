#ifndef MENU_GAMESETTINGS_H
#define MENU_GAMESETTINGS_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_GameSettingsMenu : public UI_Menu
{
public:
    UI_GameSettingsMenu();
    ~UI_GameSettingsMenu();


    void OpenMapFilters();
    void CloseMapFilters();
    void OpenExitDialog();
    void CloseExitDialog();

    void SetHeaderText(const char*);
    void HideGMSettingsBtn();
    void GameModeChanged(short gmID);
    void RefreshGameModeButtons();
    short GetCurrentGameModeID();
    const char* GetCurrentMapName();
    bool IsOnStartBtn();

    MI_MapFilterScroll * miMapFilterScroll;
    MI_MapField * miMapField;
    MI_SelectField * miGoalField[22];

private:
    MI_ImageSelectField * miModeField;
    MI_Button * miModeSettingsButton;
    MI_Button * miSettingsStartButton;
    MI_Button * miMapFiltersButton;
    MI_Button * miMapThumbnailsButton;

    MI_Image * miMapFiltersOnImage;

    MI_Image * miGameSettingsLeftHeaderBar;
    MI_Image * miGameSettingsMenuRightHeaderBar;
    MI_Text * miGameSettingsMenuHeaderText;

    MI_Image * miGameSettingsExitDialogImage;
    MI_Text * miGameSettingsExitDialogExitText;
    MI_Text * miGameSettingsExitDialogTournamentText;
    MI_Button * miGameSettingsExitDialogYesButton;
    MI_Button * miGameSettingsExitDialogNoButton;

friend class UI_NetNewRoomSettingsMenu;
};

#endif // MENU_GAMESETTINGS_H
