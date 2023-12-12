#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_TournamentScoreboard;


class UI_TournamentScoreboardMenu : public UI_Menu {
public:
    UI_TournamentScoreboardMenu();

    MI_TournamentScoreboard* miTournamentScoreboard;

private:
    MI_Button* miTournamentScoreboardNextButton;
    MI_Image* miTournamentScoreboardImage;
};
