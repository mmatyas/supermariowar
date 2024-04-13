#pragma once

#include <string>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Call Initialize_Paths() when your application launches */

bool FileExists(const std::string path);

/* All filenames must go through this door */
std::string convertPath(const std::string& source);
std::string convertPath(const std::string& source, const std::string& pack);

inline std::string concat(const std::string& a, const std::string& b) { return a + b; }

constexpr char dirSeparator() { return '/'; }
std::string getFilenameFromPath(const std::string& path);

std::string GetHomeDirectory();
std::string GetRootDirectory();

#ifdef __APPLE__
    void Initialize_Paths();
#else
#  define Initialize_Paths();
#endif

std::string GetNameFromFileName(const std::string& path, bool stripAuthor = false);
std::string stripCreatorAndExt(const std::string& filename);
std::string stripPathAndExtension(const std::string& path);
