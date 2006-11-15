#include <string>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
	#ifndef _XBOX
		#include <SDL_platform.h>
	#endif
#endif

#include "path.h"

using namespace std;
char SMW_Root_Data_Dir[PATH_MAX + 2] = "";

bool File_Exists (const std::string fileName)
{
   struct stat buffer;
   int i = stat(fileName.c_str(), &buffer);
   
   return (i == 0);
}

/*********************************************************************
  Mac OS X Application Bundles                              *********/

#ifdef __MACOSX__
#include <CoreFoundation/CoreFoundation.h>

/* Call this when your application launches */
void Initialize_Paths()
{
    UInt8 temp[PATH_MAX];
    CFBundleRef mainBundle;
    CFURLRef dirURL;
    mainBundle = CFBundleGetMainBundle();
    if (mainBundle == NULL) {
        cout << "Not running from an .app bundle" << endl;
        return;
    }
    dirURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    if (dirURL == NULL) {
        cout << "Could not get resources directory URL" << endl;
        return;
    }
    if (CFURLGetFileSystemRepresentation(dirURL, TRUE, temp,
        PATH_MAX) == NULL) {
        cout << "Could not get file system representation" << endl;
        return;
    }
    CFRelease(dirURL);
    cout << "Located data folder at: " << SMW_Root_Data_Dir << endl;
}
#endif

/*********************************************************************
  All filenames should go through this                      *********/

const string convertPath(const string& source)
{
    string s;

/****** XBOX ******/
#ifdef _XBOX
	
	s = source;
    int slash = string :: npos;

    while (string::npos != (slash = s.find("/")))
        s.replace(slash, 1, "\\");

    s = "D:\\" + s;
	return s;
#else
    static bool are_paths_initialized = false;

    if (!are_paths_initialized) 
	{

	#ifdef PREFIXPATH
        strcpy(SMW_Root_Data_Dir, PREFIXPATH);
	#endif

	#ifdef __MACOSX__
        Initialize_Paths();
	#endif

	#ifndef _WIN32
        strcat(SMW_Root_Data_Dir, "/");
	#endif
    
		are_paths_initialized = true;
    }

    s = SMW_Root_Data_Dir;
#endif
    s += source;
    return s;
}

const string convertPath(const string& source, const string& pack)
{
	if(source.find("gfx/packs/") == 0)
	{
		string trailingdir = source.substr(9);
		const string s = convertPath(pack + trailingdir);

		//If the file exists, return the path to it
		if(File_Exists(s))
			return s;

		//If not, use the classic file
		return convertPath("gfx/packs/Classic" + trailingdir);
	}

	if(source.find("sfx/packs/") == 0)
	{
		string trailingdir = source.substr(9);
		const string s = convertPath(pack + trailingdir);

		//If the file exists, return the path to it
		if(File_Exists(s))
			return s;

		//If not, use the classic file
		return convertPath("sfx/packs/Classic" + trailingdir);
	}

	return convertPath(source);
}

const string getDirectorySeperator()
{
#ifdef _XBOX
	return std::string("\\");
#else
	return std::string("/");
#endif
}

const string convertPartialPath(const string & source)
{
	string s = source;

#ifdef _XBOX
    int slash = string :: npos;

    while (string::npos != (slash = s.find("/")))
        s.replace(slash, 1, "\\");
#endif

    return s;
}

const string getFileFromPath(const string &path)
{
	short iPos = path.find_last_of('/');

	if(iPos > 0)
		return path.substr(iPos + 1);

	return path;
}
