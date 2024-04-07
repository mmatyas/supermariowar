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


void ensureSettingsDir()
{
    const std::string smwHome = GetHomeDirectory();

#if	_WIN32
    const bool success = CreateDirectory(smwHome.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
    if (!success)
        perror("[error] Could not create settings directory");

#else
    struct stat st;
    if (stat(smwHome.c_str(), &st) != 0) { // inode does not exist
        if (mkdir(smwHome.c_str(), 0775) != 0)
            perror("[error] Could not create settings directory");
    }
    else if (!S_ISDIR(st.st_mode)) {  // inode exist, but not a directory
        perror("[error] Could not access settings directory");
    }

#endif
}
