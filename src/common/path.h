#ifndef PATH_H
#define PATH_H

#include <string>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Call Initialize_Paths() when your application launches */

bool File_Exists (const std::string fileName);
/* All filenames must go through this door */
#define convertPathC(s) convertPath(s).c_str()
const std::string convertPath(const std::string& source);

#define convertPathCP(s, p) convertPath(s, p).c_str()
const std::string convertPath(const std::string& source, const std::string& pack);

const std::string getDirectorySeperator();
const std::string convertPartialPath(const std::string & source);
const std::string getFileFromPath(const std::string &path);

const std::string GetHomeDirectory();

#ifdef __APPLE__
    void Initialize_Paths();
#else
#  define Initialize_Paths();
#endif

void GetNameFromFileName(char * szName, const char * szFileName, bool fStripAuthor = false);
std::string stripCreatorAndDotMap(const std::string &filename);
std::string stripPathAndExtension(const std::string &path);

#endif // PATH_H
