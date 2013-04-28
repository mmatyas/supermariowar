
#include "global.h"
#include "Game.h"

#if	_WIN32
#include <windows.h>
#endif

#include "path.h"

extern void RunGame();

CGame::CGame(char *rootDirectory)
{
    // make sure that the .smw directory is created
    std::string smwHome = GetHomeDirectory();

#if	_WIN32
    if (CreateDirectory(smwHome .c_str(), NULL) ||
        ERROR_ALREADY_EXISTS == GetLastError())
    {
#pragma warning print log message about success
    } else {
#pragma warning print log message about error
    }
#endif

}

void CGame::Go()
{
    g_Menu.CreateMenu();
    g_Menu.RunMenu();

    while(game_values.gamestate != GS_QUIT)
    {
        switch(game_values.gamestate)
        {
            case GS_START_GAME:
            case GS_GAME:
                RunGame();
            break;

            case GS_MENU:
                g_Menu.RunMenu();
            break;

            case GS_START_WORLD:
            case GS_END_GAME:
            case GS_QUIT: // added because of warning on not handling all of enum
            break;
        }
    }	

}

CGame::~CGame(void)
{
}
