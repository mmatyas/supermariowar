#ifndef LINFUNC_H
#define LINFUNC_H

#include <list>
#include <string>


char *inPlaceLowerCase(char *str);
void inPlaceLowerCase(std::string& str);
bool cstr_ci_equals(const char* const a, const char* const b);

std::list<std::string> tokenize(const std::string& text, char delim);


//_DEBUG
#if 0
bool CopyFile(const char *src, const char *dest, bool dontOverwrite);
#endif

#endif // LINFUNC_H
