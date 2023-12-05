#pragma once

#include "uimenu.h"

class MI_MapBrowser;


class UI_MapFilterEditMenu : public UI_Menu {
public:
    UI_MapFilterEditMenu();

    MI_MapBrowser* miMapBrowser = nullptr;
};
