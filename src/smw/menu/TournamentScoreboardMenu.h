#ifndef MENU_TOURNAMENTSCOREBOARD_H
#define MENU_TOURNAMENTSCOREBOARD_H

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_TournamentScoreboard;

/*
    Description.
*/

class UI_TournamentScoreboardMenu : public UI_Menu
{
public:
    UI_TournamentScoreboardMenu();
    ~UI_TournamentScoreboardMenu();

    MI_TournamentScoreboard * miTournamentScoreboard;

private:
    MI_Button * miTournamentScoreboardNextButton;
    MI_Image * miTournamentScoreboardImage;
};

#endif // MENU_TOURNAMENTSCOREBOARD_H
