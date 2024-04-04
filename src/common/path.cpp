#include "path.h"

#ifdef USE_SDL2
#include "SDL.h"
#endif

#include <cassert>
#include <cstring>
#include <string>
#include <sys/stat.h>

#ifdef _WIN32
#include <shlobj.h>
#include "SDL_platform.h"
#else
#include <stdlib.h>
#endif


extern std::string RootDataDirectory;

using namespace std;
std::string SMW_Root_Data_Dir;

const std::string GetHomeDirectory()
{
#ifdef __APPLE__
    std::string result("/Library/Preferences/.smw/");
    char* folder = getenv("HOME");
    if (folder)
        result = std::string(folder) + result;
    return result;

#elif _WIN32
    std::string result(".smw/");
    char folder[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, folder) == S_OK)
        result = std::string(folder) + "/" + result;
    return result;

#elif ANDROID
    const char* extstorage = getenv("EXTERNAL_STORAGE");
    std::string result(extstorage ? extstorage: "/mnt/sdcard");
    result += "/supermariowar/";
    return result;

#else // catch-all for Linux-based systems
    std::string result(".smw/");
    char* folder = getenv("HOME");
    if (folder)
        result = std::string(folder) + "/" + result;
    return result;
#endif
}

const std::string GetRootDirectory()
{
#if !defined(_WIN32) && defined(USE_SDL2)
    // TODO: SDL_GetBasePath returns an UTF-8 string, which needs
    // some special treatment on Windows to work
    char* const sdl_path = SDL_GetBasePath();
    if (sdl_path) {
        std::string result = sdl_path;
        SDL_free(sdl_path);
        return result;
    }
#endif

    return "./";
}

bool File_Exists (const std::string fileName)
{
	struct stat buffer;
	int i = stat(fileName.c_str(), &buffer);

	return (i == 0);
}

/*********************************************************************
  Mac OS X Application Bundles                              *********/

#ifdef __APPLE__
#include <iostream>
#include <CoreFoundation/CoreFoundation.h>


/* Call this when your application launches */
void Initialize_Paths()
{
    if (!SMW_Root_Data_Dir.empty())
		return;

#ifdef APPBUNDLE
    UInt8 temp[PATH_MAX];
    CFBundleRef mainBundle;
    CFURLRef dirURL;
    mainBundle = CFBundleGetMainBundle();
    if (!mainBundle) {
        cout << "Not running from an .app bundle" << endl;
        return;
    }

	dirURL = CFBundleCopyBundleURL(mainBundle);

    if (!CFURLGetFileSystemRepresentation(dirURL, TRUE, temp,
        PATH_MAX)) {
        cout << "Could not get file system representation" << endl;
        return;
    }

    SMW_Root_Data_Dir = getFileFromPath(std::string((char*)temp));

    CFRelease(dirURL);
#else
	SMW_Root_Data_Dir = "./";
#endif
	cout << "Located data folder at: " << SMW_Root_Data_Dir << endl;
}
#endif

/*********************************************************************
  All filenames should go through this                      *********/

const string convertPath(const string& source)
{
    static bool are_paths_initialized = false;

    if (!are_paths_initialized) {
//		#ifdef PREFIXPATH
			SMW_Root_Data_Dir = RootDataDirectory;
//		#endif

		#ifdef __APPLE__
			Initialize_Paths();
		#endif

        size_t last_slash_pos = SMW_Root_Data_Dir.find_last_of("/");
        if (last_slash_pos != SMW_Root_Data_Dir.length() - 1)
            SMW_Root_Data_Dir += "/";

		are_paths_initialized = true;
    }

    return SMW_Root_Data_Dir + source;
}

const string convertPath(const string& source, const string& pack)
{
    if (source.find("gfx/packs/") == 0) {
		string trailingdir = source.substr(9);

		const string s = pack + trailingdir;

		//If the file exists, return the path to it
		if (File_Exists(s))
			return s;

		//If not, use the classic file
		return convertPath("gfx/packs/Classic" + trailingdir);
	}

    if (source.find("sfx/packs/") == 0) {
		string trailingdir = source.substr(9);

		const string s = pack + trailingdir;

		//If the file exists, return the path to it
		if (File_Exists(s))
			return s;

		//If not, use the classic file
		return convertPath("sfx/packs/Classic" + trailingdir);
	}

	return convertPath(source);
}

const string getFileFromPath(const string &path)
{
	short iPos = path.find_last_of(getDirectorySeperator());

	if (iPos > 0)
		return path.substr(iPos + 1);

	return path;
}

// Takes a path to a file and gives you back the file name (with or without author) as a char *
std::string GetNameFromFileName(const std::string& path, bool strip_author)
{
    constexpr char PATH_SEPARATOR = '/';

    size_t left_pos = 0;
    size_t right_pos = path.length();

    const size_t sep_pos = path.find_last_of(PATH_SEPARATOR);
    if (sep_pos != std::string::npos)
        left_pos = sep_pos + 1;

    if (strip_author) {
        const size_t underscore_pos = path.find_first_of('_', left_pos);
        if (underscore_pos != std::string::npos)
            left_pos = underscore_pos + 1;
    }

    const size_t dot_pos = path.find_last_of('.', right_pos);
    if (dot_pos != std::string::npos && left_pos < dot_pos)
        right_pos = dot_pos;

    assert(right_pos >= left_pos);
    return path.substr(left_pos, right_pos - left_pos);
}

//Takes a file name and gives you back just the name of the file with no author or file extention
//and the first letter of the name will come back capitalized
std::string stripCreatorAndDotMap(const std::string &filename)
{
    size_t firstUnderscore = filename.find("_");    //find first _
    if (firstUnderscore == std::string::npos)   //if not found start with first character
        firstUnderscore = 0;
    else
        firstUnderscore++;                      //we don't want the _

    std::string withoutPrefix = filename.substr(firstUnderscore);   //substring without bla_ and .map (length-4)
    withoutPrefix = withoutPrefix.substr(0, withoutPrefix.length() - 4);        //i have no idea why this doesn't work if i do it like this: (leaves .map if the map starts with an underscore)
    //                                                              return filename.substr(firstUnderscore, filename.length()-4);

    //Capitalize the first letter so the hash table sorting works correctly
    if (withoutPrefix[0] >= 97 && withoutPrefix[0] <= 122)
        withoutPrefix[0] -= 32;

    return withoutPrefix;
}

//Takes a path to a file and gives you back just the name of the file with no author or file extention
std::string stripPathAndExtension(const std::string &path)
{
    size_t chopHere = path.find("_");   //find first _
    if (chopHere == std::string::npos) {    //if not found, then find the beginning of the filename
        chopHere = path.find_last_of(getDirectorySeperator());  //find last /
        if (chopHere == std::string::npos)  //if not found, start with first character
            chopHere = 0;
        else
            chopHere++;                     //we don't want the /
    } else {
        chopHere++;                     //we don't want the _
    }

    std::string withoutPath = path.substr(chopHere);    //substring without bla_
    withoutPath = withoutPath.substr(0, withoutPath.length() - 4); //and without extension like .map (length-4)

    return withoutPath;
}
