#include <string>
#include <iostream>
#include <sys/stat.h>
#include <cstring>

#ifdef _WIN32
	#ifdef _XBOX
		#include <xtl.h>
	#else
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
	if(SMW_Root_Data_Dir[0] != 0)
	{ 
		return; 
	}

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

	strlcat(SMW_Root_Data_Dir, (char*)temp, PATH_MAX); 
	int i = strlen(SMW_Root_Data_Dir) -1; 
	while(SMW_Root_Data_Dir[i] !='/'){ 
	   SMW_Root_Data_Dir[i] = 0; 
	   --i; 
	} 
	SMW_Root_Data_Dir[i] = 0; 

    CFRelease(dirURL);
#else
	strlcat(SMW_Root_Data_Dir, "./", 4);
#endif
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
//		#ifdef PREFIXPATH
			strcpy(SMW_Root_Data_Dir, RootDataDirectory);
//		#endif

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

#ifdef _XBOX
		const string s = convertPartialPath(pack + trailingdir);  //Hack because pack already has d:\ in it
#else
		const string s = convertPath(pack + trailingdir);
#endif

		//If the file exists, return the path to it
		if(File_Exists(s))
			return s;

		//If not, use the classic file
		return convertPath("gfx/packs/Classic" + trailingdir);
	}

	if(source.find("sfx/packs/") == 0)
	{
		string trailingdir = source.substr(9);

#ifdef _XBOX		
		const string s = convertPartialPath(pack + trailingdir);  //Hack because pack already has d:\ in it
#else
		const string s = convertPath(pack + trailingdir);
#endif

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
	short iPos = path.find_last_of(getDirectorySeperator()[0]);

	if(iPos > 0)
		return path.substr(iPos + 1);

	return path;
}

