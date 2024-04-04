#pragma once

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

inline std::string concat(const std::string& a, const std::string& b) { return a + b; }

constexpr char getDirectorySeperator() { return '/'; }
const std::string getFileFromPath(const std::string &path);

const std::string GetHomeDirectory();
const std::string GetRootDirectory();

#ifdef __APPLE__
    void Initialize_Paths();
#else
#  define Initialize_Paths();
#endif

std::string GetNameFromFileName(const std::string& path, bool strip_author = false);
std::string stripCreatorAndDotMap(const std::string &filename);
std::string stripPathAndExtension(const std::string &path);
