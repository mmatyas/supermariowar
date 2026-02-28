#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Call Initialize_Paths() when your application launches */

bool FileExists(const std::string path);

/* All filenames must go through this door */
// TODO: Return fs::path
std::string convertPath(const std::string& source);
std::string convertPath(std::string_view relpath, const std::filesystem::path& packdir);

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
