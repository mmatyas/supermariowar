#include "Game.h"

#include "path.h"

#include <cstdio>

#if _WIN32
#include <windows.h>
#endif

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

CGame::CGame(const char *rootDirectory)
{
    // make sure that the .smw directory is created
    std::string smwHome = GetHomeDirectory();
    struct stat st;
    if (stat(smwHome.c_str(), &st) != 0) { // inode does not exist
        if (mkdir(smwHome.c_str(), 0775) != 0)
            perror("[error] Could not create settings directory");
    }
    else if (!S_ISDIR(st.st_mode)) // inode exist, but not a directory
        perror("[error] Could not access settings directory");

    MenuTransparency = 72;
    ScreenWidth = 640;
    ScreenHeight = 480;
}

CGame::~CGame(void)
{
}
