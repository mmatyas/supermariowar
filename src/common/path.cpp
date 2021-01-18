#include "path.h"

#include <cstring>
#include <string>
#include <sys/stat.h>
#include <stdlib.h>


extern std::string RootDataDirectory;

using namespace std;
std::string SMW_Root_Data_Dir;

const std::string GetHomeDirectory()
{
    std::string result(".smw/");
    char* folder = getenv("HOME");
    if (folder)
        result = std::string(folder) + "/" + result;
    return result;
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
    string s;
    static bool are_paths_initialized = false;

    if (!are_paths_initialized) {
//		#ifdef PREFIXPATH
		SMW_Root_Data_Dir = RootDataDirectory;
//		#endif

        size_t last_slash_pos = SMW_Root_Data_Dir.find_last_of("/");
        if (last_slash_pos != SMW_Root_Data_Dir.length() - 1)
            SMW_Root_Data_Dir += "/";

		are_paths_initialized = true;
    }

    s = SMW_Root_Data_Dir;
    s += source;
    return s;
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

const string getDirectorySeperator()
{
    return std::string("/");
}

const string convertPartialPath(const string & source)
{
	string s = source;
    return s;
}

const string getFileFromPath(const string &path)
{
	short iPos = path.find_last_of(getDirectorySeperator()[0]);

	if (iPos > 0)
		return path.substr(iPos + 1);

	return path;
}

//Takes a path to a file and gives you back the file name (with or without author) as a char *
void GetNameFromFileName(char * szName, const char * szFileName, bool fStripAuthor)
{
    const char * p = strrchr(szFileName, '/');

    if (!p)
        p = szFileName;
    else
        p++;

    strcpy(szName, p);

    if (fStripAuthor) {
        char * pUnderscore = strchr(szName, '_');
        if (pUnderscore)
            strcpy(szName, ++pUnderscore);
    }

    char * pLastPeriod = strrchr(szName, '.');

    if (pLastPeriod)
        *pLastPeriod = 0;
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
