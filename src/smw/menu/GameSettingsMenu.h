#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_ImageSelectField;
class MI_MapField;
class MI_MapFilterScroll;
class MI_SelectField;
class MI_Text;


class UI_GameSettingsMenu : public UI_Menu {
public:
    UI_GameSettingsMenu();

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

    MI_MapFilterScroll* miMapFilterScroll = nullptr;
    MI_MapField* miMapField = nullptr;
    MI_SelectField* miGoalField[22];

private:
    MI_ImageSelectField* miModeField = nullptr;
    MI_Button* miModeSettingsButton = nullptr;
    MI_Button* miSettingsStartButton = nullptr;
    MI_Button* miMapFiltersButton = nullptr;
    MI_Button* miMapThumbnailsButton = nullptr;

    MI_Image* miMapFiltersOnImage = nullptr;

    MI_Image* miGameSettingsLeftHeaderBar = nullptr;
    MI_Image* miGameSettingsMenuRightHeaderBar = nullptr;
    MI_Text* miGameSettingsMenuHeaderText = nullptr;

    MI_Image* miGameSettingsExitDialogImage = nullptr;
    MI_Text* miGameSettingsExitDialogExitText = nullptr;
    MI_Text* miGameSettingsExitDialogTournamentText = nullptr;
    MI_Button* miGameSettingsExitDialogYesButton = nullptr;
    MI_Button* miGameSettingsExitDialogNoButton = nullptr;

friend class UI_NetNewRoomSettingsMenu;
};
