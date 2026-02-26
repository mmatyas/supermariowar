#ifndef LINFUNC_H
#define LINFUNC_H

#include <list>
#include <string>
#include <string_view>


char *inPlaceLowerCase(char *str);
void inPlaceLowerCase(std::string& str);
[[nodiscard]] std::string lowercase(std::string str);
bool cstr_ci_equals(const char* const a, const char* const b);

std::list<std::string_view> tokenize(std::string_view text, char delim, size_t maxsplit = -1);


//_DEBUG
#if 0
bool CopyFile(const char *src, const char *dest, bool dontOverwrite);
#endif

#endif // LINFUNC_H
