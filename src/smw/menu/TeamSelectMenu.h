#pragma once

#include "uimenu.h"

class MI_Image;
class MI_TeamSelect;
class MI_Text;


class UI_TeamSelectMenu : public UI_Menu {
public:
    UI_TeamSelectMenu();

    void ResetTeamSelect();
    short GetTeamCount() const;

private:
    MI_TeamSelect* miTeamSelect;

    MI_Image* miTeamSelectLeftHeaderBar;
    MI_Image* miTeamSelectRightHeaderBar;
    MI_Text* miTeamSelectHeaderText;
};
