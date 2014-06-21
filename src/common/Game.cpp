#include "Game.h"

#if	_WIN32
#include <windows.h>
#endif

#include "path.h"

CGame::CGame(char *rootDirectory)
{
    // make sure that the .smw directory is created
    std::string smwHome = GetHomeDirectory();

#if	_WIN32
    if (CreateDirectory(smwHome .c_str(), NULL) ||
            ERROR_ALREADY_EXISTS == GetLastError()) {
    	//TODO: print that directory already exists

    } else {
    	//TODO: print that we created the directory
    }
#else
    //TODO: create directory in MacOSX/Linux systems
#endif

#ifdef _XBOX
    	//TODO: make this configurable!
    	//TV's seem to need extra shade
    	MenuTransparency = 96;
#else
    	MenuTransparency = 72;
#endif

    ScreenWidth = 640;
    ScreenHeight = 480;
}

CGame::~CGame(void)
{
}
